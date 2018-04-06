#include "arc4.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#define BYTES 256


void swap(unsigned char* array, size_t k, size_t l) {
	unsigned char c = array[k];
	array[k] = array[l];
	array[l] = c;
}


unsigned char gen_pseudoaleatoria(unsigned char* state_array, arc4_t* arc4_) {
	arc4_->i = (arc4_->i + 1) &(BYTES - 1);
	arc4_->j = (arc4_->j + state_array[arc4_->i]) &(BYTES - 1);
	swap(state_array, arc4_->i, arc4_->j);
	return state_array[(state_array[arc4_->i] + 
	state_array[arc4_->j]) &(BYTES - 1)];	
}


void arc4_create(arc4_t* arc4_, size_t largo_output_and_keystream) {
	arc4_->output = malloc(sizeof(unsigned char) * largo_output_and_keystream);
	arc4_->key_stream = malloc(sizeof(unsigned char) * 
	largo_output_and_keystream);
	arc4_->i = 0;
	arc4_->j = 0;
}


void arc4_destroy(arc4_t* arc4_) {
	free(arc4_->output);
	free(arc4_->key_stream);
}
	
	
void arc4_process(unsigned char* clave, unsigned char* input, 
size_t largo_util_input, arc4_t* arc4_) {
	/* Key-Scheduling (KSA) */
	unsigned char array_estado[BYTES];
	size_t largo_clave = strlen((const char*) clave);

	for(size_t i = 0; i < BYTES; i++) {
		array_estado[i] = (unsigned char)i;
	}
	for(size_t i = 0, j = 0; i < BYTES; i++) {
		j = (j + clave[i % largo_clave] + array_estado[i]) &(BYTES - 1);
		swap(array_estado, i, j);
	}
	
	/* lleno output y key_stream*/
	for (size_t k = 0; k < largo_util_input; k++) {
		unsigned char char_pseudoaleatorio = 
		gen_pseudoaleatoria(array_estado, arc4_);
		arc4_->output[k] = input[k] ^ char_pseudoaleatorio;
		arc4_->key_stream[k] = char_pseudoaleatorio;
	}
}


unsigned char* arc4_get_output(arc4_t* arc4_) {
	return arc4_->output;
}


unsigned char* arc4_get_key_stream(arc4_t* arc4_) {
	return arc4_->key_stream;
}
