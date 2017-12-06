#ifndef TYPES_H
#define DEBUG

// define dprint = debug print
#ifdef DEBUG
  #include <stdio.h>
  #define dprint printf
#else
  #define dprint(...)
#endif

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

typedef struct {
  unsigned long addr;
  type_enum type;
} expr_item;

#define TYPES_H
#endif
