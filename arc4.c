#include "arc4.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#define BYTES 256


void swap_(size_t k, size_t l, arc4_t* arc4_) {
	unsigned char c = arc4_->state_array[k];
	arc4_->state_array[k] = arc4_->state_array[l];
	arc4_->state_array[l] = c;
}


unsigned char gen_pseudoaleatoria(arc4_t* arc4_) {
	arc4_->i = (arc4_->i + 1) % BYTES;
	arc4_->j = (arc4_->j + arc4_->state_array[arc4_->i]) % BYTES;
	swap_(arc4_->i, arc4_->j, arc4_);
	return arc4_->state_array[(arc4_->state_array[arc4_->i] + 
	arc4_->state_array[arc4_->j]) % BYTES];	
}


void arc4_create(arc4_t* arc4_) {
	arc4_->state_array = (unsigned char*) malloc(sizeof(unsigned char) * BYTES);
	for(size_t i = 0; i < BYTES; i++) {
		arc4_->state_array[i] = (unsigned char)i;
	}
	arc4_->i = 0;
	arc4_->j = 0;
}


void arc4_destroy(arc4_t* arc4_) {
	free(arc4_->state_array);
}
	
	
void arc4_process(unsigned char* clave, unsigned char* input, 
size_t largo_util_input, unsigned char* output, unsigned char* key_stream, arc4_t* arc4_) {
	/* Key-Scheduling (KSA) */
	size_t largo_clave = strlen((const char*) clave);

	for(int m = 0, n = 0; m < BYTES; m++) {
		n = (n + clave[m % largo_clave] + arc4_->state_array[m]) % BYTES;
		swap_(m, n, arc4_);
	}
	
	/* lleno output y key_stream*/
	for (size_t k = 0; k < largo_util_input; k++) {
		unsigned char char_pseudoaleatorio = 
		gen_pseudoaleatoria(arc4_);
		output[k] = input[k] ^ char_pseudoaleatorio;
		key_stream[k] = char_pseudoaleatorio;
	}
}
