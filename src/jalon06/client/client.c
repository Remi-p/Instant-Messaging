// ================ Programmation réseau : Jalon 06 ================= *\
//	Bonnotte Benjamin						       		     . \|/ /,  /
//	Perrot Remi			    ===============				     \\`''/_-- /
//	RE216				    |   Fichier   |			  Bordeaux INP --- /
//	Octobre 2015		    |  client.c   |			  ENSEIRB  ,..\\`  /
//						    ===============			  MATMECA / | \\`  /
// ================================================================== */

#include "../commons/contrib.h"
#include "../commons/network.h"
#include "../commons/file.h"
#include "client_lib.h"

#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h> // Pour le memset

#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc,char** argv) {

    if (argc != 3) {
        fprintf(stderr,"usage: RE216_CLIENT hostname port\n");
        return 1;
    }
    
	// -----------------------------------------------------------------
	// ------------------------ Variables ------------------------------
	
	// Buffer
	char *input = NULL;		// Taille d'entrée dynamique
	char output[TAILLE_MSG];// Taille de réception fixée
    
	// Liste chaînée pour l'envoi de fichiers
	struct file fichiers;
		memset(&fichiers, 0, sizeof(struct file));

	// Récupération de la structure sockaddr_in6 pour l'adresse du serveur
	struct sockaddr_in6* server_add = get_addr_info(atoi(argv[2]), argv[1]);

	// Création de la socket
	int sckt = do_socket();  

	// Connexion de la socket à l'adresse server_add
	int conn = do_connect(sckt, *server_add);

	// Initialisation des tableaux pour utliser select ------------------
    
    fd_set fd_set_read; // Ici seront stockés les descripteurs de fichier
    
    int i, select_ret_val;
    
    // Socket du serveur quand elle existe
    int socket_fichier = -1;
    
    // Eventuellement : timeout du select
	//~ struct timeval tv;
	//~ tv.tv_sec = 5;
	//~ tv.tv_usec = 0;
	
	init_reg();
	// -----------------------------------------------------------------
	// -----------------------------------------------------------------
	
	start_line();

	// Boucle jusqu'à recevoir le "/quit" final
	do {
		
		// ------------------------ R.A.Z ------------------------------
		// clean the set before adding file descriptors
		FD_ZERO(&fd_set_read);
		
		// add the fd for server connection
		FD_SET(sckt, &fd_set_read);

		// add the fd for user-input
		FD_SET(fileno(stdin), &fd_set_read);
		// -------------------------------------------------------------
		 
		// we now wait for any file descriptor to be available for reading
        select_ret_val = select(sckt + 1, &fd_set_read, NULL, NULL, NULL);//&tv);
        
        if (select_ret_val == -1) {
			error("Erreur concernant le select ");
		}
        
        //printf("Le retour de la fonction select est : %i", select_ret_val);
		
		for (i = 0; i < (sckt+1) && select_ret_val > 0; i++) {
	
            // Le buffer est nettoyé avec memset directement dans les fonctions
            
            //printf("Bonjour je suis le i n°%i. Retour => %i\n", i, select_ret_val);

            // Si le file descripteur i est dans le set mis en écoute, c'est qu'il y a une activité
            if (FD_ISSET(i, &fd_set_read)) {
				
				// printf("Descripteur trouvé : %i\n", i);

                if (i == fileno(stdin)) // C'est une entrée utilisateur
					client_write(&input, sckt, &fichiers);
				
				else // Les données viennent du serveur
					if (!client_read(sckt, output, &fichiers, &socket_fichier))
						break;

                // Select_ret_val est le nombre de descripteurs où il y 
                // a eu une activité, on diminue donc sa valeur au fur
                // et à mesure.
                select_ret_val--;

            }
        }

	} while(notquit(input) && notquit(output));

	//printf("Extinction.\n");

	free(input);
	
	free_file(&fichiers);

	free_reg();

	// Fermeture de la socket
    close_socket(sckt);
    
	printf("Fin du tchat\n");
	
    return 0;
}
