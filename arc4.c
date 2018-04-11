#include "arc4.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#define BYTES 256


void swap(size_t k, size_t l, arc4_t* arc4_) {
	unsigned char c = arc4_->state_array[k];
	arc4_->state_array[k] = arc4_->state_array[l];
	arc4_->state_array[l] = c;
}


unsigned char gen_pseudoaleatoria(arc4_t* arc4_) {
	arc4_->i = (arc4_->i + 1) &(BYTES - 1);
	arc4_->j = (arc4_->j + arc4_->state_array[arc4_->i]) &(BYTES - 1);
	swap(arc4_->i, arc4_->j, arc4_);
	return arc4_->state_array[(arc4_->state_array[arc4_->i] + 
	arc4_->state_array[arc4_->j]) &(BYTES - 1)];	
}


void arc4_create(arc4_t* arc4_) {
	arc4_->i = 0;
	arc4_->j = 0;
}


void arc4_destroy(arc4_t* arc4_) {
	//
}
	
	
void arc4_process(unsigned char* clave, unsigned char* input, 
size_t largo_util_input, unsigned char* output, unsigned char* key_stream, arc4_t* arc4_) {
	/* Key-Scheduling (KSA) */
	
	unsigned char array_est[BYTES];
	arc4_->state_array = array_est;
	for(size_t i = 0; i < BYTES; i++) {
		arc4_->state_array[i] = (unsigned char)i;
	}
	
	size_t largo_clave = strlen((const char*) clave);

	for(int m = 0, n = 0; m < BYTES; m++) {
		n = (n + clave[m % largo_clave] + arc4_->state_array[m]) &(BYTES - 1);
		swap(m, n, arc4_);
	}
	
	/* lleno output y key_stream*/
	for (size_t k = 0; k < largo_util_input; k++) {
		unsigned char char_pseudoaleatorio = 
		gen_pseudoaleatoria(arc4_);
		output[k] = input[k] ^ char_pseudoaleatorio;
		key_stream[k] = char_pseudoaleatorio;
	}
}
/*
int main () {
	char* clave = "Key";
	char* input = "Plaintext";
	size_t largo_output_and_keystream = strlen((const char*)input);
	arc4_t arc4_;
	arc4_create(&arc4_);
	unsigned char output[largo_output_and_keystream];
	unsigned char key_stream[largo_output_and_keystream];
	arc4_process((unsigned char*)clave, (unsigned char*)input, largo_output_and_keystream, output, key_stream, &arc4_);

	for (int i = 0; i < strlen((const char*)input); i++){		
		printf("%02X", key_stream[i]);
	}
	printf("\n");
	for (int i = 0; i < strlen((const char*)input); i++){		
		printf("%02x", output[i]);
	}
	printf("\n");

	arc4_destroy(&arc4_);
	
	
	
	arc4_t arc4_2;
	arc4_create(&arc4_2);
	unsigned char output2[largo_output_and_keystream];
	unsigned char key_stream2[largo_output_and_keystream];

	arc4_process((unsigned char*)clave, output, largo_output_and_keystream, output2, key_stream2, &arc4_2);

	for (int i = 0; i < strlen((const char*)input); i++){		
		printf("%02X", key_stream2[i]);
	}
	printf("\n");
	for (int i = 0; i < strlen((const char*)input); i++){		
		printf("%02x", output2[i]);
	}
	printf("\n");
	
	FILE* archivo = fopen("out", "wb");

	fwrite(output, 1, strlen((const char*)input), archivo);

	arc4_destroy(&arc4_2);
	return 0;
}
*/
