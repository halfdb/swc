#ifndef CONFIG_H
//#define DEBUG

#ifndef NULL
#define NULL ((void*)0)
#endif

// define dprint = debug print
#ifdef DEBUG
  #include <stdio.h>
  #define dprint printf
#else
  #define dprint(...)
#endif

#define CONFIG_H
#endif
