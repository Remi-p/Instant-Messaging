// ================ Programmation réseau: Jalon 05 ================== *\
//	Bonnotte Benjamin						       		     . \|/ /,  /
//	Perrot Remi			    ===============				     \\`''/_-- /
//	RE216				    |   Fichier   |			  Bordeaux INP --- /
//	Octobre 2015		    |    user.h   |			  ENSEIRB  ,..\\`  /
//						    ===============			  MATMECA / | \\`  /
// ================================================================== */

#ifndef SALON_H_
	#define SALON_H_
	#define PSEUDO_LENGTH 32

	#include <stddef.h>
	#include <netinet/in.h>
	#include "contrib.h"
	
	struct salon{
		char* name;
		int* clients;
		int nb_clients;
		struct salon* next;
	};

	typedef struct salon* psalon;
	
	// Ajoute un nouveau salon dans la liste
	void new_salon(const psalon first, char* name);
	
	// Ajoute la socket au salon
	void add_user_salon(const psalon first, char* name, int sckt_client);
	
	// Supprime un utilisateur d'un salon - Supprime le salon si besoin
	void del_user_salon(const psalon first, char* name, int sckt_client);

	// Supprimer l'utilisateur de tous les salons existants
	void del_user_all_salons(const psalon first, int sckt_client);

	// Vérifie si l'utilisateur est déjà dans le salon
	bool user_in_salon(const psalon salons, char* name, int client_sckt);
	
	// Retourne le salon, s'il existe, NULL sinon
	psalon find_salon_by_name(const psalon first, char* name);

	// Vérifie si un salon existe
	bool salon_exist(const psalon first, char* name);

#endif /* SALON_H_ */
