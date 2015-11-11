// ================ Programmation réseau: Jalon 05 ================== *\
//	Bonnotte Benjamin						       		     . \|/ /,  /
//	Perrot Remi			    ===============				     \\`''/_-- /
//	RE216				    |   Fichier   |			  Bordeaux INP --- /
//	Octobre 2015		    |    user.h   |			  ENSEIRB  ,..\\`  /
//						    ===============			  MATMECA / | \\`  /
// ================================================================== */

#ifndef USER_H_
	#define USER_H_
	#define PSEUDO_LENGTH 32

	#include <stddef.h>
	#include <netinet/in.h>
	#include "contrib.h"

	// Structure pour un utilisateur. On fait une liste chaînée : d'où
	// le pointeur vers la structure suivante
	struct user{
		 int sckt;
		 char pseudo[PSEUDO_LENGTH];
		 struct sockaddr* adresse;
		 struct user* next;
	};

	typedef struct user* puser;
	
	// Enregistre
	void set_username(const char* nickname, puser struct_user);
	
	// Renvoie true s'il y a une commande
	bool who(char* input);
	
	// Renvoi l'IP du client, en chaîne de caractère
	char* get_user_ip(const puser usr);
	
	// Renvoi le port du client
	u_short get_user_port(const puser usr);
	
	// Ajoute un nouveau client dans la liste
	void new_client(const puser first, int sckt, struct sockaddr* adr);
	
	// Enlève un utilisateur grâce à son descripteur de fichier
	void remove_user_by_fd(const puser first, const int fd);
	
	// Affiche les adresses des utilisateurs
	void display_users_v(const puser first);
	
	// Retourne un utilisateur en fonction de son pseudo
	puser get_user_by_pseudo(const puser first, char* nickname);
	
	// Libère toutes les structures
	void free_user(const puser first);

#endif /* USER_H_ */
