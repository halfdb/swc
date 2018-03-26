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

#define INT_SIZE sizeof(long)
#define FLOAT_SIZE sizeof(float)
#define BOOL_SIZE 1
#define MAX_TYPE_SIZE (INT_SIZE > FLOAT_SIZE ? INT_SIZE : FLOAT_SIZE)

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
  NOT, AND, OR,
  GT, LT, GE, LE,
  EQ, NEQ
} algo_enum;

#define ACT_LOG_SIZE 4
/**
 * --low addr end--
 * (return unit with chosen type)
 * (parameter units)
 * dynamic link (previous base)  <-- new base
 * return address in the instruction stack
 * parameter count
 * var type list addr
 * --high addr end--
 */

void init_machine();
void close_machine();
int start_program(instruction *instructions, void *static_area);
void interpret(instruction ins, void *static_area);

#define MACHINE_H
#endif
