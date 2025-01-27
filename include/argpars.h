#include <stdbool.h>

static const int ALLOWED_CAPITAL_CHAR_LB = 0x41;
static const int ALLOWED_CAPITAL_CHAR_UB = 0x5A;
static const int ALLOWED_LOWER_CHAR_LB = 0x61;
static const int ALLOWED_LOWER_CHAR_UB = 0x7A;

typedef enum ap_ErrorType {
  OK,
  // ARGC_NEQ_FLAGS_LEN,

  // Flag config Errors
  FLAG_OVERFLOW,
  DUPLICATE_SHORTNAME,
  DISALLOWED_SHORTNAME,
  DUPLICATE_LONGNAME,
  DISALLOWED_LONGNAME,

  // Parse Errors
  INCOMPLETE_FLAG_LIST,
  TOO_MANY_FLAGS_PROVIDED,
  EMPTY_FLAG_PROVIDED,
  WRONG_FLAG_PREFIX_PROVIDED,
  DUPLICATE_FLAG_PROVIDED,
  UNKNOWN_FLAG,
} ap_ErrorType;

// const char *const ap_ErrorTypeStr[] =
//     {
//         [OK] = "OK",

//         [FLAG_OVERFLOW] = "FLAG_OVERFLOW",
//         [DUPLICATE_SHORTNAME] = "DUPLICATE_SHORTNAME",
//         [DISALLOWED_SHORTNAME] = "DISALLOWED_SHORTNAME",
//         [DUPLICATE_LONGNAME] = "DUPLICATE_LONGNAME",
//         [DISALLOWED_LONGNAME] = "DISALLOWED_LONGNAME",

//         [INCOMPLETE_FLAG_LIST] = "INCOMPLETE_FLAG_LIST",
//         [TOO_MANY_FLAGS_PROVIDED] = "TOO_MANY_FLAGS_PROVIDED",
//         [EMPTY_FLAG_PROVIDED] = "EMPTY_FLAG_PROVIDED",
//         [WRONG_FLAG_PREFIX_PROVIDED] = "WRONG_FLAG_PREFIX_PROVIDED",
//         [DUPLICATE_FLAG_PROVIDED] = "DUPLICATE_FLAG_PROVIDED",
//         [UNKNOWN_FLAG] = "UNKNOWN_FLAG",
// };

typedef enum ap_InputType {
  NUMBER,
  STRING,
  BOOLEAN
} ap_InputType;

typedef struct ap_Flag {
  bool acceptMultiArgs;
  ap_InputType inputType;
  char shortName;
  char *longName;
} ap_Flag;

typedef struct ap_Config {
  int flagc;
  bool acceptMultiArgs;
  ap_InputType inputType;

  // Internal flag index for stack like behavior
  int _flagI;
  ap_Flag *flags;
} ap_Config;

typedef struct ap_FlagResult {
  ap_Flag *flagPtr;

  bool isSet;
  char **flagv;

  int _providedIndex;
} ap_FlagResult;

// Will contain the parsed and structured information
typedef struct ap_Result {
  int argc;
  ap_FlagResult mainResult;
  ap_FlagResult *flagResults;
} ap_Result;

// Helper functions
bool __ap_isValidChar(char c);
bool __ap_isValidShortName(char shortName);
bool __ap_isValidLongName(char *longName);
int __ap_getMaxLongNameLen(ap_Config *config);
void __ap_printFlag(ap_Flag *flag);
void __ap_printConfig(ap_Config *config);

// Constructor
ap_ErrorType ap_argparsInit(ap_Config *config, int argc, bool acceptMultiArgs, ap_InputType inputType);
// Flag config
ap_ErrorType ap_addFlag(ap_Config *config, bool acceptMultiArgs, ap_InputType inputType, char shortName, char longName[]);
// Actual parsing of args
ap_ErrorType ap_parse(ap_Result *resultPtr, ap_Config *config, int argc, char **argv);

ap_ErrorType ap_providedFlagsValidity(ap_Config *config, int argc, char **argv, int *flagCount);
ap_ErrorType ap_providedShortFlagValidity(ap_Config *config, char *flag);
ap_ErrorType ap_providedLongFlagValidity(ap_Config *config, char *flag);
// ap_ErrorType ap_providedFlagsDuplicatesCheck(ap_Config *config, char **flags);

ap_ErrorType ap_mapProvidedFlags(ap_FlagResult **flags, ap_Config *config, int argc, char **argv, int flagc);

// Printing
void ap_printTooManyFlagsProvided();
void ap_printUnknownFlagProvided(char *unknownFlag);
void ap_printEmptyFlagProvided();
void ap_printWrongFlagPrefixProvided(char *flag);
void ap_printDuplicateFlagProvided(char *duplicateFlag);
void ap_printHelp();