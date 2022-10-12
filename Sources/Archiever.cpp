#include "RedArchiver.hpp"

void archive(void * src, int srcSizeBytes, void *& dest, int & destSizeBytes) {
	// TODO RUSLAN
	// YOU MAY ALLOC SOME MEMORE AND STORE SOMETHING THERE

	// TMP
	destSizeBytes = srcSizeBytes;
	dest = malloc(destSizeBytes);
	memcpy(dest, src, srcSizeBytes);
}

void unarchive(void * src, int srcSizeBytes, void *& dest, int & destSizeBytes) {
	// TODO RUSLAN
	// YOU MAY ALLOC SOME MEMORE AND STORE SOMETHING THERE

	// TMP
	destSizeBytes = srcSizeBytes;
	dest = malloc(destSizeBytes);
	memcpy(dest, src, srcSizeBytes);
}
