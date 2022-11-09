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
	unsigned int key[4] = {0x01234567,
						   0x89abcd12,
						   0x0ad562fe,
						   0x67abc24a};

	void *data = (void *)malloc(sizeof(char) * byteSize);
	void *cipher_data = (void *)malloc(sizeof(char) * byteSize);
	void *decrypted_data = (void *)malloc(sizeof(char) * byteSize);

	FILE *f;
	f = fopen(origin, "r");
    ASSERT_NE(f, nullptr);
	int nread = fread(data, sizeof(char), byteSize, f);
	fclose(f);

	int realSize = nread + blockSize - (nread % blockSize);
    int destSize = 0;
	encrypt(data, nread, cipher_data, destSize, key);

	decrypt(decrypted_data, nread, cipher_data, destSize, key);
	
    ASSERT_EQ(equal_ptr(data, decrypted_data, realSize), 1);

	free(cipher_data);
	free(data);
	free(decrypted_data);

}
