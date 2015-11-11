// ================ Programmation réseau : Jalon 06 ================= *\
//	Bonnotte Benjamin						       		     . \|/ /,  /
//	Perrot Remi			    ===============				     \\`''/_-- /
//	RE216				    |   Fichier   |			  Bordeaux INP --- /
//	Octobre 2015		    |  network.h  |			  ENSEIRB  ,..\\`  /
//						    ===============			  MATMECA / | \\`  /
// ================================================================== */

#ifndef SERVER_H

	#define SERVER_H
	
	#define MAX_CLIENTS 20

	#include <sys/types.h>
	
	// ------------ Construction des codes de retour ------------------
	// (grossièrement inspiré des codes FTP
	// https://en.wikipedia.org/wiki/List_of_FTP_server_return_codes )
	
	// On code le tout sur un octet; les deux premiers bits = type de
	// code de retour, le reste désigne le code lui-même
	enum reponse_type {
		REPONSE_OK, // 0
		REPONSE_NOK
	};
	
	enum ok_code_type {
		ANY_OK,
		ADR_STRUCTURE,
		ADR_PORT,
		MSG_PERSO,
		MSG_ALL,
		MSG_SALON,
		MSG_SERVEUR,
		MSG_CLIENT,
		FILE_REFUSED,
		FILE_LENGTH,
		FILE_RECV
	};
	
	enum nok_code_type {
		ANY_NOK,
		TOO_MANY_CONN,
		PSEUDO,
		COMMAND,
		USER_NOT_FOUND,
		SALON_NOT_FD,
		CLIENT_BUSY,
		USER_NIN_SALON
	};
	
	enum code {
		OK_ANY 			= ANY_OK 			| (REPONSE_OK << 6),
		OK_STRUCT 		= ADR_STRUCTURE 	| (REPONSE_OK << 6),
		OK_PORT 		= ADR_PORT		 	| (REPONSE_OK << 6),
		OK_PERSO 		= MSG_PERSO 		| (REPONSE_OK << 6),
		OK_ALL 			= MSG_ALL 			| (REPONSE_OK << 6),
		OK_SALON 		= MSG_SALON 		| (REPONSE_OK << 6),
		OK_SERVEUR 		= MSG_SERVEUR 		| (REPONSE_OK << 6),
		OK_CLIENT 		= MSG_CLIENT 		| (REPONSE_OK << 6),
		OK_FILE_REFUSED = FILE_REFUSED 		| (REPONSE_OK << 6),
		OK_LEN			= FILE_LENGTH		| (REPONSE_OK << 6),
		OK_RECV			= FILE_RECV			| (REPONSE_OK << 6),
		NOK_ANY 		= ANY_NOK 			| (REPONSE_NOK << 6),
		NOK_CONN 		= TOO_MANY_CONN 	| (REPONSE_NOK << 6),
		NOK_PSEUDO 		= PSEUDO 			| (REPONSE_NOK << 6),
		NOK_CMD 		= COMMAND 			| (REPONSE_NOK << 6),
		NOK_USER_NF 	= USER_NOT_FOUND 	| (REPONSE_NOK << 6),
		NOK_SALON_NF 	= SALON_NOT_FD 		| (REPONSE_NOK << 6),
		NOK_USER_BUSY 	= CLIENT_BUSY 		| (REPONSE_NOK << 6),
		NOK_NOT_IN 		= USER_NIN_SALON 	| (REPONSE_NOK << 6)
	};
	// ----------------------------------------------------------------

	typedef struct sockaddr_in* psockaddr;

	// Converti une structure IPv6 en IPv4
	psockaddr convert_ipv6(struct sockaddr_in6 address);

	//init the self address structure
	struct sockaddr_in6* get_addr_info(int port, char* hostname);

	// Retourne le port d'une structure sockaddr
	u_short get_port(struct sockaddr* adresse);

	// Creation of bind
	void do_bind(int socket, struct sockaddr_in6* serv_add);

	// Listenning for connections
	void listen_of_ambiance(int socket);
	
	// Creation of socket 
	int do_socket();
	
	// Accept the socket
	int do_accept(int sckt, struct sockaddr* adresse);

	// Message sending
	void handle_client_message(int socket, char *input, int taille, enum code code_ret);

	// Read les données envoyées par notre ami le client
	bool do_read(int socket, char *output, int taille, enum code* code_ret);

	// netstat -n -A inet pour les connexions actives
	void close_socket(int socket);

	// Check s'il y a une erreur avec la socket
	int socket_error(int socket);

#endif //SERVER_H
