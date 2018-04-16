#define _POSIX_C_SOURCE 200112L
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "socket.h"
#define OK 0
#define ERROR -1
#define MAX_CLIENTS_WAITING 5


int socket_create(socket_t* socket_) {
	int sock = socket(AF_INET, SOCK_STREAM, 0);  
	if (sock < 0) {
		fprintf(stderr, "Error: %s\n", strerror(errno));
		return ERROR;
	}
	socket_->socket = sock;
    return OK;
}


int socket_connect(socket_t* socket_, const char* hostname, const char* port) {
	struct sockaddr_in client;  

	client.sin_family = AF_INET;
	client.sin_port = htons((uint16_t)atoi(port)); 
	client.sin_addr.s_addr = inet_addr(hostname);  
	
	if (connect(socket_->socket, (struct sockaddr *)&client, 
	sizeof(client)) < 0) {
		fprintf(stderr, "Error: %s\n", strerror(errno));
		return ERROR;
	}
    return OK;
}


int socket_send(socket_t* socket_, unsigned char* chunk, size_t sizeof_chunk) {
	int bytes_enviados = 0;
	int s;
	bool is_valid_socket = true;
	bool is_open_socket = true;
	
	while(bytes_enviados < sizeof_chunk && is_valid_socket && is_open_socket) {
		s = send(socket_->socket, &chunk[bytes_enviados], 
		sizeof_chunk - bytes_enviados, MSG_NOSIGNAL);
		if (s < 0) {
			is_valid_socket = false;
		} else if (s == 0) {
			is_open_socket = false;
		} else {
			bytes_enviados += s;
		}
	}	
	if (is_valid_socket) {
		return OK;
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
	
	s = listen(socket_->socket, MAX_CLIENTS_WAITING);
	if (s == -1) {
		fprintf(stderr,"Error : %s\n", strerror(errno));
		return ERROR;
	}
	return OK;
}

int socket_accept(socket_t* socket_, socket_t* new_socket){
	int s = accept(socket_->socket, NULL, NULL);
	if (s < 0) {
		return ERROR;
	}
	new_socket->socket = s;
	return OK;
}


int socket_receive(socket_t* socket_, unsigned char* chunk, 
size_t sizeof_chunk) {
	int bytes_recibidos = 0;
	int s;
	bool is_open_socket = true;
	bool is_valid_socket = true;
	
	while(bytes_recibidos < sizeof_chunk && is_valid_socket && is_open_socket) {
		s = recv(socket_->socket, &chunk[bytes_recibidos], 
		sizeof_chunk - bytes_recibidos, 0);
		if (s < 0) {
			is_valid_socket = false;
		} else if (s == 0) {
			is_open_socket = false;
		} else {	
			bytes_recibidos += s;
		}
	}	
		
	if (!is_open_socket && is_valid_socket) {
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
