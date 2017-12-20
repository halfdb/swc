#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include "compiler.h"
#include "parser.h"
#include "machine.h"

void yyerror(char const * str) {
  errors++;
  fprintf(stderr, "error while parsing line %d\n", yylineno);
  fprintf(stderr, "%s\n\n", str);
}

void usage(char *argv0) {
  fprintf(stderr, "Usage: %s [-i] [-c [out_file]] [-r [out_file]] file\n", argv0);
  fprintf(stderr, "\t-i\t\tInteractive mode. File input will be ignored.\n");
  fprintf(stderr, "\t-c [out_file]\tCompile to instructions.\n");
  fprintf(stderr, "\t-r [out_file]\tOutput human-readable instructions.\n");
  fprintf(stderr, "\t-n\t\tDo not run the program.\n");
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
      yyin = fopen(in_name, "r");
      if (yyin == NULL) {
        fprintf(stderr, "unable to open file\n");
        exit(1);
      }
    } else {
      usage(argv[0]);
    }
  }

  init_table();
  init_compiler();
  int result = yyparse();
  result = result || errors;
  if (result) {
#ifdef DEBUG
    result = *(int*)NULL;
#else
    printf("Error during parsing.\n");
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
    close_table(0);
    close_compiler(0);

    init_machine();
    result = start_program(ins_stack, const_table);
    close_machine();
  }

  close_table(1);
  close_compiler(1);

  return result;
}
