#include <malloc.h>
#include "machine.h"

data_item *data_stack;
unsigned long stack_top = 0;
unsigned long base = 0;

void init_machine() {
  data_stack = (data_item*) malloc(DATA_STACK_SIZE * sizeof(data_item));
}

void interpreter(instruction ins) {
  
}

void close_machine() {
  free(data_stack);
}

/*
data_item *search_stack(var_locator locator) {
  return NULL;
}

void push_literal(data_item data) {
  dprint("push literal\n");
  data_stack[stack_top] = data;
  stack_top++;
}

void push_var(var_locator var) {
  dprint("push var\n");
}

data_item pop() {
  return data_stack[--stack_top];
}

void store(var_locator var) {
  dprint("storing var\n");
}
*/
