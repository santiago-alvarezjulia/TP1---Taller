#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "arc4.h"
#include "socket.h"
#include "fprintf_hexa.h"
#define OK 0
#define ERROR -1
#define INPUT_ERROR "Parametros Incorrectos"
#define SIZEOF_CHUNK 64
#define BYTE_SIZE 1


int cliente(const char* hostname, const char* port, unsigned char* key, 
char* filename) {
	// abro el archivo
	int s;
	FILE* file;
	if (filename != NULL) {
		file = fopen(filename, "r");
	} else {
		file = stdin;
	}
	if (!file) {
		fprintf(stderr, INPUT_ERROR);
		return ERROR;
	}
	
	// creo el socket del cliente
	socket_t socket;
	s = socket_create(&socket);
	if (s < 0) {
		fclose(file);
		return ERROR;
	}
	
	// me conecto con otro socket de tipo cliente
	s = socket_connect(&socket, hostname, port);
	if (s < 0) {
		fclose(file);
		socket_destroy(&socket);
		return ERROR;
	}

	unsigned char chunk[SIZEOF_CHUNK];
	bool is_valid_socket = true;
	arc4_t arc4_;
	arc4_create(&arc4_, key, (unsigned int) strlen((const char*)key));
	
	// mientras no haya errores y pueda seguir leyendo el archivo, voy a enviar
	// datos a un socket de tipo servidor asociado a este socket de tipo cliente
	while (is_valid_socket) {
		size_t bytes_read = fread(chunk, BYTE_SIZE, SIZEOF_CHUNK, file);
		int len_chunk;
		if (bytes_read < SIZEOF_CHUNK) {
			len_chunk = bytes_read;
		} else {
			len_chunk = SIZEOF_CHUNK;
		} 

		unsigned char output[len_chunk];
		unsigned char key_stream[len_chunk];
		
		// encripto lo leido en el archivo
		arc4_process(chunk, len_chunk, output, key_stream, &arc4_);
		
		// fprintfs que demanda la catedra para el tp
		fprintf_hexa(stderr, "%02X", key_stream, len_chunk);
		fprintf_hexa(stdout, "%02x", output, len_chunk);
		
		// envio los datos encriptados al socket de tipo servidor
		int s = socket_send(&socket, output, len_chunk);
		
		is_valid_socket = (s == 0);
		if (!is_valid_socket) {
			break;
		}
		if (feof(file)) {
			break;
		}
	}
	
	arc4_destroy(&arc4_);
	socket_shutdown_rw(&socket);
	socket_destroy(&socket);
	if (file != stdin) {
		fclose(file);
	}
	return OK;
}
