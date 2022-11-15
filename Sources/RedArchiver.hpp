#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdio>
#include <cstring>
#include <cstdio>
#include <cwchar>
#include <dir.h>

// errors
extern const char * errorMessage;
extern int errorLine;
extern const char * errorFile;

#define ARGS_ERROR 1
#define FILES_ERROR 2
#define MEMORY_ERROR 3
#define ASSERT(COND, ERR_CODE, MESSAGE) if (!(COND)) {errorLine = __LINE__; errorFile = __FILE__; errorMessage = MESSAGE; return -ERR_CODE;}
#define SAFE_CALL(JOB) {int tmp = 0; if ((tmp = JOB) < 0) {return tmp;}}

#define MAX_FILES 100
#define MAX_FILE_NAME_LENGTH 200


// methods
int encrypt(int argc, char * argv[]);
int decrypt(int argc, char * argv[]);

int archive(void * src, int srcSizeBytes, void *& dest, int & destSizeBytes);
int unarchive(void * src, int srcSizeBytes, void *& dest, int & destSizeBytes);

int encrypt(void * src, int srcSizeBytes, void *& dest, int & destSizeBytes, const char * key);
int decrypt(void * src, int srcSizeBytes, void *& dest, int & destSizeBytes, const char * key);

//! something like: -a.
//! This will ensure that length is 2 and return second symbol
char parseSpecialSymbol(char * str);
