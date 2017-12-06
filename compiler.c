#include <malloc.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "machine.h"
#include "compiler.h"
#include "interface.h"

unsigned long ins_top = 0;
static void _generate(action_type action, va_list argp);

void init_compiler() {
  ins_stack = (instruction*) malloc(INS_STACK_SIZE * sizeof(instruction));
  memset(ins_stack, 0, INS_STACK_SIZE * sizeof(instruction));
}

/*void move_ins_top(int addr) {
  ins_top = addr;
}*/

void change_instruction(unsigned long addr, action_type action, ...) {
  unsigned long t = ins_top;
  ins_top = addr;
  va_list argp;
  va_start(argp, action);
  _generate(action, argp);
  va_end(argp);
  ins_top = t;
}

unsigned long generate_instruction(action_type action, ...) {
  va_list argp;
  va_start(argp, action);
  _generate(action, argp);
  va_end(argp);
  return ins_top++;
}

void _generate(action_type action, va_list argp) {
  instruction *ins = ins_stack + ins_top;
  ins -> action = action;
  switch (action) {
    case LOAD:
    case STORE:
    case READ:
    {
      var_locator locator = va_arg(argp, var_locator);
      ins -> param1 = locator.scope;
      ins -> param2 = locator.var_no;
      break;
    }
    case PRINT:
    case RET:
    {
      ins -> param1 = va_arg(argp, unsigned long);
      break;
    }    
    case PUSH:
    {
      const_item item = va_arg(argp, const_item);
      ins -> param1 = item.type;
      memcpy(&ins->param2, item.value, MAX_SIZE);
      break;
    }
    case ALGO:
    {
      ins->param1 = (unsigned long) va_arg(argp, algo_enum);
      break;
    }
    case JUMP:
    case JNZ:
    case JZ:
    {
      ins -> param1 = va_arg(argp, unsigned long);
      break;
    }
    case INIT:
    {
      func_item func = va_arg(argp, func_item);
      ins -> param1 = func.param_num;
      break;
    }
    case POP:
    case ERR:
    {
      break;
    }
    default:
    {
      dprint("Unknown Instruction!!\n");
      break;
    }
  }
}

void output(const char *filename) {
  int fd = open(filename, O_WRONLY | O_TRUNC | O_CREAT);
  write(fd, &const_size, CONST_SIZE_BYTES);
  write(fd, const_table, const_size);
  int i;
  void *ins_buf = malloc(INSTRUCTION_BYTES);
  for (i=0; i<ins_top; i++) {
    // asserting ACTION_BYTES == 1
    instruction *ins = ins_stack + i;
    char action = ins->action & 0xFF;
    memcpy(ins_buf, &action, ACTION_BYTES);
    memcpy(ins_buf+ACTION_BYTES, &ins->param1, PARAM1_BYTES);
    memcpy(ins_buf+ACTION_BYTES+PARAM1_BYTES, &ins->param2, PARAM2_BYTES);
    write(fd, ins_buf, INSTRUCTION_BYTES);
  }
  free(ins_buf);
  close(fd);
}

void output_readable(const char *filename) {
  FILE *file = fopen(filename, "w");
  int i;
  for (i=0; i<ins_top; i++) {
    instruction *ins = ins_stack + i;
    fprintf(file, "%-3d: ", i);
    switch (ins->action) {
      case LOAD:
      fprintf(file, "load scope:%lu var_no:%lu\n", ins->param1, ins->param2);
      break;
      case STORE:
      fprintf(file, "store scope:%lu var_no:%lu\n", ins->param1, ins->param2);
      break;
      case READ:
      fprintf(file, "read scope:%lu var_no:%lu\n", ins->param1, ins->param2);
      break;
      case PRINT:
      fprintf(file, "print count:%lu\n", ins->param1);
      break;
      case PUSH:
      fprintf(file, "push ");
      switch (ins->param1) {
        case INT:
        fprintf(file, "int %lu\n", ins->param2);
        break;
        case FLOAT:
        fprintf(file, "float %f\n", *(float*)&ins->param2);
        break;
        case BOOL:
        fprintf(file, "bool %lu\n", ins->param2);
        break;
        case STR:
        fprintf(file, "str %lu ", ins->param2);
        fprintf(file, "%s\n", (char*)const_table + ins->param2);
        break;
        default:
        dprint("unknown type while outputing\n");
        yyerror("Unknown error.\n");
        exit(1);
      }
      break;
      case ALGO:
      fprintf(file, "algo %lu\n", ins->param1);
      break;
      case JUMP:
      fprintf(file, "jump %lu\n", ins->param1);
      break;
      case JNZ:
      fprintf(file, "jnz %lu\n", ins->param1);
      break;
      case JZ:
      fprintf(file, "jz %lu\n", ins->param1);
      break;
      case INIT:
      fprintf(file, "init param count:%lu\n", ins->param1);
      break;
      case RET:
      fprintf(file, "ret\n");
      break;
      case POP:
      fprintf(file, "pop\n");
      break;
      case ERR:
      fprintf(file, "err\n");
      break;
      default:
      dprint("unknown instruction while outputing\n");
      yyerror("Unknown error.\n");
      exit(1);
    }
  }
  fclose(file);
}

void close_compiler() {
  free(ins_stack);
}
