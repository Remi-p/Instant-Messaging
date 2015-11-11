// ================ Programmation réseau : Jalon 06 ================= *\
//	Bonnotte Benjamin						       		     . \|/ /,  /
//	Perrot Remi			    ===============				     \\`''/_-- /
//	RE216				    |   Fichier   |			  Bordeaux INP --- /
//	Octobre 2015		    |    file.c   |			  ENSEIRB  ,..\\`  /
//						    ===============			  MATMECA / | \\`  /
// ================================================================== */

#include "file.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <arpa/inet.h>

#include "../commons/network.h"

// Pour IPv6, on ne peut pas séparer avec des ":" ...
char* format_serialise = "%s<%s<%i<%i<%s";
char* format_unserialise = "%[0-9:.]<%[0-9:.]<%i<%i<%ms";

// Maj du numéro de socket correspondant au fichier
void socket_maj(const pfile first, const struct sockaddr_in6* adr_tmp, const int sckt) {
	
	u_int16_t port = adr_tmp->sin6_port;
	unsigned char* addr = (char*) adr_tmp->sin6_addr.s6_addr;
	
	pfile present = first;
	
	while (present) {
		if (present->path != NULL
		&& present->accepted == true
		&& present->finished == false) {
			if ( present->dest->sin6_port == port
			&& strcmp(present->dest->sin6_addr.s6_addr, addr) ) {
			// En IPv6, on ne compare plus simplement les entiers, il
			// faut utiliser strcmp puisque l'adresse est sur 16 chars
				present->dest_sckt = sckt;
			}
		}
		
		present = present->next;
	}
}

// Retourne "true" quand des fichiers sont acceptés et pas encore
// terminés
bool still_not_finish(const pfile first) {
	
	pfile present = first;
	
	while (present) {
		if (present->accepted == true && present->finished == false)
			return true;
		
		present = present->next;
	}
	
	return false;
	
}

// Sérialisation de la structure suivant le format_serialise
char* serialise(const pfile present) {
	
	// Cast
	struct sockaddr_in6 *cast_dest = (struct sockaddr_in6*) present->dest;
	struct sockaddr_in6 *cast_src  = (struct sockaddr_in6*) present->src;
	
	// Le port d'écoute du serveur-client sera son port de connexion
	// + (ou -) son numéro de descripteur de fichier.
	u_short port_src = cast_src->sin6_port;
	if (port_src + present->src_sckt > 65565) port_src -= present->src_sckt;
	else 									  port_src += present->src_sckt;
		
	u_short port_dest = cast_dest->sin6_port;
	if (port_dest + present->src_sckt > 65565) port_dest -= present->dest_sckt;
	else 									   port_dest += present->dest_sckt;
	
	char* ip_src = get_ip_from_sockaddr(cast_src);
	char* ip_dest = get_ip_from_sockaddr(cast_dest);
	
	u_short sizeport_src = len_int(port_src);
	u_short sizeport_dest = len_int(port_dest);
	
	// Buffer
	char* serialized = malloc( (strlen(format_serialise) + strlen(ip_src) + strlen(ip_dest) + sizeport_src + sizeport_dest + strlen(present->path))*sizeof(char) );
	
	sprintf(serialized, format_serialise, ip_src, ip_dest, port_src, port_dest, present->path);
	
	free(ip_src);
	free(ip_dest);
	
	return serialized;
	
}

// Renvoi la structure depuis la chaîne sérialisée
pfile unserialise(const char* output) {
	
	pfile fichier = malloc(sizeof(struct file));
	
	// On utilise l'inverse de sprintf : sscanf
	char ip_src[INET_ADDRSTRLEN], ip_dest[INET_ADDRSTRLEN];
	char* path;
	int port_src, port_dest;
	
	sscanf(output, format_unserialise, ip_src, ip_dest, &port_src, &port_dest, &path);
	
	struct sockaddr_in6* dest_in = get_addr_info(port_dest, ip_dest);
	struct sockaddr_in6* src_in = get_addr_info(port_src, ip_src);
	
	// On garde les ports du serveur; servent d'identifiants
	fichier->dest = dest_in;
	fichier->src = src_in;
	
	fichier->dest_sckt = -1; // Les sockets seront appliquées plus tard
	fichier->src_sckt = -1;
	
	fichier->path = path;
	
	return fichier;
	
}

// Display information on file structure
void file_v(const pfile present) {
	
	printf("Adresse : %p\n", present);
	printf("Path : %s\n", present->path);
	printf("Port src : %i / Socket src : %i\n", present->src->sin6_port, present->src_sckt);
	printf("Port dest : %i / Socket dest : %i\n", present->dest->sin6_port, present->dest_sckt);
	
}

// Renvoi le fichier qui a été supprimé
pfile get_and_delete(const pfile first, const char* seria) {
	
	pfile todelete = unserialise(seria);
	
	pfile present = first;
	pfile past = NULL;
	char *seria_tmp;
	
	while (present) {
		
		// On compare simplement les serialisations des fichiers
		seria_tmp = serialise(present);
		if (strcmp(seria, seria_tmp) == 0) {
			
			free(seria_tmp);
			
			todelete->src_sckt = present->src_sckt;
			todelete->dest_sckt = present->dest_sckt;
			
			delete_file(past, present);
			return todelete;
		}
		
		free(seria_tmp);
		past = present;
		present = present->next;
		
	}
	
	return NULL;
	
}

// Mets à jour les attributs d'un fichier (true si trouvé, false sinon)
bool remplace_fichier(const pfile fichiers, const pfile present) {
	
	// On sélectionne uniquement via le chemin
	pfile found = get_file(fichiers, NULL, NULL, present->path, NULL);
	
	if (found == NULL)
		return false;
	else {
		found->dest = present->dest;
		found->src = present->src;
		found->src_sckt = present->src_sckt; // ( )
		found->dest_sckt = present->dest_sckt;
		
		return true;
	}
	
}

// Renvoie les informations nécessaires depuis une structure
char* todata(const pfile present, int *length) {
	
	// ------------------- NE FONCTIONNE PAS  -------------------------
	/*    / \   On utilisera comme situation de
	 *   / ! \  rechange une serialisation/deserialisation,
	 *  /_____\ même si c'est moins rigolo ...
	 */
	
	// -------------------- Structure d'envoi -------------------------
	// unsigned long ip_src
	// unsigned long ip_dest
	// int port : port à écouter/utilisé
	// int length_path : longueur du chemin pour le fichier
	// char* path
	// ----------------------------------------------------------------
	
	// Cast
	struct sockaddr_in6 *cast_dest = (struct sockaddr_in6*) present->dest;
	struct sockaddr_in6 *cast_src  = (struct sockaddr_in6*) present->src;
	
	// Le port d'écoute du serveur-client sera son port de connexion
	// + (ou -) son numéro de descripteur de fichier.
	u_short port = cast_src->sin6_port;
	if (port + present->src_sckt > 65565)
		port -= present->src_sckt;
	else
		port += present->src_sckt;
	
	int sizeaddr = sizeof(u_long);
	int sizeport = sizeof(u_short);
	int sizepath = strlen(present->path) * sizeof(char);
	
	*length = 2*sizeaddr + sizeport + sizepath;
	
	char* buffer = malloc( *length );
	
	memcpy(buffer, 										&cast_src->sin6_addr.s6_addr, sizeaddr);
	memcpy(buffer + sizeaddr, 							&cast_dest->sin6_addr.s6_addr, sizeaddr);
	memcpy(buffer + sizeaddr*2, 						&port, sizeport);
	memcpy(buffer + sizeaddr*2 + sizeport, 				&sizepath, sizeof(int));
	memcpy(buffer + sizeaddr*2 + sizeport + sizeof(int), present->path, sizepath);
	
	printf("La longueur du chemin devrait être de %i.\n", sizepath);
	
	return buffer;
	
}

// Renvoi les caractères en tant que structure
pfile tofile(char* output) {
	
	// /!\ NE FONCTIONNE PAS (cf todata())
	
	//pfile fichier = malloc(sizeof(struct file));
	
	int sizeaddr = sizeof(u_long);
	int sizeport = sizeof(u_short);
	
	// Structure d'adresses
	struct sockaddr_in6 src, dest;
	memset(&src, 0, sizeof(struct sockaddr_in6));
	memset(&dest, 0, sizeof(struct sockaddr_in6));
	
	src.sin6_family=AF_INET6;
	memcpy(&(src.sin6_port), output + 2*sizeaddr, sizeport);
	memcpy(&(src.sin6_addr.s6_addr), output, sizeaddr);
	
	dest.sin6_family=AF_INET6;
	memcpy(&(dest.sin6_port), output + 2*sizeaddr, sizeport);
	memcpy(&(dest.sin6_addr.s6_addr), output + sizeaddr, sizeaddr);
	
	int sizepath = 0;
	memcpy(&sizepath, output + sizeaddr*2 + sizeport, sizeof(int));
	
	printf("La longueur du chemin fait %i.\n", sizepath);
	printf("Port : %i / Addr : %s\n", dest.sin6_port, get_ip_from_sockaddr(&dest));
		
	return NULL;
	
}

// Attribue les valeurs nécessaires à la structure
void attr_fichier(const pfile present, const puser src, const puser dest, char* path) {
	
	// On effectue un cast, car le pointeur de path ne doit pas
	// spécialement avoir le mot clé 'const'
	present->path = path;
	
	if (src != NULL && dest != NULL) {
		present->src = (struct sockaddr_in6 *) src->adresse;
		present->src_sckt = src->sckt;
		present->dest = (struct sockaddr_in6 *) dest->adresse;
		present->dest_sckt = dest->sckt;
	}
	else { // Probablement créé depuis le client : socket inutile
		present->src = NULL;
		present->dest = NULL;
		present->src_sckt = -1;
		present->dest_sckt = -1;
	}
	
	present->accepted = false;
	present->finished = false;
	present->next = NULL;
}

// Compare les attributs à une structure
bool file_equal(const pfile present, const puser src, const puser dest, const char* path) {
	
	if (src != NULL && present->src_sckt != src->sckt)
		return false;
	
	if (dest != NULL && present->dest_sckt != dest->sckt)
		return false;
	
	//printf("Comparing %s and %s\n", present->path, path);
		
	if (path != NULL && present->path != NULL) {
		if (strcmp(present->path, path) != 0)
			return false;
	} else if (present->path == NULL && src == NULL && dest == NULL)
		return false;
	
	// Sinon
	return true;
	
}

// Récupère une structure de fichier
pfile get_file(const pfile first, const puser src, const puser dest, const char* path, pfile *past) {
	
	pfile present = first;
	if (past != NULL)
		*past = NULL;
	
	while (present) {
		
		// Structure de fichier trouvée
		if (file_equal(present, src, dest, path)) {
			return present;
		}
		
		// Si on veut récuperer la structure précédente
		if(past != NULL)
			*past = present;
		
		present = present->next;
		
	}
	
	return NULL;
	
}

// Ajoute un nouveau fichier dans la liste
void new_fichier(const pfile first, const puser src, const puser dest, char* path) {
	
	// S'il n'y a pas de fichier en cours de gestion
	if (first->dest_sckt == 0)
		attr_fichier(first, src, dest, path);
	
	// Sinon
	else {
	
		pfile present = first;
		
		// Se déplace jusqu'au dernier utilisateur
		while(present->next) {
			present = present->next;
		}
		
		// On utilise malloc pour attribuer une zone mémoire
		pfile futur;
			futur = calloc(sizeof(struct user), 1);
		
		attr_fichier(futur, src, dest, path);
		present->next = futur;
		
	}
	
}

// Renvoi la structure de fichier pour laquelle l'utilisateur doit répondre
pfile waiting_for_user(const pfile first, const int sckt) {
	
	pfile present = first;
	
	while (present) {
		if (present->dest_sckt == sckt && present->accepted == false)
			return present;
		
		present = present->next;
	}
	
	return NULL;
	
}

// Assigne 'true' à accepted de la structure
// Renvoi la structure sérialisée
char* file_accepted(const pfile first, const int sckt, pfile* fichier) {
	
	pfile present = first;
	
	while (present) {
		if (present->dest_sckt == sckt && present->accepted == false) {
			
			if (verbose) printf("Fichier %s passé à accepted = true\n", present->path);
			
			present->accepted = true;
			*fichier = present;
			
			return serialise(present);
		}
		
		present = present->next;
	}
	
	return NULL;
	
}

// Supprime la structure avec accepted à false et ->path = path
void file_refused(const pfile first, const char* path) {
	
	pfile present = first;
	pfile past = NULL;
	
	while (present) {
		
		if ( present->accepted == false && present->path != NULL && strcmp(present->path, path) == 0 ) {
			
			delete_file(past, present);
			
			return;
		}
		
		past = present;
		present = present->next;
	}
	
}

// Supprime la structure avec accepted à false et dest_sckt = sckt,
// et renvoie le path
char* file_refused_path(const pfile first, const int sckt) {
	
	pfile present = first;
	pfile past = NULL;
	
	while (present) {
		
		if (present->dest_sckt == sckt && present->accepted == false) {
			
			char* tmp_path = malloc( (strlen(present->path)+1) * sizeof(char) );
			strcpy(tmp_path, present->path);
			
			delete_file(past, present);
			
			return tmp_path;
		}
		
		past = present;
		present = present->next;
	}
	
	return NULL;
	
}

// Libère toutes les structures
void free_file(const pfile first) {
	
	pfile present = first;
	pfile past = NULL;
	
	while (present) {
		
		if (past != NULL && past != first)
			free(past);
		
		if (present->path != NULL)
			free(present->path);
			
		if (present->src != NULL && present->dest != NULL) {
			free(present->src);
			free(present->dest);
		}
			
		past = present;
		present = present->next;
		
	}
	
	if (past != NULL && past != first)
		free(past);
	
}

// Supprime une structure de fichier terminé
void file_finished(const pfile first, const puser src, const puser dest, const char* path) {
	
}

// Supprime une structure de fichier terminé, par la socket
void file_finished_sckt(const pfile first, const int sckt) {
	
	pfile present = first;
	pfile past = NULL;
	
	while (present) {
		
		if ( present->accepted == true && present->dest_sckt == sckt ) {
			
			delete_file(past, present);
			
			return;
		}
		
		past = present;
		present = present->next;
	}
}

// Supprime une structure de fichier. Si past = NULL => première struct.
void delete_file(const pfile past, const pfile present) {
	
	if (verbose) printf("Deletion de la structure de fichier %s\n", present->path);
			
	if (past == NULL) { // Première structure de la liste
		
		// S'il y a un fichier après, on le copie
		if (present->next != NULL) {
			
			pfile next = present->next;
			
			present->dest_sckt = next->dest_sckt;
			present->src_sckt = next->src_sckt;
			present->src = next->src;
			present->dest = next->dest;
			
			strcpy(present->path, next->path);
			
			present->accepted = next->accepted;
			present->finished = next->finished;
			
			free(next);
		}
		else {
			memset(present, 0, sizeof(struct file));
			return;
		}

	}
	
	else { // La structure avait un antécédent
		
		past->next = present->next;
		free(present->path);
		free(present);
	}
	
}
