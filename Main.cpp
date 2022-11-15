#include "Sources/RedArchiver.hpp"

int main(int argc, char * argv[]) {
	ASSERT(argc > 2, ARGS_ERROR, "Too few arguments!");

	char firstArg = parseSpecialSymbol(argv[1]);

	switch (firstArg) {
		case 'e': encrypt(argc - 2, argv + 2); break;
		case 'd': decrypt(argc - 2, argv + 2); break;
		default: ASSERT(0, ARGS_ERROR, "No such command!"); break;
	}

	return 0;
}
