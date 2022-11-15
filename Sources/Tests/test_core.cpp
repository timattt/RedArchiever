#include <gtest/gtest.h>
#include "../RedArchiver.hpp"

const char * testFile = "../Sources/Tests/text_to_encrypt.txt";
const char * key = "abc";
const char * result = "../Sources/Tests/core_test_result.txt";
const char * dest_dir = "../Sources/Tests/Final/";
const char * dest_origin = "../Sources/Tests/Final/text_to_encrypt.txt";

TEST (CORE_TEST, Subtest_1) {
	char * argv_e[] {(char*)testFile, (char*)key, (char*)result};

	int res = 0;

	res = encrypt(3, argv_e);

	ASSERT_EQ(res, 0);

	char * argv_d[] {(char*)result, (char*)key, (char*)dest_dir};

	res = decrypt(3, argv_d);

	ASSERT_EQ(res, 0);

	int sz = 10000;
	char data1[sz] = {0};
	char data2[sz] = {0};

	FILE *f;
	f = fopen(testFile, "r");
    ASSERT_NE(f, nullptr);
	int nread = fread(data1, sizeof(char), sz, f);
	fclose(f);

	f = fopen(dest_origin, "r");
    ASSERT_NE(f, nullptr);
	nread = fread(data2, sizeof(char), sz, f);
	fclose(f);

	ASSERT_EQ(strcmp(data1, data2), 0);

	remove(result);
	remove(dest_origin);
	rmdir(dest_dir);
}
