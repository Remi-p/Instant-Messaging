// ================ Programmation réseau: Jalon 05 ================== *\
//	Bonnotte Benjamin						       		     . \|/ /,  /
//	Perrot Remi			    ===============				     \\`''/_-- /
//	RE216				    |   Fichier   |			  Bordeaux INP --- /
//	Octobre 2015		    |   salon.c   |			  ENSEIRB  ,..\\`  /
//						    ===============			  MATMECA / | \\`  /
// ================================================================== */

#include "salon.h"
#include "user.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

// Ajoute un nouveau salon dans la liste
void new_salon(const psalon first, char* name) {
	
	// S'il n'y a pas encore de salon
	if (first->name == NULL) {
		first->name = name;
		first->nb_clients = 0;
	}
	
	// Sinon
	else {
	
		psalon present = first;
		
		// Se déplace jusqu'au dernier utilisateur
		while(present->next) {
			present = present->next;
		}
		
		// On utilise calloc qui remet à 0 les éléments de la zone mém.
		psalon futur;
			futur = calloc(sizeof(struct salon), 1);
		
		present->next = futur;
		futur->name = name;
		futur->nb_clients = 0;
		
	}
	
}

// Ajoute un nouveau salon dans la liste
void del_salon(const psalon first, psalon search) {
	
	if (first == search) { // On veut delete le premier salon				
		
		if (verbose) printf("Le salon %s a été automatiquement fermé.\n", search->name);
			
		free(search->clients);
		free(search->name);
		search->clients = NULL;
		search->name = NULL;
		memset(search, 0, sizeof(struct salon));
		
		return;
	}
	
	psalon present = first;
			
	while (present) {
		if (present->next == search) {
			present->next = search->next;
			
			if (verbose) printf("Le salon %s a été automatiquement fermé.\n", search->name);
						
			free(search->clients);
			free(search->name);
			free(search);
			
			return;
		}
		
		present = present->next;
	}
	
}

// Affiche les utilisateurs connectés à un salon (debug)
void display_users_salon_v(const psalon present) {
	
	int i;
	
	fprintf(stdout, "Nombre de clients : %i\n", present->nb_clients);
	
	for (i = 0; i <= present->nb_clients - 1; i++) {
		fprintf(stdout, "Present->clients[%i] = %i\n", i, present->clients[i]);
	}
	
	fflush(stdout); // Affiche tout ce qui est dans le buffer
		
}

// Retourne le salon, s'il existe, NULL sinon
psalon find_salon_by_name(const psalon first, char* name) {
	
	psalon present = first;
	
	while (present && present->name != NULL) {
		if (strcmp(present->name, name) == 0)
			return present;
		
		present = present->next;
	}
	
	return NULL;
	
}

// Supprime un utilisateur d'un salon - Supprime le salon si besoin
void del_user_salon(const psalon first, char* name, int sckt_client) {
	
	psalon present = find_salon_by_name(first, name);
	int i;
	int N = present->nb_clients - 1;
	
	for( i = 0 ; i <= N ; i++ ) {
		
        if ( present->clients[i] == sckt_client ) {
			
            while ( i < N ) {
                present->clients[i] = present->clients[i+1];
                i++;   
            }
            break;
        }
        
    }
    
    present->clients[N] = 0;
    
    present->nb_clients = present->nb_clients - 1;
    
    if (present->nb_clients < 1)
		del_salon(first, present);
    else
		present->clients = realloc(present->clients, sizeof(int) * present->nb_clients);
	
}

// Supprimer l'utilisateur de tous les salons existants
void del_user_all_salons(const psalon first, int sckt_client) {
	
	psalon present = first;
	
	while (present && present->name != NULL) {
		if (user_in_salon(first, present->name, sckt_client))
			del_user_salon(first, present->name, sckt_client); // Pas super optimisé ?
		
		present = present->next;
	}
	
}

// Vérifie si l'utilisateur est déjà dans le salon
bool user_in_salon(const psalon first, char* name, int client_sckt) {
	
	psalon present = find_salon_by_name(first, name);
	int i;
	
	for (i = 0; i <= present->nb_clients - 1; i++) {
		
		if (present->clients[i] == client_sckt)
			return true;
	}
	
	return false;
	
}

// Ajoute la socket au salon
void add_user_salon(const psalon first, char* name, int sckt_client) {
	
	psalon present = find_salon_by_name(first, name);
	
	// Aucun client n'était encore connecté
	if(present->clients == NULL) {
		present->clients = malloc(sizeof(int));
		present->clients[0] = sckt_client;
		present->nb_clients = 1;
	}
	// Il y avait déjà des clients
	else {
		present->nb_clients = present->nb_clients + 1;
		present->clients = realloc(present->clients, sizeof(int)*(present->nb_clients));
		present->clients[present->nb_clients - 1] = sckt_client;
	}
	
}

// Vérifie si un salon existe
bool salon_exist(const psalon first, char* name) {
	
	if (find_salon_by_name(first, name) != NULL)
		return true;
	else
		return false;
	
}
