#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "table.h"
#include "interface.h"

unsigned long current_scope = 0;

//var_list var_table;
//int var_count = 0;
func_item *func_table;
unsigned long func_count = 1;

unsigned long const_size = 0;

void init_table() {
  func_table = (func_item*) malloc(FUNC_TABLE_SIZE * sizeof(func_item));
  func_table->name = "(main)";
  func_table->params = NULL;
  func_table->param_num = 0;
  func_table->vars = NULL;
  func_table->var_num = 0;

  const_table = (void*) malloc(CONST_TABLE_SIZE);
}

void close_table() {
  // TODO
}

void add_var(var_node var) {
  dprint("add to var table\n");

  func_item *func = func_table + current_scope;
  var_node *pvar = (var_node*) malloc(sizeof(var_node));
  memcpy(pvar, &var, sizeof(var_node));
  pvar -> name = strdup(pvar -> name);

  // TODO set var_no to include params.
  var_locator locator = {
    .scope = current_scope,
    .var_no = func->param_num + func->var_num++
  };
  pvar->locator = locator;
  pvar->next = func->vars;
  func->vars = pvar;

  dprint("name: %s, type: %u, scope: %lu, var_no: %lu",
      pvar->name, pvar->type, pvar->locator.scope, pvar->locator.var_no);
}

unsigned long add_func(func_item func) {
  dprint("add to func table\n");

  if (func_count >= FUNC_TABLE_SIZE) {
    yyerror("Too many functions.\n");
  }

  func.name = strdup(func.name);
  func_table[func_count] = func;
  dprint("added, name=%s, param count=%lu, return type=%d", func.name, func.param_num, func.ret_type);
  return func_count++;
}

var_node *find_var(char *name) {
  func_item *scopes[2] = {func_table + current_scope, func_table};
  int i = 0;
  for (; i < 2; i++) {
    func_item *func = scopes[i];
    var_node* var = func->vars;
    while (var != NULL) {
      if (strcmp(var->name, name) == 0) {
        dprint("%s found scope:%lu, vno:%lu\n", var->name, var->locator.scope, var->locator.var_no);
        break;
      }
      var = var->next;
    }
    if (var != NULL) {
      return var;
    }
    // not found in vars, try with params
    param_node *param = func -> params;
    while (param != NULL) {
      if (strcmp(param->name, name) == 0) {
        break;
      }
      param = param -> next;
    }
    if (param != NULL) {
      return param;
    }
  }

  dprint("var(name=%s) not found\n", name);
  return NULL;
}

func_item *find_func(char *name) {
  func_item *func;
  int i = 0;
  for (func = func_table; i < func_count; func = func_table + ++i) {
    if (strcmp(func->name, name) == 0) {
      return func;
    }
  }
  dprint("func(name=%s) not found\n", name);
  return NULL;
}

const_item add_const(data_item item) {
  const_item ret = { .type = item.type };
  unsigned long size;
  void *addr = const_table + const_size;
  switch (item.type) {
    case INT:
    size = 0;
    memcpy(ret.value, &item.value.li, INT_SIZE);
    break;

    case FLOAT:
    size = 0;
    memcpy(ret.value, &item.value.fl, FLOAT_SIZE);
    break;

    case BOOL:
    size = 0;
    ret.value[0] = item.value.bo ? 1 : 0;
    break;

    case STR:
    size = strlen(item.value.str) + 1;
    if (const_size + size > CONST_TABLE_SIZE) {
      yyerror("Too many strings.\n");
      return ret;
    }
    memcpy(ret.value, &const_size, sizeof(INT_SIZE));
    const_size += size;
    memcpy(addr, item.value.str, size);
    break;

    default:
    size = 0;
    dprint("adding illegal const!\n");
  }
  return ret;
}

unsigned long log_func(func_item func) {
  int ret = const_size;
  int size = sizeof(type_enum);
  var_list vars;
  for (vars=func.vars; vars!=NULL; vars=vars->next) {
    memcpy(const_table + const_size, &vars->type, size);
    const_size += size;
  }
  if (const_size - ret != func.var_num * size) {
    yyerror("Table error.\n");
    exit(1);
  }
  return ret;
}
