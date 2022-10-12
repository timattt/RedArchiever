// includes
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <cstdio>
#include <cstring>
#include <cstdio>
#include <cwchar>

// constants
#define MAX_FILES 100
#define MAX_FILE_NAME_LENGTH 200

// methods
void encrypt(int argc, char * argv[]);
void decrypt(int argc, char * argv[]);

void archive(void * src, int srcSizeBytes, void *& dest, int & destSizeBytes);
void unarchive(void * src, int srcSizeBytes, void *& dest, int & destSizeBytes);

void encrypt(void * src, int srcSizeBytes, void *& dest, int & destSizeBytes, char * key);
void decrypt(void * src, int srcSizeBytes, void *& dest, int & destSizeBytes, char * key);

//!Reports error and closes the program
void error(const char * message);

//! something like: -a.
//! This will ensure that length is 2 and return second symbol
char parseSpecialSymbol(char * str);
