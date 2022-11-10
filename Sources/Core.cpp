#include "RedArchiver.hpp"

int fileSize(char * file_name) {
	// opening the file in read mode
	FILE *fp = fopen(file_name, "r");

	// checking if the file exist or not
	if (fp == NULL) {
		error("File Not Found!");
		return -1;
	}

	fseek(fp, 0L, SEEK_END);

	// calculating the size of the file
	long int res = ftell(fp);

	// closing the file
	fclose(fp);

	return res;
}

void readFromFileToBuf(char * fileName, int fileSize, char * dest) {
	FILE * file = fopen(fileName, "rb");

	if (file == NULL) {
		error("Can not open some of files!");
	}

	long long sz = fread(dest, 1, fileSize, file);

	if (sz != fileSize) {
		error("Something bad while reading file!");
	}

	fclose(file);
}

void writeToFile(char * fileName, char * data, int size) {
	remove(fileName);

	FILE * file = fopen(fileName, "wb");

	if (file == NULL) {
		printf("%s\n", fileName);
		error("Can not open some of files!");
	}

	long long sz = fwrite(data, 1, size, file);

	if (sz != size) {
		error("Something bad while writing file!");
	}

	fclose(file);
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

void encrypt(int argc, char * argv[]) {
	int totalFiles = argc - 2;

	if (totalFiles <= 0) {
		error("No files!");
	}

	char * key = argv[totalFiles];
	char * outFile = argv[totalFiles + 1];

	if (argc - 2 > MAX_FILES) {
		error("Too many files!");
	}

	char * shortFileNames[MAX_FILES] = { 0 };
	int fileSizes[MAX_FILES] = { 0 };
	int fileNameSizes[MAX_FILES] = { 0 };
	int totalFilesSize = 0;
	int totalNamesSize = 0;

	for (int i = 0; i < totalFiles; i++) {
		fileSizes[i] = fileSize(argv[i]);
		totalFilesSize += fileSizes[i];
		shortFileNames[i] = makeShortName(argv[i], strlen(argv[i]));
		fileNameSizes[i] = strlen(shortFileNames[i]);
		totalNamesSize += fileNameSizes[i];

		printf("Found file: name=[%s], fileSize=[%d]\n", shortFileNames[i], fileSizes[i]);
	}

	// buffer to store all files
	int buf_size = 4 + 4 * totalFiles + totalNamesSize + totalFiles /*to ensure endline symbol*/ + totalFilesSize;
	char * buf = (char*) calloc(buf_size, 1);

	// total files count
	int * struct_total = (int*) buf;
	*struct_total = totalFiles;

	// file names
	char * cur = buf + sizeof(int);
	for (int i = 0; i < totalFiles; i++) {
		if (strlen(shortFileNames[i]) > MAX_FILE_NAME_LENGTH) {
			error("Too long file name!");
		}
		sprintf(cur, "%s", shortFileNames[i]);
		cur += (strlen(shortFileNames[i]) + 1);
	}

	// files data
	char * pointers_start = cur;
	cur = pointers_start + sizeof(int) * totalFiles;

	for (int i = 0; i < totalFiles; i++) {
		// store data
		readFromFileToBuf(argv[i], fileSizes[i], cur);
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

	archive(init, buf_size, arch, arch_size);
	encrypt(arch, arch_size, encr, encr_size, key);

	printf("Creating outfile: name=[%s]\n", outFile);

	writeToFile(outFile, (char*)encr, encr_size);

	free(arch);
	free(encr);
	free(buf);
}

void decrypt(int argc, char * argv[]) {
	if (argc != 2 && argc != 3) {
		error("Ты как рубашку снял?");
	}

	// ARGS
	char * inputFileName = argv[0];
	char * key = argv[1];
	char * dest_dir = NULL;
	if (argc == 3) {
		dest_dir = argv[2];
	}

	// PROCESS
	int input_size = fileSize(inputFileName);
	char * input_data = (char*) calloc(1, input_size);
	readFromFileToBuf(inputFileName, input_size, input_data);

	void * decr = NULL;
	int decr_size = 0;

	void * unarch = NULL;
	int unarch_size = 0;

	decrypt(input_data, input_size, decr, decr_size, key);
	unarchive(decr, decr_size, unarch, unarch_size);

	char * buf = (char*) unarch;
	int buf_size = unarch_size;

	// UNPACK
	mkdir(dest_dir);

	int total_files = *(int*)(buf);

	if (total_files > MAX_FILES) {
		error("File is damaged!");
	}

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
		printf("Found file: name=[%s], nameSize=[%d]\n", name, len);

		cur += (len + 1);

		if (cur > buf + buf_size) {
			error("File is damaged!");
		}
	}

	char * data = cur + total_files * sizeof(int);

	for (int i = 0; i < total_files; i++) {
		int sz = *(int*)(cur);
		cur += sizeof(int);

		if (data + sz > buf + buf_size || sz < 0) {
			error("File is damaged!");
		}

		printf("Writing file: name=[%s], fileSize=[%d]\n", fileNames[i], sz);

		writeToFile(fileNames[i], data, sz);

		data += sz;
	}

	free(input_data);
	free(decr);
	free(unarch);
}
