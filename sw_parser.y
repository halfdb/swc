%{
#include "types.h"
#include "table.h"
#include "machine.h"
#include "compiler.h"

extern int yylex();
void yyerror(char const *);

char *type_names[] = {
  "VOID",
  "INT",
  "FLOAT",
  "BOOL",
  "STR"
};

char check_type(type_enum t1, type_enum t2);
//type_enum merge_type(type_enum t1, type_enum t2);

param_list first_param = NULL;

%}
%token ADDSYM SUBSYM MULSYM DIVSYM MODSYM ASSNSYM
%token ANDSYM ORSYM NOTSYM TRUESYM FALSESYM RELSYM
%token INTSYM BOOLSYM FLOATSYM VOIDSYM
%token AIDENTSYM BIDENTSYM STRSYM INUMSYM FNUMSYM
%token IFSYM ELSESYM FORSYM INSYM ELLIPSYM WHILESYM
%token SMCLSYM COMMASYM
%token LPASYM RPASYM LBRSYM RBRSYM
%token READSYM PRINTSYM CALLSYM RETSYM

%union {
  var_node var;
  func_item func;
  data_item data;
  unsigned long addr;
  expr_item expr;
  char flag;
}

%type <var> INTSYM BOOLSYM FLOATSYM VOIDSYM type
%type <var> AIDENTSYM BIDENTSYM ident
%type <func> param_list func_decl
%type <data> INUMSYM FNUMSYM STRSYM RELSYM
%type <addr> statement_list line_statement block_statement
%type <addr> assign_stat read_stat print_stat return_stat
%type <addr> if_stat if_pt1 for_stat while_stat str_arg_list
%type <expr> call_stat expression
%type <expr> bool_expr bool_term bool_factor alg_expr alg_term alg_factor
%type <flag> arg_list

%%
program:
  var_decl_list func_decl_list statement_list
;
var_decl_list:
  %empty
  |
  var_decl_list var_decl SMCLSYM
;
var_decl:
  type ident
  {
    if ($2.type != VOID) {
      // indicating redefinition
      // TODO redefinition error
    } else {
      var_node i = { .name = $2.name, .type=$1.type };
      add_var(i);
    }
  }
;
type:
  INTSYM
  |FLOATSYM
  |BOOLSYM
  |VOIDSYM
;
ident:
  AIDENTSYM
  |
  BIDENTSYM
;

func_decl_list:
  %empty
  |
  func_decl func_decl_list
;
func_decl:
  type ident LPASYM param_list RPASYM
  {
    func_item func = {
      .param_num = $4.param_num,
      .params = first_param,
      .name = $2.name,
      .ret_type = $1.type
    };
    func_item *f = find_func(func.name);
    if (f != NULL) {
      // TODO redefinition
    }
    unsigned long scope_num = add_func(func);
    dprint("func added, no=%lu\n", scope_num);
    current_scope = scope_num;
  }
  LBRSYM func_body RBRSYM
  { current_scope = 0; dprint("func decl finished\n"); }
  |
  type ident LPASYM RPASYM
  {
    func_item func = {
      .param_num = 0,
      .name = $2.name,
      .ret_type = $1.type
    };
    unsigned long scope_num = add_func(func);
    dprint("added, no=%lu\n", scope_num);
    current_scope = scope_num;
  }
  LBRSYM func_body RBRSYM
  { current_scope = 0; dprint("func decl finished\n"); }
;
param_list:
  type ident
  {
    $$.param_num = 1;
    $$.params = (param_list) malloc(sizeof(param_node));
    $$.params -> name = $2.name;
    $$.params -> type = $1.type;
    $$.params -> next = NULL;
    first_param = $$.params;
    dprint("param %lu: name=%s, type=%u\n", $$.param_num - 1, $$.params->name, $$.params->type);
  }
  |
  param_list COMMASYM type ident
  {
    $$.param_num = $1.param_num + 1;
    $$.params = (param_list) malloc(sizeof(param_node));
    $$.params -> name = $4.name;
    $$.params -> type = $3.type;
    $1.params -> next = $$.params; // the list starts with the right first param
    dprint("param %lu: name=%s, type=%u\n", $1.param_num, $$.params->name, $$.params->type);
  }
;
func_body:
  var_decl_list statement_list
  {
    generate_instruction(ERR); // TODO void return type
  }
;

statement_list:
  %empty
  {
    $$ = 0;
  }
  |
  statement_list line_statement SMCLSYM
  {
    $$ = $1 ? $1 : $2;
  }
  |
  statement_list block_statement
  {
    $$ = $1 ? $1 : $2;
    dprint("semicolon omitted\n");
  }
;
block_statement:
  if_stat|for_stat|while_stat
;
line_statement:
  %empty
  {
    $$ = 0;
  }
  |
  call_stat
  {
    $$ = $1.addr;
    if ($1.type != VOID) {
      // the return value is not used, pop it
      generate_instruction(POP);
    }
  }
  |assign_stat|read_stat|print_stat|return_stat
;

read_stat:
  READSYM LPASYM ident RPASYM
  {
    $$ = generate_instruction(READ, $3.locator);
  }
;
print_stat:
  PRINTSYM LPASYM
  {
    $<addr>$ = 0; // a unit for storing arg nums
  }
  str_arg_list RPASYM
  {
    $$ = $4;
    dprint("print %lu args\n", $<addr>3);
    generate_instruction(PRINT, $<addr>3);
  }
  |
  PRINTSYM LPASYM RPASYM
  {
    dprint("print w/o args\n");
    $$ = generate_instruction(PRINT, 0);
  }
;
str_arg_list:
  STRSYM
  {
    const_item item = add_const($1);
    $<addr>0 = 1;
    $$ = generate_instruction(PUSH, item);

    free($1.value.str);
  }
  |
  expression
  {
    $<addr>0 = 1;
    $$ = $1.addr;
  }
  |
  str_arg_list COMMASYM expression
  {
    $<addr>0 += 1;
    $$ = $1;
  }
  |
  str_arg_list COMMASYM STRSYM
  {
    const_item item = add_const($3);
    generate_instruction(PUSH, item);
    $<addr>0 += 1;
    $$ = $1;

    free($3.value.str);
  }
;
return_stat:
  RETSYM expression
  {
    dprint("return expression\n");
  }
  |
  RETSYM { dprint("return\n"); }
;
assign_stat:
  ident ASSNSYM expression
  {
    dprint("assign\n");
    generate_instruction(STORE, $1.locator);
  }
;
if_stat:
  if_pt1
  {
    $$ = $1;
    dprint("if..\n");
  }
  |
  if_pt1 ELSESYM LBRSYM statement_list RBRSYM
  {
    $$ = $1;
    dprint("if..else..\n");
  }
;
if_pt1:
  IFSYM bool_expr
  {
    $<addr>$ = generate_instruction(JZ, 0); // dummy
  }
  LBRSYM statement_list RBRSYM
  {
    $$ = $2.addr;
    change_instruction($<addr>3, JZ, ins_top);
  }
;
while_stat:
  WHILESYM bool_expr
  {
    $<addr>$ = generate_instruction(JZ, 0); // dummy
  }
  LBRSYM statement_list RBRSYM
  {
    $$ = $2.addr;
    unsigned long addr = generate_instruction(JUMP, $2.addr);
    change_instruction($<addr>3, JZ, addr+1);
  }
;
for_stat:
  /**
   * ... (alg_expr1)
   * STORE var
   * LOAD var #LABEL
   * ... (alg_expr2)
   * ALGO >=
   * JNZ #END
   * ... (statement_list)
   * JUMP #LABEL
   */
  FORSYM AIDENTSYM INSYM alg_expr
  {
    generate_instruction(STORE, $2.locator);
    $<addr>$ = generate_instruction(LOAD, $2.locator);

    free($2.name);
  }
  ELLIPSYM alg_expr
  {
    generate_instruction(ALGO, GE);
    $<addr>$ = generate_instruction(JNZ, 0); // placeholder
  }
  LBRSYM statement_list RBRSYM
  {
    $$ = $4.addr;
    unsigned long addr = generate_instruction(JUMP, $<addr>5);
    change_instruction($<addr>8, JNZ, addr+1);
  }
;
call_stat:
  CALLSYM AIDENTSYM LPASYM
  {
    func_item *func = find_func($2.name);
    free($2.name);

    if (NULL == func) {
      // function does not exist
      // TODO
    }
    if (func -> ret_type != VOID) {
      // make a unit for the return value
      data_item dummy = { .type = func->ret_type };
      const_item const_dummy = add_const(dummy);
      $<addr>2 = generate_instruction(PUSH, const_dummy); // reuse $2 to keep the starting addr
    }
    $<func>$ = *func;
    dprint("arg of %s\n", $<func>$.name);
  }
  arg_list RPASYM
  {
    func_item func = $<func>4;
    $$.type = func.ret_type;
    $$.addr = $<addr>2;
    generate_instruction(INIT, func);
    dprint("call function(arglist)\n");
  }
;
arg_list:
  %empty
  {
    $$ = 1;
  }
  |
  expression
  {
    first_param = $<func>0.params;
    if (!check_type($1.type, first_param->type)) {
      // TODO type error
    }
    dprint("expression(arg)\n");
  }
  |
  arg_list COMMASYM expression
  {
    if ($1) {
      // indicating %empty
      // TODO syntax error
      first_param = $<func>0.params;
    }
    first_param = first_param -> next;
    if (!check_type($3.type, first_param->type)) {
      // TODO
    }
    dprint("arg..., expression\n");
  }
;

expression:
  bool_expr
  { $$ = $1; }
  |
  alg_expr
  { $$ = $1; }
;
bool_expr:
  bool_term
  { $$ = $1; }
  |
  bool_expr ORSYM bool_term
  {
    $$ = $1;
    generate_instruction(ALGO, OR);
    dprint("or\n");
  }
;
bool_term:
  bool_term ANDSYM bool_factor
  {
    $$ = $1;
    generate_instruction(ALGO, AND);
    dprint("and\n");
  }
  |
  bool_factor
  { $$ = $1; }
;
bool_factor:
  alg_expr RELSYM alg_expr
  {
    $$ = $1;
    generate_instruction(ALGO, (algo_enum) $2.value.li);
    dprint("expression REL expression\n");
  }
  |
  TRUESYM
  {
    data_item item = { .type = BOOL, .value.bo = 1 };
    const_item citem = add_const(item);
    $$.addr = generate_instruction(PUSH, citem);
    $$.type = BOOL;    
    dprint("true\n");
  }
  |
  FALSESYM
  {
    data_item item = { .type = BOOL, .value.bo = 0 };
    const_item citem = add_const(item);
    $$.addr = generate_instruction(PUSH, citem);
    $$.type = BOOL;
    dprint("false\n");
  }
  |
  BIDENTSYM
  {
    if (VOID == $1.type) {
      // TODO undefined
    }
    $$.addr = generate_instruction(LOAD, $1.locator);
    $$.type = BOOL;
    dprint("bool ident\n");
    free($1.name);
  }
  |
  NOTSYM bool_factor
  {
    $$ = $2;
    generate_instruction(ALGO, NOT);
    dprint("not bfactor\n");
  }
  |
  LPASYM bool_expr RPASYM
  {
    $$ = $2;
    dprint("( bexp )\n");
  }
;
alg_expr:
  alg_term
  { $$ = $1; }
  |
  ADDSYM alg_term
  {
    $$ = $2;
  }
  |
  SUBSYM alg_term
  {
    $$ = $2;
    generate_instruction(ALGO, OPP);
  }
  |
  alg_expr ADDSYM alg_term
  {
    $$ = $1;
    generate_instruction(ALGO, ADD);
    dprint("alg_expr + alg_term\n");
  }
  |
  alg_expr SUBSYM alg_term
  {
    $$ = $1;
    generate_instruction(ALGO, SUB);
    dprint("alg_expr - alg_term\n");
  }
;
alg_term:
  alg_factor
  {
    $$ = $1;
    dprint("alg_factor\n");
  }
  |
  alg_term MULSYM alg_factor
  {
    $$ = $1;
    generate_instruction(ALGO, MUL);
    dprint("alg_term * alg_factor\n");
  }
  |
  alg_term DIVSYM alg_factor
  {
    $$ = $1;
    generate_instruction(ALGO, DIV);
    dprint("alg_term / alg_factor\n");
  }
  |
  alg_term MODSYM alg_factor
  {
    $$ = $1;
    generate_instruction(ALGO, MOD);
    dprint("alg_term %% alg_factor\n");
  }
;
alg_factor:
  AIDENTSYM
  {
    if (VOID == $1.type) {
      // TODO undefined
    }
    $$.addr = generate_instruction(LOAD, $1.locator);
    $$.type = $1.type;
    dprint("alg ident scope:%lu var_no:%lu\n", $1.locator.scope, $1.locator.var_no);
    free($1.name);
  }
  |
  call_stat
  {
    $$ = $1;
    dprint("call_stat\n");
  }
  |
  INUMSYM
  {
    const_item citem = add_const($1);
    $$.addr = generate_instruction(PUSH, citem);
    $$.type = INT;
    dprint("int: %lu\n", $1.value.li);
  }
  |
  FNUMSYM
  {
    const_item citem = add_const($1);
    $$.addr = generate_instruction(PUSH, citem);
    $$.type = FLOAT;
    dprint("float: %lf\n", $1.value.fl);
  }
  |
  LPASYM alg_expr RPASYM
  {
    $$ = $2;
    dprint("( alg_expr )\n");
  }
%%

char check_type(type_enum t1, type_enum t2) {
  if (t1 == t2) {
    return 1;
  } else if ((t1 == FLOAT || t1 == INT) && (t2 == FLOAT || t2 == INT)) {
    return 1;
  } else {
    // TODO type error
    dprint("type error!\n");    
    return 0;
  }
}

void yyinit() {
  init_table();
  init_compiler();
}

void yyclose() {
  close_compiler();
  close_table();
}
