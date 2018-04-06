#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "arc4.h"
#include "socket.h"
#include "fprintf_personalizados.h"
#define TODO_OK 0
#define ERROR -1
#define ERROR_ENTRADA "Parametros Incorrectos"
#define SIZEOF_CHUNK 50
#define NOMBRE_ARCHIVO_OUTPUT "out"
#define TAMANIO_BYTE 1

int servidor(const char* service_name, unsigned char* clave) {
	FILE* archivo = fopen(NOMBRE_ARCHIVO_OUTPUT, "w+b");
	if(!archivo) {
		fprintf(stderr, ERROR_ENTRADA);
		return ERROR;
	}
	
	// inicio el socket del servidor
	socket_t socket_;
	int s = socket_create(&socket_);
	if (s < 0) {
		fclose(archivo);
		return ERROR;
	}
	s = socket_bind_and_listen(&socket_, service_name);
	if (s < 0) {
		fclose(archivo);
		socket_destroy(&socket_);
		return ERROR;
	}
	
	socket_t socket_asociado;
	s = socket_accept(&socket_, &socket_asociado);
	if (s < 0) {
		fclose(archivo);
		socket_shutdown_rw(&socket_);
		socket_destroy(&socket_);
		return ERROR;
	}

	unsigned char crypted_chunk[SIZEOF_CHUNK + 1];
	
	bool es_socket_valido = true;
	
	while(es_socket_valido) {
		int bytes_recibidos = socket_receive(&socket_asociado, 
		crypted_chunk, SIZEOF_CHUNK);
		es_socket_valido = (bytes_recibidos > 0);
		if (!es_socket_valido) {
			break;
		}
		
		crypted_chunk[bytes_recibidos] = '\0';
		arc4_t arc4_;
		arc4_create(&arc4_, bytes_recibidos + 1);
		
		arc4_process(clave, crypted_chunk, bytes_recibidos + 1, &arc4_);
		unsigned char* output =  arc4_get_output(&arc4_);
		unsigned char* key_stream =  arc4_get_key_stream(&arc4_);
		
		fwrite(output, TAMANIO_BYTE, strlen((const char*)output), archivo);
		fprintf_hexadecimal_stdout((const char*) output, bytes_recibidos + 1);
		fprintf_hexadecimal_stderr((const char*) key_stream, 
		bytes_recibidos + 1);
		
		arc4_destroy(&arc4_);
	}
	
	socket_shutdown_rw(&socket_asociado);
	socket_destroy(&socket_asociado);
	socket_shutdown_rw(&socket_);
	socket_destroy(&socket_);
	fclose(archivo);
	return TODO_OK;
}
