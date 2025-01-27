#include <stdio.h>
#include <string.h>

#include "argpars.h"

bool __ap_isValidChar(char c) {
  int ascii = (int)c;
  return ((ascii >= ALLOWED_CAPITAL_CHAR_LB) &&
          (ascii <= ALLOWED_CAPITAL_CHAR_UB)) ||
         ((ascii >= ALLOWED_LOWER_CHAR_LB) &&
          (ascii <= ALLOWED_LOWER_CHAR_UB));
}

bool __ap_isValidShortName(char shortName) {
  return __ap_isValidChar(shortName) || (shortName == 0x0);
}

bool __ap_isValidLongName(char *longName) {
  int i = 0;
  if (longName[i] == '-') {
    return false;
  }
  while (longName[i] != 0x0) {
    if (!__ap_isValidChar(longName[i]) && longName[i] != '-') {
      return false;
    }
    i++;
  }
  if (longName[i - 1] == '-') {
    return false;
  }
  return true;
}

int __ap_getMaxLongNameLen(ap_Config *config) {
  int maxLen = 0;
  for (int i = 0; i < config->flagc; i++) {
    int len = strlen((config->flags)[i].longName);
    if (len >= maxLen) {
      maxLen = len;
    }
  }
  return maxLen;
}

void __ap_printFlag(ap_Flag *flag) {
  printf("{\n");
  printf("acceptMultiArgs: %d\n", flag->acceptMultiArgs);
  printf("inputType: %d\n", flag->inputType);
  printf("shortName: %d\n", flag->shortName);
  printf("longName: \"%s\"\n", flag->longName);
  printf("}\n");
}

void __ap_printConfig(ap_Config *config) {
  printf("{\n");
  printf("argc: %d\n", config->flagc);
  printf("acceptMultiArgs: %d\n", config->acceptMultiArgs);
  printf("inputType: %d\n", config->inputType);
  printf("_flagI: %d\n", config->_flagI);
  printf("flags: [\n");
  for (int i = 0; i < config->flagc; i++) {
    __ap_printFlag(&(config->flags[i]));
  }
  printf("}\n");
}
