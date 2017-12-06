#ifndef MACHINE_H
#include "types.h"

/* data stack */


#define DATA_STACK_SIZE ((1<<24)-1)

typedef enum {
  INIT,
  RET,
  JUMP,
  JNZ, // jump if true
  JZ, // jump if false
  LOAD,
  STORE,
  PUSH,
  POP,
  ALGO,
  READ,
  PRINT,
  ERR // runtime error
} action_type;

typedef struct {
  action_type action;
  unsigned long param1;
  unsigned long param2;
} instruction;

typedef enum {
  ADD, SUB, OPP,
  MUL, DIV, MOD,
  /*
  ADDF, SUBF, OPPF,
  MULF, DIVF,
  ITOF, FTOI,
  */
  NOT, AND, OR,
  GT, LT, GE, LE,
  EQ, NEQ
} algo_enum;

void init_machine();

#define MACHINE_H
#endif
