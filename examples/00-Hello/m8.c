#include "m8.h"


int main(const int argc, const char* const argv[]) {

	const char* const source_files[] = { "main.c" };
	output = "hello"_executable;
	return m8_main(argc, argv, enumerate(source_files), enumerate(default_build_commands));
}
