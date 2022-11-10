#include <gtest/gtest.h>
#include "../RedArchiver.hpp"
#include "../encrypt.hpp"

const char *origin = "../Sources/Tests/text_to_encrypt.txt";

int equal_ptr(void* a_, void* b_, int size) {
	char *a = static_cast<char*>(a_);
	char *b = static_cast<char*>(b_);
	for (int i = 0; i < size; i++) {
		if (a[i] != b[i]) 
			return 0;
	}
	return 1;
}

TEST (ENCRYPT_TEST, Subtest_1) {
	int sz = 10000;
	const char * key = "\x63\x61\x74\x55";

	void *data = (void *)malloc(sz);
	void *cipher_data = (void *)malloc(sz);
	void *decrypted_data = (void *)malloc(sz);

	FILE *f;
	f = fopen(origin, "r");
    ASSERT_NE(f, nullptr);
	int nread = fread(data, sizeof(char), sz, f);
	fclose(f);

	int realSize = nread + blockSize - (nread % blockSize);
    int destSize = 0;
	encrypt(data, nread, cipher_data, destSize, key);

	decrypt(cipher_data, nread, decrypted_data, destSize, key);
	
    ASSERT_EQ(equal_ptr(data, decrypted_data, realSize), 1);

	free(cipher_data);
	free(data);
	free(decrypted_data);

}
