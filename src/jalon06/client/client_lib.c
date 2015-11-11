// ================ Programmation réseau : Jalon 06 ================= *\
//	Bonnotte Benjamin						       		     . \|/ /,  /
//	Perrot Remi			    ===============				     \\`''/_-- /
//	RE216				    |   Fichier   |			  Bordeaux INP --- /
//	Octobre 2015		    | client_lib.c|			  ENSEIRB  ,..\\`  /
//						    ===============			  MATMECA / | \\`  /
// ================================================================== */

#include "client_lib.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h> // (malloc)
#include <string.h>

#include <fcntl.h>

#include <sys/sendfile.h>

#include <unistd.h> // (sleep)

#include "../commons/network.h"
#include "../commons/colors.h"

// /send
size_t nb_match_send_to = 0;
regmatch_t *pmatch_send_to = NULL;
regex_t preg_send_to;
const char *regex_send_to = "^/send [-_0-9A-Za-z]+ (.+)\n$";
// Refus/succès de transfert de fichier
size_t nb_match_generic_file = 0;
regmatch_t *pmatch_generic_file = NULL;
regex_t preg_generic_file;
const char *regex_generic_file = "^.+\\: (.*)\\.$";

void init_reg() {
	
	//	/send
	if (regcomp (&preg_send_to, regex_send_to, REG_EXTENDED) != 0)
		error("Erreur lors de l'initialisation de l'expr° régulière pour /send_to ");
	
	nb_match_send_to = preg_send_to.re_nsub + 1;
	
	pmatch_send_to = malloc (sizeof (regmatch_t) * nb_match_send_to);
	
	// Refus/succès de transfert de fichier
	if (regcomp (&preg_generic_file, regex_generic_file, REG_EXTENDED) != 0)
		error("Erreur lors de l'initialisation de l'expr° régulière pour /generic_file ");
	
	nb_match_generic_file = preg_generic_file.re_nsub + 1;
	
	pmatch_generic_file = malloc (sizeof (regmatch_t) * nb_match_generic_file);
	
}

void free_reg() {
	regfree(&preg_send_to);
	free(pmatch_send_to);
	
	regfree(&preg_generic_file);
	free(pmatch_generic_file);
}

// Création d'un client sur une machine cliente (pour le peer-to-peer)
void client_on_client(const pfile fichier, const pfile fichiers, const int socket) {
	
	u_short port_src = fichier->dest->sin6_port;
	
	struct sockaddr_in6* self_adr = get_addr_info(port_src, NULL);
	struct sockaddr_in6* serv_adr = get_addr_info(fichier->src->sin6_port,
												get_ip_from_sockaddr(fichier->src));
	char buffer[TAILLE_MSG];
	int size;
	int nb_recv;
	FILE *received_file;
	char * filename = nom_fichier(fichier->path);
	char * home = getenv("HOME");
	char * format_to_write = "%s/Downloads/%s";
	char * path_to_write = malloc((strlen(home) + strlen(filename) + strlen(format_to_write)) * sizeof(char));
	
	// Chemin exact où l'on enregistrera le fichier
	sprintf(path_to_write, format_to_write, home, filename);
	
	// On fait un bind, pour forcer l'attribution du port
	do_bind(socket, self_adr);
	
	do_connect(socket, *serv_adr);
	
	// Réception de la taille du fichier
	do_read(socket, buffer, TAILLE_MSG, NULL);
    size = atoi(buffer);
    
    if (verbose) printf("Taille du fichier : %i\n", size);
    
    received_file = fopen(path_to_write, "w+");
    
    if (received_file == NULL)
		error("Impossible d'écrire le fichier ");
    
    // On aurait pu utiliser do_read, mais il aurait fallu changer la
    // fonction pour qu'elle renvoie le nombre de bits lus
    while (((nb_recv = recv(socket, buffer, TAILLE_MSG, 0)) > 0) && (size > 0)) {
		fwrite(buffer, sizeof(char), nb_recv, received_file);
		size -= nb_recv;
	}
    
    fclose(received_file);
	
	// Fermeture
	close_socket(socket);
	
	// Suppression du fichier
	file_finished_sckt(fichiers, socket);
	
}
	
// Création d'un serveur sur une machine cliente (pour le peer-to-peer)
void server_on_client(const int sckt, const pfile fichier, const pfile fichiers) {
	
	// La boucle liée au serveur se fera ici => pthread
	
	// --------------------------------- cf server_app.c pour le détail
	// Variables
	u_short port = fichier->src->sin6_port;
	struct sockaddr_in6* adr = get_addr_info(port, NULL);
	int j;
	struct sockaddr* adr_tmp = malloc(sizeof(struct sockaddr));
	
	int fd; // Numéro de descripteur pour le fichier à envoyer
	off_t size;
	char size_char[TAILLE_MSG]; // File_size envoyée au client-client
	off_t offset;
	int nb_sent;
	
	do_bind(sckt, adr);
	
	listen_of_ambiance(sckt);
	
    do { // Boucle principale

		j = do_accept(sckt, adr_tmp);
		
		// Maj de la socket correspondant à ce fichier
		socket_maj(fichiers, (struct sockaddr_in6*) adr_tmp, j);
		
		// 											   Envoi du fichier
		// http://stackoverflow.com/questions/11952898/c-send-and-receive-file
		fd = open(fichier->path, O_RDONLY);
		
		if (fd == -1)
			error("Echec à l'ouverture du fichier ");
		
		size = file_size(fd);
		
		sprintf(size_char, "%d", size);
		
		// Envoi de la taille
		handle_client_message(j, size_char, TAILLE_MSG, OK_LEN);
		// On envoie une taille de taille_msg, sinon le fichier se
		// retrouvera à la suite de la taille ...
		
		// Envoi des données
		offset = 0;
        /* Sending file data */
        while (((nb_sent = sendfile(j, fd, &offset, TAILLE_MSG)) > 0)
		&& (size > 0)) {
			size -= nb_sent;
        }
		
		// Suppression du fichier
		file_finished_sckt(fichiers, j);
			
		// Il n'y a pas de réception de données :)
		close_socket(j);
		
    } while( still_not_finish(fichiers) );
    
    close_socket(sckt);
}

// Actions à effectuer lors de la réception de données clavier
void client_write(char** input, const int sckt, const pfile fichiers) {
	
	// On récupère ce que l'utilisateur a écrit
	ssize_t nb_char_in = readline(input);
	
	if (*input[0] == '/') { // On a à faire à une commande
		
		// Le client a entré une fonction correct de send to
		if (regexec(&preg_send_to, *input, nb_match_send_to, pmatch_send_to, 0) == 0) {
			// On crée une structure de fichier pour l'envoi
			
			char* path = get_string_reg(pmatch_send_to[1], *input);
			
			// Vérification de l'existence du fichier + droits d'accès
			if( access( path, R_OK ) == -1 || is_regular_file(path) == 0 ) {
				printf("Le fichier n'a pas pu être trouvé !\n");
				start_line();
				return;
			}
			
			new_fichier(fichiers, NULL, NULL, path);
			
		}
	}

	// Permet d'enlever le saut de ligne enregistré par fgets
	strtok(*input, "\n");

	// On envoie le message au serveur
	handle_client_message(sckt, *input, strlen(*input), MSG_CLIENT);

	// Réécriture du ">"
	start_line();
}

// Actions à effectuer lors de la réception de données serveurs
bool client_read(const int sckt, char* output, const pfile fichiers, int* socket_fichier) {
	
	enum code code_ret; // Code retour de do_read
	
	// On vérifie s'il n'y a pas d'erreur sur la socket
	if (socket_error(sckt))
		return false;

	// On récupère les informations du serveur
	if (do_read(sckt, output, TAILLE_MSG, &code_ret) == false) {
		
		//printf("La socket a été fermée\n");
		// La socket a été fermée
		toquit(output);
	}
	else {
		
		// Affiche en rouge si le code de retour est
		// un code d'erreur
		if ((code_ret >> 6) == REPONSE_NOK)
			color(ANSI_COLOR_RED);
			
		else if (code_ret == OK_FILE_REFUSED) { // Un fichier a été refusé
			
			if (regexec(&preg_generic_file, output, nb_match_generic_file, pmatch_generic_file, 0) == 0) {
				
				char* path = get_string_reg(pmatch_generic_file[1], output);
				
				// On enlève la structure de fichier qui a ce chemin,
				// et dont la destination n'est pas défini
				
				file_refused(fichiers, path);
				free(path);
			}
			else
				error("Erreur de regex (client_lib.c:client_read()) ");
			
		}
		else if (code_ret == OK_STRUCT) { // Réception d'une structure
			
			// Enregistrement des données utiles : adresse + port
			pfile fichier = unserialise(output);
			
			// Lancement d'une socket dédiée
			if (*socket_fichier == -1)
				*socket_fichier = do_socket();
			
			if (remplace_fichier(fichiers, fichier)) { // On est dans le client-serveur
				
				server_on_client(*socket_fichier, fichier, fichiers);
				
				*socket_fichier = 0;
			}
			else { // On est dans le client-client
	
				client_on_client(fichier, fichiers, *socket_fichier);
				
				// Confirmation au serveur
				handle_client_message(sckt, output, strlen(output), OK_RECV);
				
				*socket_fichier = 0;
			}
			
			free(fichier->path);
			free(fichier);			
		}
		
		if (code_ret != OK_STRUCT)
		//printf("[Serveur] : %s\n", output);
			printf("%s%s\n", output, ANSI_COLOR_RESET);
	
		start_line();
	}
	
	return true;
}

// Connection
int do_connect(int socket, struct sockaddr_in6 serv_add) {
	
	int conn;
	
	// ------- Reste à faire : Gestion des adresse IPv4 mappées en IPv6
	//~ if (serv_add.sin6_family == AF_INET) { // IPv4
		//~ struct sockaddr_in* serv_ipv4 = convert_ipv6(serv_add);
		//~ 
		//~ conn = connect(socket, (struct sockaddr *) serv_ipv4, sizeof(struct sockaddr_in));
		//~ 
		//~ free(serv_ipv4);
	//~ }
	//~ else // IPv6
	// ----------------------------------------------------------------
		conn = connect(socket, (struct sockaddr *) &serv_add, sizeof(struct sockaddr_in6));
	
	if (conn == -1) {
		error("Voici l'erreur concernant la connexion ");
	}
	
	if (verbose) printf("Connexion réussie (%i).\n\n", conn);
	
	return conn;
}
