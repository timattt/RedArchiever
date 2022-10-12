#include "RedArchiver.hpp"

void encrypt(void * src, int srcSizeBytes, void *& dest, int & destSizeBytes, char * key) {
	// TODO AZNAUR
	// YOU MAY ALLOC SOME MEMORE AND STORE SOMETHING THERE

	// TMP
	destSizeBytes = srcSizeBytes;
	dest = malloc(destSizeBytes);
	memcpy(dest, src, srcSizeBytes);
}
void decrypt(void * src, int srcSizeBytes, void *& dest, int & destSizeBytes, char * key) {
	// TODO AZNAUR
	// YOU MAY ALLOC SOME MEMORE AND STORE SOMETHING THERE

	// TMP
	destSizeBytes = srcSizeBytes;
	dest = malloc(destSizeBytes);
	memcpy(dest, src, srcSizeBytes);
}
