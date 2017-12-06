#ifndef COMPILER_H
#include <stdarg.h>
//#include <stdio.h>
#include "types.h"
#include "machine.h"
#include "table.h"

void init_compiler();
void close_compiler();

// instruction stack
#define INS_STACK_SIZE (1<<24)
instruction *ins_stack;
unsigned long ins_top;

// void move_ins_top(int addr);

#define ACT_LOG_SIZE 3
/**
 * --low addr end--
 * (return unit)
 * (parameter units)
 * dynamic link (previous base)  <-- new base
 * return address in the instruction stack
 * parameter count
 * --high addr end--
 */

/**
 * variable arguments:
 * LOAD, STORE, READ:
 * ..., var_locator locator
 * PRINT:
 * ..., unsigned long count
 * PUSH:
 * ..., const_item item
 * ALGO:
 * ..., algo_enum algorithm
 * JUMP, JNZ, JZ:
 * ..., unsigned long addr
 * INIT:
 * ..., func_item func
 * RET, POP, ERR:
 * ... (NONE)
 */
unsigned long generate_instruction(action_type action, ...);
void change_instruction(unsigned long addr, action_type action, ...);

void output(const char *filename);
#define CONST_SIZE_BYTES 4
#define ACTION_BYTES 1
#define PARAM1_BYTES 4
#define PARAM2_BYTES 4
#define INSTRUCTION_BYTES (ACTION_BYTES + PARAM1_BYTES + PARAM2_BYTES)

void output_readable(const char *filename);


#define COMPILER_H
#endif
