#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "arc4.h"
#include "socket.h"
#include "fprintf_hexa.h"
#define OK 0
#define ERROR -1
#define INPUT_ERROR "Parametros Incorrectos"
#define SIZEOF_CHUNK 50
#define NAME_OUTPUT_FILE "out"
#define BYTE_SIZE 1


int servidor(const char* service_name, unsigned char* key) {
	FILE* file = fopen(NAME_OUTPUT_FILE, "wb");
	if(!file) {
		fprintf(stderr, INPUT_ERROR);
		return ERROR;
	}
	
	// creo el socket del servidor
	socket_t socket_;
	int s = socket_create(&socket_);
	if (s < 0) {
		fclose(file);
		return ERROR;
	}
	
	// defino interface a la que se conecta el socket y cuantas conexiones
	// puede tener en espera
	s = socket_bind_and_listen(&socket_, service_name);
	if (s < 0) {
		fclose(file);
		socket_destroy(&socket_);
		return ERROR;
	}
	
	// dejo el socket esperando que acepte la conexion con un socket cliente
	// creo un nuevo socket asociado al socket cliente
	socket_t socket_asociado;
	s = socket_accept(&socket_, &socket_asociado);
	if (s < 0) {
		fclose(file);
		socket_shutdown_rw(&socket_);
		socket_destroy(&socket_);
		return ERROR;
	}

	unsigned char crypted_chunk[SIZEOF_CHUNK];
	bool is_valid_socket = true;
	arc4_t arc4_;
	arc4_create(&arc4_, key, (unsigned int) strlen((const char*)key));
	
	// mientras siga recibiendo datos a traves del socket asociado, voy a
	// desencriptarlos e imprimirlos
	while(is_valid_socket) {
		size_t bytes_received = socket_receive(&socket_asociado, 
		crypted_chunk, SIZEOF_CHUNK);
		is_valid_socket = (bytes_received > 0);
		if (!is_valid_socket) {
			break;
		}
		
		int len_chunk;
		if (bytes_received < SIZEOF_CHUNK) {
			len_chunk = bytes_received;
		} else {
			len_chunk = SIZEOF_CHUNK;
		} 

		unsigned char output[len_chunk];
		unsigned char key_stream[len_chunk];
		
		// desencripto los datos recibidos a traves del socket asociado
		arc4_process(crypted_chunk, len_chunk, output, key_stream, &arc4_);

		// fprintfs que demanda la catedra para el tp
		fwrite(output, BYTE_SIZE, len_chunk, file);
		fprintf_hexa(stderr, "%02X", key_stream, len_chunk);
		fprintf_hexa(stdout, "%02x", output, len_chunk);	
	}
	
	arc4_destroy(&arc4_);
	socket_shutdown_rw(&socket_asociado);
	socket_destroy(&socket_asociado);
	socket_shutdown_rw(&socket_);
	socket_destroy(&socket_);
	fclose(file);
	return OK;
}
