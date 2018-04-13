#ifndef ARC4_H
#define ARC4_H

#include <stddef.h>

typedef struct {
	unsigned char* state_array;
	size_t i;
	size_t j;
}arc4_t;

// algoritmo de cifrado
void arc4_create(arc4_t* arc4_);
void arc4_destroy(arc4_t* arc4_);
void arc4_process(unsigned char* clave, unsigned char* input, 
size_t largo_util_input, unsigned char* output, unsigned char* key_stream, arc4_t* arc4_);

#endif //ARC4_H
