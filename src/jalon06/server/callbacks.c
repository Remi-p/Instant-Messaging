// ================ Programmation réseau: Jalon 06 ================== *\
//	Bonnotte Benjamin						       		     . \|/ /,  /
//	Perrot Remi			    ===============				     \\`''/_-- /
//	RE216				    |   Fichier   |			  Bordeaux INP --- /
//	Octobre 2015		    |  callbacks.c|			  ENSEIRB  ,..\\`  /
//						    ===============			  MATMECA / | \\`  /
// ================================================================== */

#include "callbacks.h"
#include "user.h"
#include "salon.h"
#include "../commons/network.h"
#include "../commons/contrib.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// --------------- REGEX -----------------
// 									  /who
regex_t preg_who;
const char *regex_who = "^/who$";
//									 /nick
size_t nb_match_nick = 0;
regmatch_t *pmatch_nick = NULL;
regex_t preg_nick;
// On ne prend pas d'espace dans le pseudo
const char *regex_nick = "^/nick ([-_0-9A-Za-z]+)$";
//									  /all
size_t nb_match_all = 0;
regmatch_t *pmatch_all = NULL;
regex_t preg_all;
const char *regex_all = "^/all (.+)$";
//	   /create, /join, /whois, /salon_quit
// On utilise la même regex pour ces
// commandes.
size_t nb_match_generic = 0;
regmatch_t *pmatch_generic = NULL;
regex_t preg_generic;
const char *regex_generic = "^/(create|join|whois|salon_quit) ([-_0-9A-Za-z]+)$";
//					   /msg, /salon, /send
size_t nb_match_generic_to = 0;
regmatch_t *pmatch_generic_to = NULL;
regex_t preg_generic_to;
const char *regex_generic_to = "^/(msg|salon|send) ([-_0-9A-Za-z]+) (.+)$";

// ---------------------------------------

void init_reg() {
	
// 									  /who
	if (regcomp (&preg_who, regex_who, REG_NOSUB | REG_EXTENDED) != 0)
		error("Erreur sur l'initialisation de l'expression régulière /who ");
	
//									 /nick
	if (regcomp (&preg_nick, regex_nick, REG_EXTENDED) != 0)
		error("Erreur lors de l'initialisation de l'expr° régulière pour /nick ");
		
	nb_match_nick = preg_nick.re_nsub + 1;
	
	pmatch_nick = malloc (sizeof (regmatch_t) * nb_match_nick);
	
//									  /all
	if (regcomp (&preg_all, regex_all, REG_EXTENDED) != 0)
		error("Erreur lors de l'initialisation de l'expr° régulière pour /all ");
	
	nb_match_all = preg_all.re_nsub + 1;
	
	pmatch_all = malloc (sizeof (regmatch_t) * nb_match_all);

//					   /msg, /salon, /send
	if (regcomp (&preg_generic_to, regex_generic_to, REG_EXTENDED) != 0)
		error("Erreur lors de l'initialisation de l'expr° régulière pour /generic_to ");
	
	nb_match_generic_to = preg_generic_to.re_nsub + 3;
	
	pmatch_generic_to = malloc (sizeof (regmatch_t) * nb_match_generic_to);
	
//						/create&join&whois
	if (regcomp (&preg_generic, regex_generic, REG_EXTENDED) != 0)
		error("Erreur lors de l'initialisation de l'expr° régulière pour /create, /join et /whois ");
	
	nb_match_generic = preg_generic.re_nsub + 2;
	
	pmatch_generic = malloc (sizeof (regmatch_t) * nb_match_generic);
}

void free_reg() {
	
	regfree(&preg_who);
	
	regfree(&preg_nick);
	free(pmatch_nick);
	
	regfree(&preg_all);
	free(pmatch_all);
		
	regfree(&preg_generic_to);
	free(pmatch_generic_to);
	
	regfree(&preg_generic);
	free(pmatch_generic);
}

bool serv_cmd(char output[TAILLE_MSG], const puser first, const pfile fichiers, const psalon salons, puser present) {
	
	// Buffer du nickname
	char* nickname = malloc(PSEUDO_LENGTH * sizeof(char));
		
	// Si on recoit "/quit" on ferme la socket client, et on supprime
	// la structure liée
	if (!notquit(output)) {
		close_socket(present->sckt);
		del_user_all_salons(salons, present->sckt);
		remove_user_by_fd(first, present->sckt);
		// TODO : Remove from files
		return true;
	}
	// Sinon, si c'est pour un changement de pseudo
	else if (regexec(&preg_nick, output, nb_match_nick, pmatch_nick, 0) == 0)
		handle_nick(first, present, pmatch_nick, output);
		
	// Si le pseudo n'est pas défini, on force la définition
	else if (strlen(present->pseudo) == 0)
		handle_client_message(present->sckt, string_connect, strlen(string_connect), NOK_PSEUDO);
	
	// /who
	else if (regexec(&preg_who, output, 0, NULL, 0) == 0)
		handle_who(first, present->sckt);
	
	// /whois [pseudo] - /create [channel] - /join [channel]
	else if (regexec(&preg_generic, output, nb_match_generic, pmatch_generic, 0) == 0) {
		
		// On a utilisé la même regex pour les commandes citées plus
		// haut, puisque leur syntaxe est similaire. On les départage
		// avec la première lettre de la commande entrée.
		switch (output[1])
		{
			case 'w':
				handle_whois(first, present->sckt, pmatch_generic, output);
				break;
			case 'c':
				handle_create(first, salons, present->sckt, pmatch_generic, output);
				break;
			case 'j':
				handle_join(first, salons, present->sckt, pmatch_generic, output);
				break;
			case 's':
				handle_quit(first, salons, present->sckt, pmatch_generic, output);
				break;
			default:
				error("Command not found");
				break;
		}
		
	}
		
	// /all [message]
	else if (regexec(&preg_all, output, nb_match_all, pmatch_all, 0) == 0)
		handle_msg_all(first, present, pmatch_all, output);
		
	// /msg [pseudo] [message] - /salon [salon] [message]
	else if (regexec(&preg_generic_to, output, nb_match_generic_to, pmatch_generic_to, 0) == 0) {
		
		switch (output[1])
		{
			case 'm':
				handle_msg_uni(first, present, pmatch_generic_to, output);
				break;
			case 's':
				if(output[2] == 'a') // /salon
					handle_msg_multi(first, present, salons, pmatch_generic_to, output);
				else 				 // /send
					handle_file_ask(first, present, fichiers, pmatch_generic_to, output);
				break;
			default:
				error("Command not found");
				break;
		}
		
	}
	
	else
		handle_client_message(present->sckt, string_cmd_not_found, strlen(string_cmd_not_found), NOK_CMD);
	
	return false;
}

// Prise en charge de la demande d'envoi de fichier
void handle_file_ask(const puser first, const puser present, const pfile fichiers, regmatch_t * pmatch, const char* output) {
	
	char *pseudo = get_string_reg(pmatch[2], output);
	char *path = get_string_reg(pmatch[3], output);
	
	puser destinataire = get_user_by_pseudo(first, pseudo);
	
	if (destinataire == NULL) {
		handle_client_message(present->sckt, string_user_not_found, strlen(string_user_not_found), NOK_USER_NF);
		free(path);
	}
	
	else if (waiting_for_user(fichiers, destinataire->sckt)) {
		handle_client_message(present->sckt, string_transfert_client_doit_acc, strlen(string_transfert_client_doit_acc), NOK_USER_BUSY);
		free(path);
	}
	
	else if(destinataire->sckt == present->sckt)
		handle_client_message(present->sckt, string_useless_send_himself, strlen(string_useless_send_himself), NOK_USER_BUSY);
	
	else if(get_file(fichiers, present, destinataire, path, NULL) != NULL) {
		handle_client_message(present->sckt, string_transfert_en_cours, strlen(string_transfert_en_cours), NOK_USER_BUSY);
		free(path);
	}
	
	else {
		
		char* filename = nom_fichier(path);
	
		// Création de la structure nécessaire
		new_fichier(fichiers, present, destinataire, path);
		
		// Message au destinataire
		char str[strlen(string_ask_file) + PSEUDO_LENGTH + strlen(filename)];
		
		sprintf(str, string_ask_file, present->pseudo, filename);
	
		handle_client_message(destinataire->sckt, str, strlen(str), OK_SERVEUR);
		
		free(filename);
	}
	
	free(pseudo);
	
}
	
// Enregistre le pseudo
void handle_nick(const puser first, const puser present, regmatch_t * pmatch, const char* output) {

	char *nickname = get_string_reg(pmatch[1], output);
	
	// Vérification si l'utilisateur n'existe pas
	puser destinataire = get_user_by_pseudo(first, nickname);
	
	if (destinataire != NULL)
		handle_client_message(present->sckt, string_user_exist, strlen(string_user_exist), NOK_USER_NF);
	else {
		printf("Le client %i devient %s\n", present->sckt, nickname);
		
		set_username(nickname, present);
		
		handle_client_message(present->sckt, string_username, strlen(string_username), OK_SERVEUR);
	}
		
	// Libération du buffer
	free(nickname);
}

// Broadcast à tous les utilisateurs
void handle_msg_all(const puser first, const puser present, regmatch_t * pmatch, const char* output) {
	
	char *msg = get_string_reg(pmatch[1], output);
	
	char str[PSEUDO_LENGTH + strlen(msg) + strlen(format_pseudo_msg)];
	
	sprintf(str, format_pseudo_msg, present->pseudo, msg);
	
	int sckt = present->sckt;
	puser actuel = first; // Initialisation de la liste chaînée
	
	while(actuel) {
		
		if (actuel->sckt != sckt)
			handle_client_message(actuel->sckt, str, strlen(str), OK_ALL);
		
		actuel = actuel->next;
	}
	
	// Libération du buffer
	free(msg);
	
}

//  Unicast à un user
void handle_msg_uni(const puser first, const puser present, regmatch_t * pmatch, const char* output) {
	
	char *pseudo = get_string_reg(pmatch[2], output);
	char *msg = get_string_reg(pmatch[3], output);
	
	puser destinataire = get_user_by_pseudo(first, pseudo);
	
	if (destinataire == NULL)
		handle_client_message(present->sckt, string_user_not_found, strlen(string_user_not_found), NOK_USER_NF);
	
	else {
		
		char str[PSEUDO_LENGTH + strlen(msg) + strlen(format_pseudo_msg)];
		
		sprintf(str, format_pseudo_msg, present->pseudo, msg);
	
		handle_client_message(destinataire->sckt, str, strlen(str), OK_PERSO);
	}
	
	// Libération des buffers
	free(pseudo);
	free(msg);
	
}

// Multicast à un salon
void handle_msg_multi(const puser first, const puser present, const psalon salons, regmatch_t * pmatch, const char* output) {
	
	char *name = get_string_reg(pmatch[2], output);
	char *msg = get_string_reg(pmatch[3], output);
	
	// Le salon n'existe pas
	if (!salon_exist(salons, name))
		handle_client_message(present->sckt, string_salon_nexiste, strlen(string_salon_nexiste), NOK_SALON_NF);
	
	// L'utilisateur n'est pas dans le salon
	else if (!user_in_salon(salons, name, present->sckt))
		handle_client_message(present->sckt, string_not_in_salon, strlen(string_not_in_salon), NOK_NOT_IN);
		
	// Envoi à tous les utilisateurs du salon
	else {
		
		int i;
		psalon found = find_salon_by_name(salons, name);
		
		// Message
		char str[PSEUDO_LENGTH + strlen(msg) + strlen(format_pseudo_msg) + strlen(name)];
		sprintf(str, format_salon_msg, name, present->pseudo, msg);
		
		for (i = 0; i <= found->nb_clients - 1; i++) {
			if (found->clients[i] != present->sckt)
				handle_client_message(found->clients[i], str, strlen(str), OK_SALON);
		}
		
	}
	
	// Libération des buffers
	free(name);
	free(msg);
	
}

// Création, s'il existe, du salon entré
void handle_create(const puser first, const psalon salons, const int client_sckt, regmatch_t * pmatch, const char* output) {
	
	char *name = get_string_reg(pmatch[2], output);
	
	if (salon_exist(salons, name))
		handle_client_message(client_sckt, string_salon_existe, strlen(string_salon_existe), NOK_ANY);
	
	else { // Le salon n'existe pas
		new_salon(salons, name);
		handle_client_message(client_sckt, string_salon_cree, strlen(string_salon_cree), OK_SERVEUR);
		
		printf("Salon %s créé par le client %i\n", name, client_sckt);
	}
	
	// On ne libère pas la mémoire (utilisée dans la structure)
	// free(name);
	
}

// Le client joint un salon
void handle_join(const puser first, const psalon salons, const int client_sckt, regmatch_t * pmatch, const char* output) {
	
	char *name = get_string_reg(pmatch[2], output);
	
	if (!salon_exist(salons, name)) // Le salon n'existe pas
		handle_client_message(client_sckt, string_salon_nexiste, strlen(string_salon_nexiste), NOK_SALON_NF);
	
	else if (user_in_salon(salons, name, client_sckt))
		handle_client_message(client_sckt, string_already_in_salon, strlen(string_already_in_salon), NOK_ANY);
	
	else {
		add_user_salon(salons, name, client_sckt);
		
		printf("Le client %i rejoint le salon %s\n", client_sckt, name);
		
		char str[strlen(string_welcome_salon) + PSEUDO_LENGTH];
		sprintf(str, string_welcome_salon, name);
		
		handle_client_message(client_sckt, str, strlen(str), OK_SERVEUR);
	}
	
	// On ne libère pas la mémoire (utilisée dans la structure)
	// free(name);
	
}

// Le client quitte un salon
void handle_quit(const puser first, const psalon salons, const int client_sckt, regmatch_t * pmatch, const char* output) {
	
	char *name = get_string_reg(pmatch[2], output);
	
	if (!salon_exist(salons, name))
		handle_client_message(client_sckt, string_salon_nexiste, strlen(string_salon_nexiste), NOK_SALON_NF);
	
	else if (!user_in_salon(salons, name, client_sckt))
		handle_client_message(client_sckt, string_not_in_salon, strlen(string_not_in_salon), NOK_NOT_IN);
	
	else {
		del_user_salon(salons, name, client_sckt);
		
		printf("Le client %i est sorti du salon %s.\n", client_sckt, name);
		
		handle_client_message(client_sckt, string_goodbye_salon, strlen(string_goodbye_salon), OK_SERVEUR);
	}
	
	// On ne libère pas la mémoire (utilisée dans la structure)
	// free(name);
	
}

// Envoi, si elle existe, l'information au client
void handle_whois(const puser first, const int client_sckt, regmatch_t * pmatch, const char* output) {
	
	char *nickname = get_string_reg(pmatch[2], output);
	
	// Recherche d'information
	puser match = get_user_by_pseudo(first, nickname);
	
	if (match == NULL)
		handle_client_message(client_sckt, string_user_not_found, strlen(string_user_not_found), NOK_USER_NF);
	else {
		
		// Récupération de l'adresse IP
		char* ip = get_user_ip(match);
		int port = get_user_port(match);
		
		int len = strlen(string_whois);
		int nombre_digits = len_int(port); 
		
		char str[len + PSEUDO_LENGTH + INET_ADDRSTRLEN + nombre_digits];
		
		sprintf(str, string_whois, match->pseudo, ip, port);
		
		handle_client_message(client_sckt, str, strlen(str), OK_SERVEUR);
		
		free(ip);
	}
	
	// Libération du buffer
	free(nickname);
	
}

void handle_who(const puser first, const int client_sckt) {
	
	puser present = first;
	int len;
	char tosend[PSEUDO_LENGTH + 2]; // tab + newline
	
	handle_client_message(client_sckt, string_who, strlen(string_who), OK_SERVEUR);
		
	while (present) {
		len = strlen(present->pseudo);
		
		if (len > 0) {
			strcpy(tosend, "\t");
			strcat(strcat(tosend, present->pseudo), "\n");
			handle_client_message(client_sckt, tosend, len+2, ' ');
		}
		
		present = present->next;
	}
	
}

// ---------------------------------------------------------------- Help
//~ /////////////////////////////////////////////////////////////////////////////////////////
//~ //BORDEAUX INP ENSEIRB-MATMECA
//~ //DEPARTEMENT TELECOM
//~ //RE216 PROGRAMMATION RESEAUX
//~ //{daniel.negru,joachim.bruneau_-_queyreix,nicolas.herbaut}@ipb.fr
//~ ////////////////////////////////////////////////////////////////////////////////////////
//~ 
//~ #include "callbacks.h"
//~ 
//~ 
//~ //
//~ //bool handle_who(const char buffer[256], const int client_socket_fd) {
//~ //
//~ //}
//~ //
//~ //bool handle_msg(const char buffer[256], const int client_socket_fd) {
//~ //
//~ //}
//~ //
//~ //bool handle_whois(const char buffer[256], const int client_socket_fd) {
//~ //
//~ //}
//~ //
//~ //bool handle_nick(const char buffer[256], const int client_socket_fd) {
//~ //
//~ //}
//~ //
//~ //bool handle_join(const char buffer[256], const int client_socket_fd) {
//~ //
//~ //}
//~ //
//~ //bool handle_send(const char buffer[256], const int client_socket_fd) {
//~ //
//~ //}
//~ //
// ---------------------------------------------------------------------
