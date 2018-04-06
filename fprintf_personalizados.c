#include <stdio.h>
#include <string.h>

void fprintf_hexadecimal_stdout(const char* string, size_t largo_util_string) {
	for (int i = 0; i < largo_util_string; i++) {
		fprintf(stdout, "%x", string[i]);
	}
	fprintf(stdout, "\n");
}

void fprintf_hexadecimal_stderr(const char* string, size_t largo_util_string) {
	for (int i = 0; i < largo_util_string; i++) {
		fprintf(stderr, "%02X", string[i]);
	}
	fprintf(stdout, "\n");
}
