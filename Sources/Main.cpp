#include "RedArchiver.hpp"

int main(int argc, char * argv[]) {
	if (argc < 2) {
		error("Too few arguments!");
	}

	char firstArg = parseSpecialSymbol(argv[1]);

	switch (firstArg) {
		case 'e': encrypt(argc - 2, argv + 2); break;
		case 'd': decrypt(argc - 2, argv + 2); break;
		default: error("No such command!"); break;
	}

	return 0;
}
