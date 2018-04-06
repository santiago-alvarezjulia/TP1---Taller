#ifndef ARC4_H
#define ARC4_H

#include <stddef.h>

typedef struct {
	unsigned char* output;
	unsigned char* key_stream;
	size_t i;
	size_t j;
}arc4_t;

// algoritmo de cifrado
// post: unsigned char* almacenado en el heap
void arc4_create(arc4_t* arc4_, size_t largo_output_and_keystream);
void arc4_destroy(arc4_t* arc4_);
void arc4_process(unsigned char* clave, unsigned char* input, 
size_t largo_util_input, arc4_t* arc4_);
unsigned char* arc4_get_output(arc4_t* arc4_);
unsigned char* arc4_get_key_stream(arc4_t* arc4_); 

#endif //ARC4_H
