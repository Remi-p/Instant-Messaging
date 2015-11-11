// ================ Programmation réseau: Jalon 06 ================== *\
//	Bonnotte Benjamin						       		     . \|/ /,  /
//	Perrot Remi			    ===============				     \\`''/_-- /
//	RE216				    |   Fichier   |			  Bordeaux INP --- /
//	Octobre 2015		    |    user.c   |			  ENSEIRB  ,..\\`  /
//						    ===============			  MATMECA / | \\`  /
// ================================================================== */

#include "user.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

char* get_user_ip(const puser usr) {

	struct sockaddr_in6 *cast = (struct sockaddr_in6*) usr->adresse;
	
	return get_ip_from_sockaddr(cast);
	
}

u_short get_user_port(const puser usr) {
	
	struct sockaddr_in6 *cast = (struct sockaddr_in6*) usr->adresse;
	return cast->sin6_port;
	
}

void set_username(const char* nickname, puser struct_user) {
	
	memset(struct_user->pseudo, 0, sizeof(char)*PSEUDO_LENGTH);
	
	int len = 0;
	int i;
	
	if (strlen(nickname) < PSEUDO_LENGTH)
		len = strlen(nickname);
	else
		len = PSEUDO_LENGTH;
	
	for (i = 0; i < len; i++) {
		struct_user->pseudo[i] = nickname[i];
	}
	
}

// Ajoute un nouveau client dans la liste
void new_client(const puser first, int sckt, struct sockaddr* adr) {
	
	// S'il n'y a pas encore d'utilisateur
	if (first->sckt == 0) {
		first->sckt = sckt;
		first->adresse = adr;
	}
	
	// Sinon
	else {
	
		puser present = first;
		
		// Se déplace jusqu'au dernier utilisateur
		while(present->next) {
			present = present->next;
		}
		
		// On utilise calloc qui remet à 0 les éléments de la zone mém.
		puser futur;
			futur = calloc(sizeof(struct user), 1);
		
		present->next = futur;
		futur->sckt = sckt;
		futur->adresse = adr;
		
	}
	
}

void display_users_v(const puser first) {
	
	puser present = first;
	
	printf("Utilisateurs : %p", present);
	do {
		printf("->%p", present->next);
		present = present->next;
	} while(present);
	
	printf("\n");
}

// Enlève un utilisateur grâce à son descripteur de fichier
void remove_user_by_fd(const puser first, const int fd) {
	
	if (first->sckt == fd) {
		
		// S'il y a un utilisateur après, on le copie sur le first
		if (first->next != NULL) {
			puser next = first->next;
			
			first->sckt = next->sckt;
			strcpy(first->pseudo, next->pseudo);
			first->next = next->next;
			first->adresse = next->adresse;
			
			free(next->adresse);
			free(next);
		}
		else {
			free(first->adresse);
			memset(first, 0, sizeof(struct user));
		}
		
		return;
	}
	
	
	// Dans le cas où le premier élément de la liste n'est pas concerné
	puser present = first;
	puser past = first;
	
	do {
		past = present;
		present = present->next;
		
		if (present->sckt == fd) { // On a trouvé la structure
			
			past->next = present->next;
			printf("Libération de %i (%p)\n", present->sckt, present);
			free(present->adresse);
			free(present);
			return;
		}
		
	} while(present->next);
	
}

puser get_user_by_pseudo(const puser first, char* nickname) {
	
	puser present = first;
	
	while(present) {
		//printf("Similitude : %i\n", strcmp(present->pseudo, nickname));
		
		if (strcmp(present->pseudo, nickname) == 0)
			return present;
		
		present = present->next;
	}
	
	return NULL;
	
}


// Libère toutes les structures
void free_user(const puser first) {
	
	puser present = first;
	puser past = NULL;
	
	while (present) {
		
		if (past != NULL && past != first)
			free(past);
		
		if (present->adresse != NULL)
			free(present->adresse);
			
		past = present;
		present = present->next;
		
	}
	
	if (past != NULL && past != first)
		free(past);
	
}
