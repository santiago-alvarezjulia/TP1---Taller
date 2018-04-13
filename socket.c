#define _POSIX_C_SOURCE 200112L
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#include <unistd.h>
#include "socket.h"
#define TODO_OK 0
#define ERROR -1
#define CLIENTES_ESPERANDO_MAX 5


int socket_create(socket_t* socket_) {
	int sock = socket(AF_INET, SOCK_STREAM, 0);  
	if (sock < 0) {
		fprintf(stderr, "Error: %s\n", strerror(errno));
		return ERROR;
	}
	socket_->socket = sock;
    return TODO_OK;
}


int socket_connect(socket_t* socket_, const char* hostname, const char* port) {
	struct sockaddr_in cliente;  

	cliente.sin_family = AF_INET;
	cliente.sin_port = htons((uint16_t)atoi(port)); 
	cliente.sin_addr.s_addr = inet_addr(hostname);  
	
	if (connect(socket_->socket, 
	(struct sockaddr *)&cliente, sizeof(cliente)) < 0) {
		fprintf(stderr, "Error: %s\n", strerror(errno));
		return ERROR;
	}
    return TODO_OK;
}


int socket_send(socket_t* socket_, unsigned char* chunk, size_t sizeof_chunk) {
	int bytes_enviados = 0;
	int s;
	bool es_socket_valido = true;
	bool es_socket_abierto = true;
	
	while(bytes_enviados < sizeof_chunk && es_socket_valido && 
	es_socket_abierto) {
		s = send(socket_->socket, &chunk[bytes_enviados], 
		sizeof_chunk - bytes_enviados, MSG_NOSIGNAL);
		if (s < 0) {
			es_socket_valido = false;
		} else if (s == 0) {
			es_socket_abierto = false;
		} else {
			bytes_enviados += s;
		}
	}	
	if (es_socket_valido) {
		return TODO_OK;
	}
	return ERROR;
}

	
int socket_bind_and_listen(socket_t* socket_, const char* port) {
	int val = 1;
	setsockopt(socket_->socket, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
	
	struct addrinfo hints;
	struct addrinfo *result;
	int s = 0;
	
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	s = getaddrinfo(NULL, port, &hints, &result);
	
	s = bind(socket_->socket, result->ai_addr, result->ai_addrlen);
	if (s == -1) {
		fprintf(stderr,"Error : %s\n", strerror(errno));
		return ERROR;
	}
	freeaddrinfo(result);
	
	s = listen(socket_->socket, CLIENTES_ESPERANDO_MAX);
	if (s == -1) {
		fprintf(stderr,"Error : %s\n", strerror(errno));
		return ERROR;
	}
	return TODO_OK;
}

int socket_accept(socket_t* socket_viejo, socket_t* socket_nuevo){
	int s = accept(socket_viejo->socket, NULL, NULL);
	if (s < 0) {
		return ERROR;
	}
	socket_nuevo->socket = s;
	return TODO_OK;
}


int socket_receive(socket_t* socket_, unsigned char* chunk, 
size_t sizeof_chunk) {
	int bytes_recibidos = 0;
	int s;
	bool es_socket_abierto = true;
	bool es_socket_valido = true;
	
	while(bytes_recibidos < sizeof_chunk && es_socket_valido 
	&& es_socket_abierto) {
		s = recv(socket_->socket, &chunk[bytes_recibidos], 
		sizeof_chunk - bytes_recibidos, 0);
		if (s < 0) {
			es_socket_valido = false;
		} else if (s == 0) {
			es_socket_abierto = false;
		} else {	
			bytes_recibidos += s;
		}
	}	
		
	if (!es_socket_abierto && es_socket_valido) {
		return bytes_recibidos;
	}
	return ERROR;
}


void socket_shutdown_rw(socket_t* socket_) {
	shutdown(socket_->socket, SHUT_RDWR);
}


void socket_destroy(socket_t* socket_) {
	close(socket_->socket);
}
