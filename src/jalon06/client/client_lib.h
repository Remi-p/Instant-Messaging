// ================ Programmation réseau : Jalon 06 ================= *\
//	Bonnotte Benjamin						       		     . \|/ /,  /
//	Perrot Remi			    ===============				     \\`''/_-- /
//	RE216				    |   Fichier   |			  Bordeaux INP --- /
//	Octobre 2015		    | client_lib.c|			  ENSEIRB  ,..\\`  /
//						    ===============			  MATMECA / | \\`  /
// ================================================================== */

#ifndef CLIENT_LIB
	#define CLIENT_LIB

	#include "../commons/contrib.h"
	#include "../commons/file.h"

	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netdb.h>
	#include <regex.h>
	
	// Actions à effectuer lors de la réception de données clavier
	void client_write(char** input, const int sckt, const pfile fichiers);
	
	// Création d'un client sur une machine cliente (pour le peer-to-peer)
	void client_on_client(const pfile fichier, const pfile fichiers, const int socket);
	
	// Création d'un serveur sur une machine cliente (pour le peer-to-peer)
	void server_on_client(const int sckt, const pfile fichier, const pfile fichiers);

	// Actions à effectuer lors de la réception de données serveurs
	bool client_read(const int sckt, char* output, const pfile fichiers, int* socket_fichier);

	// Effectue la connexion au serveur
	int do_connect(int socket, struct sockaddr_in6 serv_add);
	
	// Initialise les expressions régulières
	void init_reg();

	// Libère les structures des regex
	void free_reg();

#endif // CLIENT_LIB
