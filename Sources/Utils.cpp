#include "RedArchiver.hpp"

void error(const char * message) {
	printf("%s\n", message);
	exit(0);
}

char parseSpecialSymbol(char * str) {
	int len = strlen(str);

	if (len != 2) {
		error("Bad argument length!");
		return 0;
	}

	return str[1];
}
