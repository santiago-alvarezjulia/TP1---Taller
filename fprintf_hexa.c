#include <stdio.h>

void fprintf_hexa(FILE* file, char* flag, unsigned char* input, int len_input) {
	for (int i = 0; i < len_input; i++){		
		fprintf(file, flag, input[i]);
	}
}
