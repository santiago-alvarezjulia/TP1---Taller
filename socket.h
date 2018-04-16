#ifndef SOCKET_H
#define SOCKET_H

#include <stddef.h>
#include <stdbool.h>
#include <netdb.h> 

// definicion estructura socket 
typedef struct socket {
	int socket;
}socket_t;


// declaracion primitivas socket
int socket_create(socket_t* socket);
int socket_connect(socket_t* socket, 
const char* hostname, const char* service_name);

int socket_send(socket_t* socket_, 
unsigned char* chunk, size_t sizeof_chunk);

int socket_receive(socket_t* socket_, unsigned char* chunk, 
size_t sizeof_chunk);

int socket_bind_and_listen(socket_t* socket, const char* port);
int socket_accept(socket_t* socket_, socket_t* new_socket);
void socket_shutdown_rw(socket_t* socket);
void socket_destroy(socket_t* socket_);


#endif // SOCKET_H
