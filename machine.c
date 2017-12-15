#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include "machine.h"

data_item *data_stack;
unsigned long stack_top = 0;
unsigned long base = 0;
unsigned long instruction_pointer = 0;

typedef enum {
  UNINITIALIZED,
  INITIALIZED,
  RUNNING,
  FINISHED,
  ERROR
} status_enum;
status_enum status = UNINITIALIZED;

static void _assign(data_item *dest_item, const data_item *item);
static data_item *_locate(unsigned long scope, unsigned long var_no);

void init_machine() {
  if (data_stack != NULL) {
    free(data_stack);
  }
  data_stack = (data_item*) malloc(DATA_STACK_SIZE * sizeof(data_item));
  status = INITIALIZED;
}

void start_program(instruction* instructions, void *static_area) {
  if (status != INITIALIZED) {
    dprint("not intialized!!\n");
  }
  memset(data_stack, 0, 3 * sizeof(data_item));
  stack_top = 3;
  base = 0;
  instruction_pointer = 0;

  status = RUNNING;
  while (status == RUNNING) {
    dprint("instruction %ld\n", instruction_pointer);
    interpret(instructions[instruction_pointer], static_area);
  }
  
  switch (status) {
    case FINISHED:
    break;
    case ERROR:
    default:
    runtime_error("unknown");
    break;
  }
  dprint("program finished\n");
}

void interpret(instruction ins, void *static_area) {
  switch (ins.action) {
    case INIT:
    {
      stack_top = base + 3;
      unsigned long var_num = ins.param1;
      if (var_num != 0) {
        data_stack[stack_top].type = VOID;
        data_stack[stack_top].value.addr = var_num;

        int i;
        type_enum *types = static_area + ins.param2;
        for (i=0; i<var_num; i++) {
          data_stack[stack_top + var_num - i].type = types[i];
        }
      }
      stack_top += var_num + 1;
      break;
    }
    case CALL:
    {
      unsigned long top = stack_top;

      data_stack[top].type = VOID;
      data_stack[top].value.addr = base;
      base = top;
      top++;

      data_stack[top].type = VOID;
      data_stack[top].value.addr = instruction_pointer + 1;
      instruction_pointer = ins.param1;
      top++;

      data_stack[top].type = VOID;
      data_stack[top].value.addr = ins.param2;
      top++;

      stack_top = top;
      return;
    }
    case RET:
    {
      if (base == 0) {
        status = FINISHED;
        return;
      }
      unsigned long top = base - data_stack[base + 2].value.li;
      if (ins.param1 == 1) {
        _assign(data_stack + top - 1, data_stack + stack_top - 1);
      } else if (ins.param1 != 0) {
        runtime_error("illegal instruction");
      }

      stack_top = top;
      instruction_pointer = data_stack[base + 1].value.addr;
      base = data_stack[base].value.addr;
      return;
    }
    case JZ:
    if (data_stack[stack_top - 1].type == BOOL) {
      if (data_stack[stack_top - 1].value.bo == 1) {
        break;
      }
    } else {
      runtime_error("the top unit's type should be bool for JNZ");
    }
    instruction_pointer = ins.param1;
    return;
    case JNZ:
    if (data_stack[stack_top - 1].type == BOOL) {
      if (data_stack[stack_top - 1].value.bo != 1) {
        break;
      }
    } else {
      runtime_error("the top unit's type should be bool for JNZ");
    }
    instruction_pointer = ins.param1;
    return;
    case JUMP:
    instruction_pointer = ins.param1;
    return;
    case LOAD:
    data_stack[stack_top++] = *_locate(ins.param1, ins.param2);
    break;
    case STORE:
    stack_top--;
    _assign(_locate(ins.param1, ins.param2), data_stack + stack_top);
    break;
    case PUSH:
    memcpy(&((data_stack + stack_top)->value), &ins.param2, MAX_TYPE_SIZE);
    data_stack[stack_top].type = ins.param1;
    stack_top++;
    break;
    case POP:
    stack_top--;
    break;
    case READ:
    {
      data_item *dest = _locate(ins.param1, ins.param2);
      if (dest -> type != BOOL) {
        char *format;
        switch (dest -> type) {
          case INT:
          format = "%d";
          break;
          case FLOAT:
          format = "%f";
          break;
          default:
          runtime_error("reading unknown read");
        }
        if (scanf(format, &(dest->value)) != 1) {
          dprint("warning! variable not read\n");
        }
      } else {
        char c = getchar();
        while (c == ' ' || c == '\t' || c == '\n'); // empty body
        switch (c) {
          case 't':
          if (getchar() == 'r') {
            if (getchar() == 'u') {
              if (getchar() == 'e') {
                dest -> value.bo = 1;
                break;
              }
            }
          }
          dprint("warning! variable not read\n");
          break;
          case 'f':
          if (getchar() == 'a') {
            if (getchar() == 'l') {
              if (getchar() == 's') {
                if (getchar() == 'e') {
                  dest -> value.bo = 0;
                  break;
                }
              }
            }
          }
          dprint("warning! variable not read\n");
          break;
          default:
          dprint("warning! variable not read\n");
          break;
        }
      }
    }
    break;
    case PRINT:
    {
      unsigned long count = ins.param1;
      unsigned long i;
      for (i=0; i<count; i++) {
        data_item *item = data_stack + stack_top - count + i;
        if (item -> type == BOOL) {
          if (item -> value.bo) {
            printf("true");
          } else {
            printf("false");
          }
        } else if (item -> type == STR) {
          char *str = (char*) static_area + item -> value.addr;
          printf("%s", str);
        } else {
          switch (item -> type) {
            case INT:
            printf("%ld", item->value.li);
            break;
            case FLOAT:
            printf("%f", item->value.fl);
            break;
            default:
            runtime_error("printing unknown type");
          }
        }
      }
      printf("\n");
      stack_top -= count;
      break;
    }
    case ALGO:
    {
      data_item *i1 = data_stack + stack_top - 2;
      data_item *i2 = data_stack + stack_top - 1;
      switch (ins.param1) {
        case ADD:
        if (i1->type == i2->type && i1->type == INT) {
          i1->value.li += i2->value.li;
        } else if (i1->type == i2->type && i1->type == FLOAT) {
          i1->value.fl += i2->value.fl;
        } else if (i1->type == FLOAT && i2->type == INT) {
          i1->value.fl += i2->value.li;
        } else if (i1->type == INT && i2->type == FLOAT) {
          i1->type = FLOAT;
          i1->value.fl = i1->value.li + i2->value.fl;
        } else {
          runtime_error("unexpected type");
        }
        break;
        case SUB:
        if (i1->type == i2->type && i1->type == INT) {
          i1->value.li -= i2->value.li;
        } else if (i1->type == i2->type && i1->type == FLOAT) {
          i1->value.fl -= i2->value.fl;
        } else if (i1->type == FLOAT && i2->type == INT) {
          i1->value.fl -= i2->value.li;
        } else if (i1->type == INT && i2->type == FLOAT) {
          i1->type = FLOAT;
          i1->value.fl = i1->value.li - i2->value.fl;
        } else {
          runtime_error("unexpected type");
        }
        break;
        case MUL:
        if (i1->type == i2->type && i1->type == INT) {
          i1->value.li *= i2->value.li;
        } else if (i1->type == i2->type && i1->type == FLOAT) {
          i1->value.fl *= i2->value.fl;
        } else if (i1->type == FLOAT && i2->type == INT) {
          i1->value.fl *= i2->value.li;
        } else if (i1->type == INT && i2->type == FLOAT) {
          i1->type = FLOAT;
          i1->value.fl = i1->value.li * i2->value.fl;
        } else {
          runtime_error("unexpected type");
        }
        break;
        case DIV:
        if (i2->value.li == 0) {
          runtime_error("divided by 0");
        }
        if (i1->type == i2->type && i1->type == INT) {
          i1->value.li /= i2->value.li;
        } else if (i1->type == i2->type && i1->type == FLOAT) {
          i1->value.fl /= i2->value.fl;
        } else if (i1->type == FLOAT && i2->type == INT) {
          i1->value.fl /= i2->value.li;
        } else if (i1->type == INT && i2->type == FLOAT) {
          i1->type = FLOAT;
          i1->value.fl = i1->value.li / i2->value.fl;
        } else {
          runtime_error("unexpected type");
        }
        break;
        case MOD:
        if (i1->type != INT || i2->type != INT) {
          runtime_error("mod involving incompatible type");
        }
        i1->value.li = i1->value.li % i2->value.li;
        break;
        case OPP:
        if (i2->type == FLOAT) {
          i2->value.fl = -i2->value.fl;
        } else if (i2->type == INT) {
          i2->value.li = -i2->value.li;
        } else {
          runtime_error("opp on incompatible type");
        }
        stack_top++;
        break;

        case AND:
        if (i1->type == BOOL && i2->type == BOOL) {
          i1->value.bo = i1->value.bo && i2->value.bo;
        } else {
          runtime_error("and involving incompatible type");
        }
        break;
        case OR:
        if (i1->type == BOOL && i2->type == BOOL) {
          i1->value.bo = i1->value.bo || i2->value.bo;
        } else {
          runtime_error("or involving incompatible type");
        }
        break;
        case NOT:
        if (i2->type == BOOL) {
          i2->value.bo = !i2->value.bo;
        } else {
          runtime_error("not on incompatible type");
        }
        stack_top++;
        break;
        case GT:
        if (i1->type == i2->type && i1->type == INT) {
          i1->value.bo = i1->value.li > i2->value.li;
        } else if (i1->type == i2->type && i1->type == FLOAT) {
          i1->value.bo = i1->value.fl > i2->value.fl;
        } else if (i1->type == FLOAT && i2->type == INT) {
          i1->value.bo = i1->value.fl > i2->value.li;
        } else if (i1->type == INT && i2->type == FLOAT) {
          i1->value.bo = i1->value.li > i2->value.fl;
        } else {
          runtime_error("unexpected type");
        }
        i1->type = BOOL;
        break;
        case LT:
        if (i1->type == i2->type && i1->type == INT) {
          i1->value.bo = i1->value.li < i2->value.li;
        } else if (i1->type == i2->type && i1->type == FLOAT) {
          i1->value.bo = i1->value.fl < i2->value.fl;
        } else if (i1->type == FLOAT && i2->type == INT) {
          i1->value.bo = i1->value.fl < i2->value.li;
        } else if (i1->type == INT && i2->type == FLOAT) {
          i1->value.bo = i1->value.li < i2->value.fl;
        } else {
          runtime_error("unexpected type");
        }
        i1->type = BOOL;
        break;
        case GE:
        if (i1->type == i2->type && i1->type == INT) {
          i1->value.bo = i1->value.li >= i2->value.li;
        } else if (i1->type == i2->type && i1->type == FLOAT) {
          i1->value.bo = i1->value.fl >= i2->value.fl;
        } else if (i1->type == FLOAT && i2->type == INT) {
          i1->value.bo = i1->value.fl >= i2->value.li;
        } else if (i1->type == INT && i2->type == FLOAT) {
          i1->value.bo = i1->value.li >= i2->value.fl;
        } else {
          runtime_error("unexpected type");
        }
        i1->type = BOOL;
        break;
        case LE:
        if (i1->type == i2->type && i1->type == INT) {
          i1->value.bo = i1->value.li <= i2->value.li;
        } else if (i1->type == i2->type && i1->type == FLOAT) {
          i1->value.bo = i1->value.fl <= i2->value.fl;
        } else if (i1->type == FLOAT && i2->type == INT) {
          i1->value.bo = i1->value.fl <= i2->value.li;
        } else if (i1->type == INT && i2->type == FLOAT) {
          i1->value.bo = i1->value.li <= i2->value.fl;
        } else {
          runtime_error("unexpected type");
        }
        i1->type = BOOL;
        break;
        case EQ:
        if (i1->type == i2->type && i1->type == INT) {
          i1->value.bo = i1->value.li == i2->value.li;
        } else if (i1->type == i2->type && i1->type == FLOAT) {
          i1->value.bo = i1->value.fl == i2->value.fl;
        } else if (i1->type == FLOAT && i2->type == INT) {
          i1->value.bo = i1->value.fl == i2->value.li;
        } else if (i1->type == INT && i2->type == FLOAT) {
          i1->value.bo = i1->value.li == i2->value.fl;
        } else {
          runtime_error("unexpected type");
        }
        i1->type = BOOL;
        break;
        case NEQ:
        if (i1->type == i2->type && i1->type == INT) {
          i1->value.bo = i1->value.li != i2->value.li;
        } else if (i1->type == i2->type && i1->type == FLOAT) {
          i1->value.bo = i1->value.fl != i2->value.fl;
        } else if (i1->type == FLOAT && i2->type == INT) {
          i1->value.bo = i1->value.fl != i2->value.li;
        } else if (i1->type == INT && i2->type == FLOAT) {
          i1->value.bo = i1->value.li != i2->value.fl;
        } else {
          runtime_error("unexpected type");
        }
        i1->type = BOOL;
        break;
      }
      stack_top--;
      break;
    }
    case ERR:
    default:
    runtime_error("unexpected instruction");
  }
  instruction_pointer++;
}

static void _assign(data_item *dest_item, const data_item *item) {
  if (dest_item -> type == item -> type) {
    *dest_item = *item;
  } else if (dest_item -> type == FLOAT && item -> type == INT) {
    dest_item -> value.fl = (float) item -> value.li;
  } else if (dest_item -> type == INT && item -> type == FLOAT) {
    dest_item -> value.li = (long) item -> value.fl;
  } else {
    runtime_error("assigning incompatible data type");
  }
}

static data_item *_locate(unsigned long scope, unsigned long var_no) {
  unsigned long scope_base = scope ? base : 0;
  unsigned long param_count = data_stack[scope_base + 2].value.addr;
  unsigned long var_count = data_stack[scope_base + 3].value.addr;

  if (var_no < param_count) {
    return data_stack + scope_base - param_count + var_no;
  } else if ((var_no -= param_count) < var_count) {
    return data_stack + scope_base + ACT_LOG_SIZE + var_no;
  } else {
    runtime_error("locating unknown parameter or variable");
  }
}

void runtime_error(char* error) {
  status = ERROR;
  fprintf(stderr, "runtime error: %s\n", error);
  fprintf(stderr, "instruction address: %lu\n", instruction_pointer);
  exit(1);
}

void close_machine() {
  free(data_stack);
}
