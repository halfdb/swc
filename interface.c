#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include "compiler.h"
#include "machine.h"

extern void yyinit();
extern void yyclose();
extern int yyparse();

void yyerror(char const * str) {
  fprintf(stderr, "%s", str);
}

void usage(char *argv0) {
  fprintf(stderr, "Usage: %s [-i] [-c [out_file]] [-r [out_file]] file\n", argv0);
  fprintf(stderr, "\t-i\tInteractive mode. File input will be ignored.\n");
  fprintf(stderr, "\t-c [out_file]\tCompile to instructions.\n");
  fprintf(stderr, "\t-r [out_file]\tOutput human-readable instructions.\n");
  fprintf(stderr, "\t-n\tDo not run the program.\n");
  exit(1);  
}

int main(int argc, char* argv[]) {
  int opt;
  char interactive_flag = 0;
  char compile_flag = 0;
  char *out_name = "out.swi";
  char readable_flag = 0;
  char *readable_name = "out.txt";
  char run_flag = 1;
  while((opt = getopt(argc, argv, "ic::r::n")) != -1) {
    switch (opt) {
      case 'i':
      interactive_flag = 1;
      break;
      case 'c':
      compile_flag = 1;
      if (optarg != NULL) {
        out_name = optarg;
      }
      break;
      case 'r':
      readable_flag = 1;
      if (optarg != NULL) {
        readable_name = optarg;
      }
      break;
      case 'n':
      run_flag = 0;
      break;
      default:
      usage(argv[0]);
    }
  }
  if (!interactive_flag) {
    if (optind + 1 == argc) {
      char *in_name = argv[optind];
      int file = open(in_name, O_RDONLY);
      dup2(file, STDIN_FILENO);
    } else {
      usage(argv[0]);
    }
  }

  yyinit();
  int result = yyparse();
  if (result) {
#ifdef DEBUG
    result = *(int*)NULL;
#else
    yyerror("Error during parsing.");
    exit(1);
#endif
  }
  
  if (compile_flag) {
    output(out_name);
  }
  if (readable_flag) {
    output_readable(readable_name);
  }

  if (run_flag) {
    // TODO run
  }
  yyclose();

  return result;
}
