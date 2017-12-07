#ifndef MACHINE_H
#include "config.h"

/* data stack */
#define DATA_STACK_SIZE ((1<<24)-1)

/* types */
typedef enum {
  VOID,
  INT,
  FLOAT,
  BOOL,
  STR
} type_enum;

#define INT_SIZE 4
#define FLOAT_SIZE 4
#define BOOL_SIZE 1
#define MAX_SIZE 4

typedef struct {
  type_enum type;
  union {
    long int li;
    float fl;
    char bo;
    char* str;
    unsigned long addr;
  } value;
} data_item;

typedef enum {
  INIT,
  CALL,
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

#define ACT_LOG_SIZE 3
/**
 * --low addr end--
 * (return unit with chosen type)
 * (parameter units)
 * dynamic link (previous base)  <-- new base
 * return address in the instruction stack
 * parameter count
 * --high addr end--
 */

void init_machine();

#define MACHINE_H
#endif
