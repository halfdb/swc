#ifndef TABLE_H
#include "types.h"
/* global variables */
unsigned long current_scope;

typedef struct {
  unsigned long scope;
  unsigned long var_no;
} var_locator;

typedef struct __node {
  char *name;
  type_enum type;
  var_locator locator;
  struct __node *next;
} param_node, var_node, *param_list, *var_list;

typedef struct {
  char* name;
  type_enum ret_type;
  unsigned long param_num;
  param_list params;
  unsigned long var_num;
  var_list vars;
  unsigned long addr;
} func_item;

void init_table();
void close_table();

/* tables */
/* variable table */
//#define VAR_TABLE_SIZE ((1<<16)-1)

/* function table */
#define FUNC_TABLE_SIZE ((1<<24)-1) // by items
func_item *func_table;

/* functions */
// add a funcion to the table
unsigned long add_func(func_item);
// add a variable to the current scope
void add_var(var_node);

// find a var/function by its name in the current scope
var_node *find_var(char* name);
func_item *find_func(char *name);

/* static table */
typedef struct {
  type_enum type;
  char value[MAX_SIZE];
} const_item;

#define CONST_TABLE_SIZE ((1<<24)-1) // by bytes
void *const_table;
unsigned long const_size;

// add a new const and return its const_item
const_item add_const(data_item item);

#define TABLE_H
#endif
