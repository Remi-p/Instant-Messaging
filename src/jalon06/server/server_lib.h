// ================ Programmation réseau : Jalon 06 ================= *\
//	Bonnotte Benjamin						       		     . \|/ /,  /
//	Perrot Remi			    ===============				     \\`''/_-- /
//	RE216				    |   Fichier   |			  Bordeaux INP --- /
//	Octobre 2015		    | server_lib.h|			  ENSEIRB  ,..\\`  /
//						    ===============			  MATMECA / | \\`  /
// ================================================================== */

#ifndef SERVER_LIB

	#define SERVER_LIB

	#include <netinet/in.h>
	#include <sys/socket.h>
	#include "user.h"
	#include "salon.h"
	#include "../commons/file.h"
	
	extern char* string_username;
	extern char* string_connect;
	extern char* string_user_exist;
	extern char* string_who;
	extern char* string_cmd_not_found;
	extern char* string_whois;
	extern char* string_user_not_found;
	extern char* format_pseudo_msg;
	extern char* string_salon_existe;
	extern char* string_salon_nexiste;
	extern char* string_salon_cree;
	extern char* string_already_in_salon;
	extern char* string_not_in_salon;
	extern char* string_welcome_salon;
	extern char* string_goodbye_salon;
	extern char* string_no_cmd_entered;
	extern char* format_salon_msg;
	extern char* string_useless_send_himself;
	extern char* string_ask_file;
	extern char* string_transfert_en_cours;
	extern char* string_answer_by_yn;
	extern char* string_transfert_client_doit_acc;
	extern char* string_file_refused;
	extern char* string_file_received;

	extern bool verbose;

	// Set non blocking socket
	void setnonblocking(int sock);

	// Refuse une connexion
	void refus(int sock);

	// Actions liées à la réception de données
	bool serv_reception(const puser first, const psalon salons, const pfile fichiers, fd_set * psocks, char* string_connect, const bool verbose, int* nb_user);
	
	// Ajoute les connexions au tableau des descripteurs de fichier
	// Renvoie le plus grand descripteur de fichier
	int add_connex(const puser first, fd_set * psocks, int sckt);

#endif //SERVER_LIB
