#ifndef CONFIG_H
#define DEBUG

// define dprint = debug print
#ifdef DEBUG
  #include <stdio.h>
  #define dprint printf
#else
  #define dprint(...)
#endif

#define CONFIG_H
#endif
