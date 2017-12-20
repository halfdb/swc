%{
#include <stdio.h>
#include <string.h>
#include "config.h"
#include "machine.h"
#include "table.h"
#include "parser.h"
extern void yyerror(char const*);

%}

%option noyywrap
%option yylineno

LETTER  [a-zA-Z]
DIGIT   [0-9]
NUMBER  {DIGIT}+
IDENT   ({LETTER}|_)({LETTER}|{DIGIT}|_)*

%%
[\t\n ]             { }
"/*"(.|\n)*"*/"     { }
"//".*$             { }
\"[^\"]*\"          {
                      char *text = strdup(yytext + 1);
                      text[strlen(text) - 1] = '\0';
                      yylval.data.value.str = text;
                      yylval.data.type = STR;
                      return STRSYM;
                    }
;|；                { return SMCLSYM; }
[=!！]=             {
                      if (yytext[0] == '=') {
                        yylval.data.value.li = EQ;
                      } else {
                        yylval.data.value.li = NEQ;
                      }
                      return RELSYM;
                    }
[<《]=              {
                      yylval.data.value.li = LE;
                      return RELSYM;
                    }
[>》]=              {
                      yylval.data.value.li = GE;
                      return RELSYM;
                    }
[<《]               {
                      yylval.data.value.li = LT;
                      return RELSYM;
                    }
[>》]               {
                      yylval.data.value.li = GT;
                      return RELSYM;
                    }
"read"              { return READSYM; }
"print"             { return PRINTSYM; }
"call"              { return CALLSYM; }
"return"            { return RETSYM; }
"true"              { return TRUESYM; }
"false"             { return FALSESYM; }
"&&"                { return ANDSYM; }
"||"                { return ORSYM; }
!|！                { return NOTSYM; }
\(|（               { return LPASYM; }
\)|）               { return RPASYM; }
\{                  { return LBRSYM; }
\}                  { return RBRSYM; }
,|，                { return COMMASYM; }
"+"                 { return ADDSYM; }
"-"                 { return SUBSYM; }
"*"                 { return MULSYM; }
"/"                 { return DIVSYM; }
"%"                 { return MODSYM; }
"="                 { return ASSNSYM; }
"if"                { return IFSYM; }
"else"              { return ELSESYM; }
"while"             { return WHILESYM; }
"for"               { return FORSYM; }
"in"                { return INSYM; }
"..."|".."          { return ELLIPSYM; }
"void"              { yylval.var.type = VOID; return VOIDSYM; }
"int"               { yylval.var.type = INT; return INTSYM; }
"bool"              {
                      yylval.var.type = BOOL;
                      return BOOLSYM;
                    }
"float"             { yylval.var.type = FLOAT; return FLOATSYM; }
{IDENT}             {
                      var_node *value = &yylval.var;
                      value->name = strdup(yytext);
                      func_item *pfunc = find_func(value->name);
                      if (pfunc != NULL) {
                        value->type = VOID; // actually function
                        return AIDENTSYM;
                      }
                      // indicating not a function
                      var_node *pvar = find_var(value->name);
                      if (pvar == NULL) {
                        // not defined
                        value->type = VOID;
                        return AIDENTSYM;
                      } else {
                        char *t = value -> name;
                        memcpy(value, pvar, sizeof(var_node));
                        value -> name = t;
                        switch (pvar->type) {
                        case BOOL:
                          return BIDENTSYM;
                        default:
                          return AIDENTSYM;
                        }
                      }
                    }
{NUMBER}\.{NUMBER}  { yylval.data.value.fl = atof(yytext); yylval.data.type = FLOAT; return FNUMSYM; }
{NUMBER}            { yylval.data.value.li = atoi(yytext); yylval.data.type = INT; return INUMSYM; }
.                   { yyerror("Unknow character."); return 0; }

%%

