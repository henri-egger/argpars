#include "argpars.h"

#include <stdlib.h>
#include <string.h>

void ap_printTooManyFlagsProvided() {}
void ap_printUnknownFlagProvided(char *unknownFlag) {}
void ap_printEmptyFlagProvided() {}
void ap_printWrongFlagPrefixProvided(char *flag) {}
void ap_printDuplicateFlagProvided(char *duplicateFlag) {}
void ap_printHelp() {}

ap_ErrorType ap_argparsInit(ap_Config *configPtr, int flagc, bool acceptMultiArgs, ap_InputType inputType) {
  // NEEDS FREE
  ap_Flag *flags = calloc(flagc, sizeof(ap_Flag));
  ap_Config config = {flagc, acceptMultiArgs, inputType, 0, flags};
  *configPtr = config;

  return OK;
}

// If flag should not have a shortName, pass 0x0
ap_ErrorType ap_addFlag(ap_Config *config, bool acceptMultiArgs, ap_InputType inputType, char shortName, char *longName) {
  if (config->_flagI == config->flagc) {
    return FLAG_OVERFLOW;
  }

  for (int i = 0; i < config->_flagI; i++) {
    if (config->flags[i].shortName == shortName) {
      return DUPLICATE_SHORTNAME;
    }
  }

  for (int i = 0; i < config->_flagI; i++) {
    if (strcmp(config->flags[i].longName, longName) == 0) {
      return DUPLICATE_LONGNAME;
    }
  }

  if (!__ap_isValidShortName(shortName)) {
    return DISALLOWED_SHORTNAME;
  }

  if (!__ap_isValidLongName(longName)) {
    return DISALLOWED_LONGNAME;
  }

  ap_Flag flag = {acceptMultiArgs, inputType, shortName, longName};
  config->flags[config->_flagI] = flag;
  config->_flagI++;

  return OK;
}

ap_ErrorType ap_providedShortFlagValidity(ap_Config *config, char *flag) {
  // Check for empty flag or wrong prefix
  if (strlen(flag) > 1) {
    ap_printWrongFlagPrefixProvided(flag);
    ap_printHelp();
    return WRONG_FLAG_PREFIX_PROVIDED;
  } else if (strlen(flag) == 0) {
    ap_printEmptyFlagProvided();
    ap_printHelp();
    return EMPTY_FLAG_PROVIDED;
  }

  char flagChar = flag[0];

  // Compare against registered shortFlags
  bool isRegistered = false;
  for (int i = 0; i < config->flagc; i++) {
    if (flagChar == (config->flags)[i].shortName) {
      isRegistered = true;
      break;
    }
  }

  if (!isRegistered) {
    ap_printUnknownFlagProvided(flag);
    ap_printHelp();
    return UNKNOWN_FLAG;
  }

  return OK;
}

ap_ErrorType ap_providedLongFlagValidity(ap_Config *config, char *flag) {
  // Check for empty flag or wrong prefix
  if (strlen(flag) == 1) {
    ap_printWrongFlagPrefixProvided(flag);
    ap_printHelp();
    return WRONG_FLAG_PREFIX_PROVIDED;
  } else if (strlen(flag) == 0) {
    ap_printEmptyFlagProvided();
    ap_printHelp();
    return EMPTY_FLAG_PROVIDED;
  }

  // Compare against registered shortFlags
  bool isRegistered = false;
  for (int i = 0; i < config->flagc; i++) {
    if (strcmp(flag, (config->flags)[i].longName) == 0) {
      isRegistered = true;
      break;
    }
  }

  if (!isRegistered) {
    ap_printUnknownFlagProvided(flag);
    ap_printHelp();
    return UNKNOWN_FLAG;
  }

  return OK;
}

ap_ErrorType ap_providedFlagsValidity(ap_Config *config, int argc, char **argv) {
  // Starting at 1, omitting program name
  // Copying flags from argv into **flags
  int flagc = 0;
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      // Strip leading - or --
      // Check for flag validity
      char *flag = argv[i];
      if (flag[1] == '-') {
        flag += 2;
        ap_ErrorType flagValidity = ap_providedLongFlagValidity(config, flag);
        if (flagValidity != 0) {
          return flagValidity;
        }
      } else {
        flag += 1;
        ap_ErrorType flagValidity = ap_providedShortFlagValidity(config, flag);
        if (flagValidity != 0) {
          return flagValidity;
        }
      }

      flagc++;
    }
  }

  if (flagc > config->flagc) {
    ap_printTooManyFlagsProvided();
    ap_printHelp();
    return TOO_MANY_FLAGS_PROVIDED;
  }

  return OK;
}

// Returns array references to the config flags
ap_ErrorType ap_mapProvidedFlags(ap_FlagResult **mappedFlagsPtr, ap_Config *config, int argc, char **argv) {
  // NEEDS FREE
  ap_FlagResult *mappedFlags = calloc(config->flagc, sizeof(ap_FlagResult));

  // Create array of FlagResult from config
  for (int i = 0; i < config->flagc; i++) {
    ap_FlagResult mappedFlag = {
        &((config->flags)[i]),
        false,
        NULL,
        -1,
    };
    mappedFlags[i] = mappedFlag;
  }

  // Check provided flags against mapped flags
  // Check for duplicates
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      // Strip leading - or --
      // Check for flag validity
      char *flag = argv[i];
      if (flag[1] == '-') {
        flag += 2;

        for (int j = 0; j < config->flagc; j++) {
          if (strcmp(mappedFlags[j].flagPtr->longName, flag) == 0) {
            if (mappedFlags[j].isSet == true) {
              ap_printDuplicateFlagProvided(flag);
              ap_printHelp();
              return DUPLICATE_FLAG_PROVIDED;
            }
            mappedFlags[j].isSet = true;
            mappedFlags[j]._providedIndex = i;
            break;
          }
        }

      } else {
        flag += 1;
        char flagChar = flag[0];

        for (int j = 0; j < config->flagc; j++) {
          if (mappedFlags[j].flagPtr->shortName == flagChar) {
            if (mappedFlags[j].isSet == true) {
              ap_printDuplicateFlagProvided(flag);
              ap_printHelp();
              return DUPLICATE_FLAG_PROVIDED;
            }
            mappedFlags[j].isSet = true;
            mappedFlags[j]._providedIndex = i;
            break;
          }
        }
      }
    }
  }

  *mappedFlagsPtr = mappedFlags;

  return OK;
}

// resultPtr - pointer which to store the result to
// config - readonly config information
// argc - users argc
// argv - users argv
ap_ErrorType ap_parse(ap_Result *resultPtr, ap_Config *config, int argc, char **argv) {
  if (config->_flagI != config->flagc) {
    return INCOMPLETE_FLAG_LIST;
  }

  ap_ErrorType providedFlagsValidity = ap_providedFlagsValidity(config, argc, argv);
  if (providedFlagsValidity != 0) {
    return providedFlagsValidity;
  }

  ap_FlagResult *flags;
  ap_ErrorType providedFlagsMappingValidity = ap_mapProvidedFlags(&flags, config, argc, argv);
  if (providedFlagsMappingValidity != 0) {
    return providedFlagsMappingValidity;
  }

  // Implement
  ap_mapArgcToFlags(&flags, argc, argv);

  free(flags);
  return OK;
}
