#include "RedArchiver.hpp"

const char * errorMessage = NULL;
const char * errorFile;
int errorLine = 0;

int fileSize(char * file_name) {
	// opening the file in read mode
	FILE *fp = fopen(file_name, "r");

	// checking if the file exist or not
	ASSERT(fp, FILES_ERROR, "File not found");

	fseek(fp, 0L, SEEK_END);

	// calculating the size of the file
	long int res = ftell(fp);

	// closing the file
	fclose(fp);

	return res;
}

int readFromFileToBuf(char * fileName, int fileSize, char * dest) {
	FILE * file = fopen(fileName, "rb");

	ASSERT(file, FILES_ERROR, "Can not open some of files");

	long long sz = fread(dest, 1, fileSize, file);

	ASSERT(sz == fileSize, FILES_ERROR, "Something bad while reading file");

	fclose(file);

	return 0;
}

int writeToFile(char * fileName, char * data, int size) {
	remove(fileName);

	FILE * file = fopen(fileName, "wb");

	ASSERT(file, FILES_ERROR, "Can not open some of files");

	long long sz = fwrite(data, 1, size, file);

	ASSERT(sz == size, FILES_ERROR, "Something bad while writing file");

	fclose(file);

	return 0;
}

char* makeShortName(char * name, int size) {
	char * cur = name + size-1;
	while (cur != name && *cur != '/') {
		cur--;
	}
	if (*cur == '/')
		cur++;
	return cur;
}

int encrypt(int argc, char * argv[]) {
	int totalFiles = argc - 2;

	ASSERT(totalFiles > 0, ARGS_ERROR, "No files");

	char * key = argv[totalFiles];
	char * outFile = argv[totalFiles + 1];

	ASSERT(argc - 2 < MAX_FILES, ARGS_ERROR, "Too many files");

	char * shortFileNames[MAX_FILES] = { 0 };
	int fileSizes[MAX_FILES] = { 0 };
	int fileNameSizes[MAX_FILES] = { 0 };
	int totalFilesSize = 0;
	int totalNamesSize = 0;

	for (int i = 0; i < totalFiles; i++) {
		SAFE_CALL(fileSizes[i] = fileSize(argv[i]));

		totalFilesSize += fileSizes[i];
		shortFileNames[i] = makeShortName(argv[i], strlen(argv[i]));
		fileNameSizes[i] = strlen(shortFileNames[i]);
		totalNamesSize += fileNameSizes[i];

		ASSERT(totalFilesSize < (2 << 29), FILES_ERROR, "Some file is too large");
#ifdef DEBUG
		printf("Found file: name=[%s], fileSize=[%d]\n", shortFileNames[i], fileSizes[i]);
#endif
	}

	// buffer to store all files
	int buf_size = 4 + 4 * totalFiles + totalNamesSize + totalFiles /*to ensure endline symbol*/ + totalFilesSize;

	char * buf = (char*) calloc(buf_size, 1);

	ASSERT(buf, MEMORY_ERROR, "Can not allocate result file buffer");

	// total files count
	int * struct_total = (int*) buf;
	*struct_total = totalFiles;

	// file names
	char * cur = buf + sizeof(int);
	for (int i = 0; i < totalFiles; i++) {
		ASSERT(strlen(shortFileNames[i]) < MAX_FILE_NAME_LENGTH, FILES_ERROR, "Too long file name");
		sprintf(cur, "%s", shortFileNames[i]);
		cur += (strlen(shortFileNames[i]) + 1);
	}

	// files data
	char * pointers_start = cur;
	cur = pointers_start + sizeof(int) * totalFiles;

	for (int i = 0; i < totalFiles; i++) {
		// store data
		SAFE_CALL(readFromFileToBuf(argv[i], fileSizes[i], cur));
		char * ptr = pointers_start + i * sizeof(int);
		int * ptr_i = (int*)ptr;
		// store data size
		*ptr_i = fileSizes[i];
		cur += fileSizes[i];
	}

	// PROCESS
	void * init = buf;

	void * arch = NULL;
	int arch_size = 0;

	void * encr = NULL;
	int encr_size = 0;

	SAFE_CALL(archive(init, buf_size, arch, arch_size));
	SAFE_CALL(encrypt(arch, arch_size, encr, encr_size, key));

#ifdef DEBUG
	printf("Creating outfile: name=[%s]\n", outFile);
#endif

	SAFE_CALL(writeToFile(outFile, (char*)encr, encr_size));

	free(arch);
	free(encr);
	free(buf);

	return 0;
}

int decrypt(int argc, char * argv[]) {
	ASSERT(argc == 2 || argc == 3, ARGS_ERROR, "Ты как рубашку снял?");

	// ARGS
	char * inputFileName = argv[0];
	char * key = argv[1];
	char * dest_dir = NULL;
	if (argc == 3) {
		dest_dir = argv[2];
	}

	// PROCESS
	int input_size = 0;
	SAFE_CALL(input_size = fileSize(inputFileName));

	ASSERT(input_size < (2 << 29), FILES_ERROR, "Input file is too large!");

	char * input_data = (char*) calloc(1, input_size);
	SAFE_CALL(readFromFileToBuf(inputFileName, input_size, input_data));

	void * decr = NULL;
	int decr_size = 0;

	void * unarch = NULL;
	int unarch_size = 0;

	SAFE_CALL(decrypt(input_data, input_size, decr, decr_size, key));
	SAFE_CALL(unarchive(decr, decr_size, unarch, unarch_size));

	char * buf = (char*) unarch;
	int buf_size = unarch_size;

	// UNPACK
	mkdir(dest_dir);

	int total_files = *(int*)(buf);

	ASSERT(total_files < MAX_FILES, FILES_ERROR, "File is damaged");

	char * cur = buf + sizeof(int);

	char fileNames[MAX_FILES][2*MAX_FILE_NAME_LENGTH] = { 0 };

	for (int i = 0; i < total_files; i++) {
		char name[MAX_FILE_NAME_LENGTH] = { 0 };
		sscanf(cur, "%s", name);

		if (dest_dir != NULL) {
			sprintf(fileNames[i], "%s%s", dest_dir, name);
		} else {
			sprintf(fileNames[i], "%s", name);
		}

		int len = strlen(name);

#ifdef DEBUG
		printf("Found file: name=[%s], nameSize=[%d]\n", name, len);
#endif

		cur += (len + 1);

		ASSERT(cur < buf + buf_size, FILES_ERROR, "File is damaged");
	}

	char * data = cur + total_files * sizeof(int);

	for (int i = 0; i < total_files; i++) {
		int sz = *(int*)(cur);
		cur += sizeof(int);

		ASSERT(data + sz <= buf + buf_size && sz > 0, FILES_ERROR, "File is damaged");

#ifdef DEBUG
		printf("Writing file: name=[%s], fileSize=[%d]\n", fileNames[i], sz);
#endif

		SAFE_CALL(writeToFile(fileNames[i], data, sz));

		data += sz;
	}

	free(input_data);
	free(decr);
	free(unarch);

	return 0;
}
