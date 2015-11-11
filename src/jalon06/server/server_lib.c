// ================ Programmation réseau : Jalon 06 ================= *\
//	Bonnotte Benjamin						       		     . \|/ /,  /
//	Perrot Remi			    ===============				     \\`''/_-- /
//	RE216				    |   Fichier   |			  Bordeaux INP --- /
//	Octobre 2015		    | server_lib.c|			  ENSEIRB  ,..\\`  /
//						    ===============			  MATMECA / | \\`  /
// ================================================================== */

#include "server_lib.h"

#include "../commons/contrib.h"
#include "../commons/network.h"
#include "user.h"
#include "salon.h"
#include "callbacks.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <fcntl.h> //fcntl

char* string_username = "Quel drôle de nom, pourquoi pas Libellule ou Papillon ?\n";
char* string_connect = "Please use /nick [Username] to choose your username";
char* string_user_exist = "Ce pseudo est déjà utilisé.";
char* string_who = "Utilisateur(s) connecté(s) :\n";
char* string_cmd_not_found = "Commande incorrecte";
char* string_whois = "L'utilisateur %s possède l'adresse IP %s et utilise le port %i.";
char* string_user_not_found = "L'utilisateur n'a pas été trouvé.";
char* string_salon_existe = "Ce salon existe déjà !";
char* string_salon_nexiste = "Ce salon n'existe pas !";
char* string_salon_cree = "Salon créé. Tapez /join [name] pour le rejoindre.";
char* string_already_in_salon = "Vous êtes déjà dans ce salon.";
char* string_not_in_salon = "Vous n'êtes pas dans ce salon.";
char* string_welcome_salon = "Bienvenue dans le salon %s";
char* string_goodbye_salon = "A bientôt !";
char* string_no_cmd_entered = "Vous n'avez pas entré de commande.";
char* string_useless_send_himself = "Inutile de vous envoyer un message à vous-même, voyons chef !";
char* string_ask_file = "Le client %s souhaite vous envoyer le fichier %s. Acceptez-vous ? [o/n]";
char* string_transfert_en_cours = "Le transfert de ce fichier est déjà en cours vers ce client.";
char* string_answer_by_yn = "Merci de répondre par \"o\" ou \"n\".";
char* string_transfert_client_doit_acc = "Cet utilisateur doit accepter un autre transfert avant de pouvoir recevoir le vôtre.";
char* string_file_refused = "Le client %s a refusé le transfert de fichier : %s.";
char* string_file_received = "Le fichier %s a bien été reçu.";

char* format_pseudo_msg = "%s: %s";
char* format_salon_msg = "%s> %s: %s";

// Passage d'une socket en mode non bloquant (from Internet)
void setnonblocking(int sock) {
	
	int opts;

	opts = fcntl(sock,F_GETFL);
	if (opts < 0) {
		perror("fcntl(F_GETFL)");
		exit(EXIT_FAILURE);
	}
	opts = (opts | O_NONBLOCK);
	if (fcntl(sock,F_SETFL,opts) < 0) {
		perror("fcntl(F_SETFL)");
		exit(EXIT_FAILURE);
	}
	return;
}

void refus(int sckt) {
	
	char* toomanyco = "Server cannot accept incoming connections anymore. Try again later.";
	int tmp;
	
	tmp = do_accept(sckt, NULL);
							
	handle_client_message(tmp, toomanyco, strlen(toomanyco), NOK_CONN);
	
	close_socket(tmp);
}

// Actions liées à la réception de données
bool serv_reception(const puser first, const psalon salons, const pfile fichiers, fd_set * psocks, char* string_connect, const bool verbose, int* nb_user) {
	
	bool sbdy_quit = false;
	// Buffer pour la lecture des messages envoyés par les clients
	char output[TAILLE_MSG];
	pfile fichier_to_accept;
	pfile waiting_user; // Fichier en attente
	enum code code_ret; // Code de retour
	
	puser present = first;
	
	// On parcourt les connexions ouvertes
	while(present) {
		
		if (present->sckt == 0)
			error("serv_reception => la socket de la structure user est égale à zero!\n");
		
		if (FD_ISSET(present->sckt, psocks)) {
			// On réceptionne ce que le client envoie.
			
			if (verbose)
				printf("Réception de %p (sckt %i)\n", present, present->sckt);
			
			if (do_read(present->sckt, output, TAILLE_MSG, &code_ret) == false) {
			// La socket a été fermée de l'autre côté
				close_socket(present->sckt);
				remove_user_by_fd(first, present->sckt);
				
				*nb_user = *nb_user - 1;
				sbdy_quit = true;
				
			} else {
				
				if (output[0] == '/') { // Réception d'une commande
					
					sbdy_quit = serv_cmd(output, first, fichiers, salons, present);
					
					if (sbdy_quit)
						*nb_user = *nb_user - 1;
				}
					
				else if (strlen(present->pseudo) == 0)
					handle_client_message(present->sckt, string_connect, strlen(string_connect), NOK_PSEUDO);
				
				// Cas de la réception de fichier
				else if (waiting_user = waiting_for_user(fichiers, present->sckt)) {
					
					if (strlen(output) == 1 && output[0] == 'o') {
						
						char* tosend = file_accepted(fichiers, present->sckt, &fichier_to_accept);
						
						// Envoie des structures (avec les ports) aux clients
						handle_client_message(fichier_to_accept->src_sckt, tosend, strlen(tosend), OK_STRUCT);
						handle_client_message(fichier_to_accept->dest_sckt, tosend, strlen(tosend), OK_STRUCT);
					}
					
					// Refus de la part du client
					else if (strlen(output) == 1 && output[0] == 'n') {
						
						int socket_src = waiting_user->src_sckt;
						
						char* path = file_refused_path(fichiers, present->sckt);
						
						char str[strlen(present->pseudo) + strlen(string_file_refused) + strlen(path)];
						
						sprintf(str, string_file_refused, present->pseudo, path);
						
						handle_client_message(socket_src, str, strlen(str), OK_FILE_REFUSED);
						
						free(path);
					}
						
					else
						handle_client_message(present->sckt, string_answer_by_yn, strlen(string_answer_by_yn), NOK_CMD);
				}
				
				// Un transfert de fichier est terminé
				else if (code_ret == OK_RECV) {
					
					pfile todelete = get_and_delete(fichiers, output);
					if (todelete == NULL)
						error("Erreur de confirmation d'envoi ");
					
					char* filename = nom_fichier(todelete->path);
				
					// Message de validation
					char str[strlen(string_file_received) + strlen(filename)];
					sprintf(str, string_file_received, filename);
					
					free(filename);
				
					handle_client_message(todelete->src_sckt, str, strlen(str), OK_SERVEUR);
					handle_client_message(todelete->dest_sckt, str, strlen(str), OK_SERVEUR);
					
					free(todelete->path);
					free(todelete);
				}
					
				// Sinon fonctionnement normal
				else
					handle_client_message(present->sckt, string_no_cmd_entered, strlen(string_no_cmd_entered), NOK_CMD);
				
			} // Fin de la condition sur la vérification de connexion
			
		} // Fin de la réception
		
		present = present->next;
		
	}
	
	return sbdy_quit;
}

// Ajoute les connexions au tableau des descripteurs de fichier
int add_connex(const puser first, fd_set * psocks, const int sckt) {
	
	// Ce cas de figure se présente quand tous les clients se sont
	// déconnectés
	if (first->sckt == 0) {
		return sckt;
	}
	
	puser present = first;
	int plus_grde_sckt = sckt;
	
	while (present) {
		
		// Si nous sommes là, c'est que le client est connecté, mais
		// dans le doute :
		if (present->sckt == 0)
			error("add_connex => la socket de la structure user est égale à zero!\n");
		
		// Ajout au set
		FD_SET(present->sckt,psocks);
		
		// Mise à jour du plus grand descripteur
		if (present->sckt > plus_grde_sckt)
			plus_grde_sckt = present->sckt;
		
		present = present->next;
	}
	
	return plus_grde_sckt;
}
