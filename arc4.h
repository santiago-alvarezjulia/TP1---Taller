#ifndef ARC4_H
#define ARC4_H

#include <stddef.h>

typedef struct {
	unsigned char* state_array;
	unsigned int i;
	unsigned int j;
}arc4_t;

// algoritmo de cifrado
void arc4_create(arc4_t* arc4_, unsigned char* key, unsigned int len_key);
void arc4_destroy(arc4_t* arc4_);
void arc4_process(unsigned char* input, size_t largo_util_input, 
unsigned char* output, unsigned char* key_stream, arc4_t* arc4_);

#endif //ARC4_H
