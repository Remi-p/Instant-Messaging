// ================ Programmation réseau: Jalon 06 ================== *\
//	Bonnotte Benjamin						       		     . \|/ /,  /
//	Perrot Remi			    ===============				     \\`''/_-- /
//	RE216				    |   Fichier   |			  Bordeaux INP --- /
//	Octobre 2015		    |  callbacks.c|			  ENSEIRB  ,..\\`  /
//						    ===============			  MATMECA / | \\`  /
// ================================================================== */
#ifndef CALLBACKS_H
	#define CALLBACKS_H
	
	#include "server_lib.h"
	#include "../commons/file.h"
	#include <regex.h> // http://nicolasj.developpez.com/articles/regex/
	
	// -------------------------------------------------------- Routeur
	// Exécute les commandes. Renvoie true si quelqu'un quitte
	bool serv_cmd(char output[TAILLE_MSG], const puser first, const pfile fichiers, const psalon salons, puser present);
	
	// --------------------------------------------------- Informations
	// Envoie le résultat de /who à l'utilisateur
	void handle_who(const puser first, const int client_sckt);
	
	// Envoi, si elle existe, l'information au client
	void handle_whois(const puser first, const int client_sckt, regmatch_t * pmatch, const char* output);
	
	// Enregistre le pseudo
	void handle_nick(const puser first, const puser present, regmatch_t * pmatch, const char* output);
	
	// -------------------------------------------------------- Fichier
	// Prise en charge de la demande d'envoi de fichier
	void handle_file_ask(const puser first, const puser present, const pfile fichiers, regmatch_t * pmatch, const char* output);

	// --------------------------------------------------------- Salons
	
	// Création, s'il existe, du salon entré
	void handle_create(const puser first, const psalon salons, const int client_sckt, regmatch_t * pmatch, const char* output);

	// Le client joint un salon
	void handle_join(const puser first, const psalon salons, const int client_sckt, regmatch_t * pmatch, const char* output);

	// Le client quitte un salon
	void handle_quit(const puser first, const psalon salons, const int client_sckt, regmatch_t * pmatch, const char* output);

	// Multicast à un salon
	void handle_msg_multi(const puser first, const puser present, const psalon salons, regmatch_t * pmatch, const char* output);
	
	// -------------------------------------------------------- Message
	//  Unicast à un user
	void handle_msg_uni(const puser first, const puser present, regmatch_t * pmatch, const char* output);
	
	// Broadcast à tous les utilisateurs
	void handle_msg_all(const puser first, const puser present, regmatch_t * pmatch, const char* output);

	// ---------------------------------------------------------- Regex
	// Initialise les expressions régulières
	void init_reg();

	// Libère les structures des regex
	void free_reg();
	
#endif
