#include <stdbool.h>
#include <stdio.h>

#include "argpars.h"

void testProvidedFlagValidityFromList(int argc, char **argv) {
  ap_Config cfg;
  ap_argparsInit(&cfg, 3, false, STRING);

  ap_addFlag(&cfg, false, STRING, 'h', "help");
  ap_addFlag(&cfg, false, STRING, 'n', "number");
  ap_addFlag(&cfg, false, STRING, 'b', "isBoolean");

  ap_Result rslt;
  ap_ErrorType err = ap_parse(&rslt, &cfg, argc, argv);
  printf("%d\n", err);
}

void testProvidedFlagValidity() {
  char *l1[6] = {"./m", "-h", "what", "--number", "hellooo", "hello"};
  testProvidedFlagValidityFromList(6, l1);
  // char **l2 = {"./m", "--"};
  // testProvidedFlagValidityFromList(3, l1);
  // char **l3 = {"./m", "---"};
  // testProvidedFlagValidityFromList(3, l1);
  // char **l4 = {"./m", "-h"};
  // testProvidedFlagValidityFromList(3, l1);
  // char **l5 = {"./m", "-n", "-b"};
  // testProvidedFlagValidityFromList(3, l1);
  // char **l6 = {"./m", "-number", "-b"};
  // testProvidedFlagValidityFromList(3, l1);
}

int main(int argc, char **argv) {
  // ap_Config cfg;
  // ap_argparsInit(&cfg, 3, false, STRING);

  // ap_addFlag(&cfg, false, STRING, 'h', "help");
  // ap_addFlag(&cfg, false, STRING, 'n', "number");
  // ap_addFlag(&cfg, false, STRING, 'b', "isBoolean");

  // ap_Result rslt;
  // ap_parse(&rslt, &cfg, argc, argv);

  // free(cfg);

  testProvidedFlagValidity();

  return 0;
}
