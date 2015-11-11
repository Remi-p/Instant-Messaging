// ================ Programmation réseau : Jalon 06 ================= *\
//	Bonnotte Benjamin						       		     . \|/ /,  /
//	Perrot Remi			    ===============				     \\`''/_-- /
//	RE216				    |   Fichier   |			  Bordeaux INP --- /
//	Octobre 2015		    |  network.c  |			  ENSEIRB  ,..\\`  /
//						    ===============			  MATMECA / | \\`  /
// ================================================================== */

#include <contrib.h>
#include <network.h>

#include <stdlib.h> // exit()
#include <stdio.h> // perror()
#include <unistd.h> // close()
#include <string.h> // memset()

#include <netinet/in.h> // Pour
#include <arpa/inet.h>  // inet_addr

#include <sys/socket.h>
#include <netinet/in.h>


// Converti une structure IPv6 en IPv4
psockaddr convert_ipv6(struct sockaddr_in6 address) {
	
	struct sockaddr_in* addr_ipv4 = malloc(sizeof(struct sockaddr_in));
	
	addr_ipv4->sin_family = AF_INET;
	addr_ipv4->sin_port = address.sin6_port;
	
	addr_ipv4->sin_addr.s_addr = inet_addr(get_ip_from_sockaddr(&address));
	
	return addr_ipv4;
}

// Création du bind
void do_bind(int socket, struct sockaddr_in6* serv_add) {
	
	// Solution pour la taille de la structure trouvée sur
	// http://long.ccaba.upc.es/long/045Guidelines/eva/ipv6.html
	
	int bindtmp = bind(socket, (struct sockaddr *) serv_add, sizeof(struct sockaddr_in6));
	
	if (bindtmp == -1) {
		error("Voici l'erreur concernant la création du bind M. ");
	}
}

// Retourne le port d'une structure sockaddr
u_short get_port(struct sockaddr* adresse) {
	return ((struct sockaddr_in6*) adresse)->sin6_port;
}

// Acceptation d'un client
int do_accept(int sckt, struct sockaddr* adresse) {
	
	socklen_t length_client = sizeof(struct sockaddr);
	
	int sckt2 = accept(sckt, adresse, &length_client);
	
	if (sckt2 < 0) {
		error("Voici l'erreur concernant l'acceptation : ");
	}
	
	//printf("Bienvenue sur le tchat de l'ambiance, vous possédez la socket n°%i\n", sckt2);
	
	return sckt2;
}

// Ecoute d'un maximum de MAX_CLIENTS clients
void listen_of_ambiance(int socket) {
	
	int listendelambiance = listen(socket, MAX_CLIENTS);
	
	if (listendelambiance == -1) {
		error("Voici l'erreur concernant l'écoute M. Bonnotte ");
	}
}

//init the self address structure
// hostname can be NULL, we then use in6addr_loopback
struct sockaddr_in6* get_addr_info(int port, char* hostname) {
	
	// Gestion de l'IPv6 :
	// https://www-01.ibm.com/support/knowledgecenter/mobile/#!/ssw_ibm_i_72/rzab6/xacceptboth.htm
	// http://elinux.org/images/c/c6/Adapting_Your_Network_Code_For_IPv6_Support.pdf
	
	//~ struct sockaddr_in6 sin;
	//~ memset(&sin, 0, sizeof(struct sockaddr_in6));
	struct sockaddr_in6* sin = malloc(sizeof(struct sockaddr_in6));
	
	sin->sin6_family=AF_INET6;
	sin->sin6_port=htons(port);
	
	if (hostname == NULL)
		sin->sin6_addr = in6addr_loopback; //Utiliser loopback
	else {
		
		struct in6_addr adresse;
		
		// Test d'enregistrement de l'IP en tant qu'IPv4
		if (inet_pton(AF_INET, hostname, &adresse) == 1) {
			sin->sin6_addr = adresse;
			sin->sin6_family = AF_INET;
		}
		// De même avec IPv6
		else if (inet_pton(AF_INET6, hostname, &adresse) == 1)
			sin->sin6_addr = adresse;
		else
			error("Nous n'avons pas réussi à convertir l'adresse IP ");
		
	}
		
	//printf("Port : %i / Addr : %i", sin.sin6_port, sin.sin6_addr.s6_addr);
	
	return sin;

}

// Creation of socket 
int do_socket() {
	
	int sckt = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
	
	if (sckt == -1) {
		error("Voici l'erreur concernant la création de socket M. Perrot ");
	}
	
	// ------- Possiblement utile pour le support IPv4 (mappée en IPv6)
	//~ int v6only = 0;
    //~ setsockopt(sckt, IPPROTO_IPV6, IPV6_V6ONLY, &v6only, sizeof(int));
    //~ 
    //~ int addrform = 1;
    //~ setsockopt(sckt, IPPROTO_IPV6, IPV6_ADDRFORM, &addrform, sizeof(int));
	// ----------------------------------------------------------------
	
	return sckt;
}

// Message sending
void handle_client_message(int socket, char *input, int taille, enum code code_ret) {
	
	int offset = 0;
	
	// Ajout du code de retour
	char str[strlen(input) + 1];
	char char_ret = (char) code_ret; // Codé sur 8 bits
	taille++;
	
	str[0] = (char) code_ret; // Codé sur 8 bits
	memcpy(str + 1, input, strlen(input));
	//sprintf(str, "%c%s", char_ret, input);
	
	// Envoi
	do {
		offset = send(socket, str + offset, taille - offset, 0);
		
		if (offset == -1) {
			error("Voici l'erreur concernant l'envoi ");
		}
	}
	while (offset!=taille);
	
}

// Read les données envoyées par notre ami le client
// Retourne false si la socket est fermée
bool do_read(int socket, char *output, int taille, enum code* code_ret) {
	
	// Ici on réceptionnera la chaîne de caractère + le code
	char* msg_received = calloc( (TAILLE_MSG+1)*sizeof(char), 1 );
	
	// Le serveur attend l'envoi des données dans cette fonction (avec recv)
	memset(output,0,TAILLE_MSG*sizeof(char));
	
	ssize_t offset = 0;

	offset = recv(socket, msg_received + offset, taille - offset, 0);
	
	if (offset < 0)
		error("Erreur de lecture ");
		
	// D'après `man recv` : "If no messages are available to be received
	// and the peer has performed an orderly shutdown, recv() shall
	// return 0." Dans ce cas, output doit être égale à /quit
	else if (offset == 0) {
		free(msg_received);
		return false;
	}
	else {		
		// On enregistre le code de retour dans code_ret, et on copie la
		// chaîne reçue dans output
		
		if (code_ret != NULL)
			*code_ret = (enum code) msg_received[0];
		
		memcpy(output, msg_received + 1, TAILLE_MSG);
		
		free(msg_received);
		
		return true;
	}
}

// netstat -n -A inet pour les connexions actives
void close_socket(int socket) {
	
	int fermeture;
	
	fermeture = shutdown(socket, SHUT_RDWR);
	
	if (fermeture == -1) {
		error("Voici l'erreur concernant la fermeture de connexion (shutdown) ");
	}
	
	fermeture = close(socket);
	
	if (fermeture == -1) {
		error("Voici l'erreur concernant la fermeture de connexion (close) ");
	}
	
	if (verbose) printf("Extinction et fermeture de la sckt n°%i\n", socket);
	
}

int socket_error(int socket) {
	
	// http://stackoverflow.com/questions/4142012/how-to-find-the-socket-connection-state-in-c
	
	int error_sckt = 0;
	socklen_t len = sizeof (error);
	int retval = getsockopt(socket, SOL_SOCKET, SO_ERROR, &error_sckt, &len);
	
	//printf("Retval=%i / Error_sckt=%i\n",retval, error_sckt);
	
	if (retval != 0) {
		error("Impossible d'obtenir le code d'erreur de la socket ");
	}

	if (error_sckt != 0) {
		printf("Socket error : %i", error_sckt);
		return error_sckt;
	}
	
	else
		return 0;
}
