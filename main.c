#include <stdio.h>
#include <string.h>
#include "cliente.h"
#include "servidor.h"
#define TODO_OK 0
#define CLIENTE "client"
#define SERVIDOR "server"
#define ERROR 1
#define ERROR_ENTRADA "Parametros Incorrectos"
#define CANTIDAD_MINIMA_PARAMETROS 1


int main(int argc, char* argv[]) {
	if (argc < CANTIDAD_MINIMA_PARAMETROS) {
		fprintf(stderr, ERROR_ENTRADA);
		return ERROR;
	}
	
	if (strcmp(argv[1], CLIENTE) == 0) {
		return cliente((const char*)argv[2], 
		(const char*)argv[3], (unsigned char*)argv[4], argv[5]);
	} else if (strcmp(argv[1], SERVIDOR) == 0) {
		return servidor((const char*)argv[2], (unsigned char*)argv[3]);
	} else {
		fprintf(stderr, ERROR_ENTRADA);
		return ERROR;
	}
}
