#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "arc4.h"
#include "socket.h"
#include "fprintf_personalizados.h"
#define TODO_OK 0
#define ERROR -1
#define ERROR_ENTRADA "Parametros Incorrectos"
#define SIZEOF_CHUNK 64
#define TAMANIO_BYTE 1


int cliente(const char* hostname, const char* service_name, 
unsigned char* clave, char* nombre_archivo) {
	// abro el archivo
	int s;
	FILE* archivo;
	if (nombre_archivo != NULL) {
		archivo = fopen(nombre_archivo, "r");
	} else {
		archivo = stdin;
	}
	if (!archivo) {
		fprintf(stderr, ERROR_ENTRADA);
		return ERROR;
	}
	
	// inicio el socket del cliente
	socket_t socket;
	s = socket_create(&socket);
	if (s < 0) {
		fclose(archivo);
		return ERROR;
	}
	s = socket_connect(&socket, hostname, service_name);
	if (s < 0) {
		fclose(archivo);
		socket_destroy(&socket);
		return ERROR;
	}

	unsigned char chunk[SIZEOF_CHUNK + 1];
	
	bool es_socket_valido = true;
	
	while (es_socket_valido) {
		size_t bytes_leidos = fread(chunk, TAMANIO_BYTE, SIZEOF_CHUNK, archivo); 
		int largo_util_chunk;
		if (bytes_leidos < SIZEOF_CHUNK) {
			largo_util_chunk = bytes_leidos;
		} else {
			largo_util_chunk = SIZEOF_CHUNK;
		} 
		
		chunk[largo_util_chunk] = '\0';
		arc4_t arc4_;
		arc4_create(&arc4_, largo_util_chunk + 1);
		
		arc4_process(clave, chunk, largo_util_chunk + 1, &arc4_);
		
		unsigned char* crypted_output = arc4_get_output(&arc4_);
		unsigned char* key_stream = arc4_get_key_stream(&arc4_);
		fprintf_hexadecimal_stdout((const char*) crypted_output, 
		largo_util_chunk + 1);
		fprintf_hexadecimal_stderr((const char*) key_stream, 
		largo_util_chunk + 1);
		
		int s = socket_send(&socket, crypted_output, largo_util_chunk);
		
		arc4_destroy(&arc4_);
		
		es_socket_valido = (s == 0);
		if (!es_socket_valido) {
			break;
		}
		if (largo_util_chunk < SIZEOF_CHUNK) {
			break;
		}
	}
	
	socket_shutdown_rw(&socket);
	socket_destroy(&socket);
	if (archivo != stdin) {
		fclose(archivo);
	}
	return TODO_OK;
}
