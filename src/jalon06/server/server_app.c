// ================ Programmation réseau: Jalon 06 ================== *\
//	Bonnotte Benjamin						       		     . \|/ /,  /
//	Perrot Remi			    ===============				     \\`''/_-- /
//	RE216				    |   Fichier   |			  Bordeaux INP --- /
//	Octobre 2015		    | server_app.c|			  ENSEIRB  ,..\\`  /
//						    ===============			  MATMECA / | \\`  /
// ================================================================== */

// http://www.lowtek.com/sockets/select.html

#include "../commons/contrib.h"
#include "../commons/network.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h> // sockaddr_in6

#include "server_lib.h"
#include "callbacks.h"

int main(int argc, char** argv)
{
	
    if (argc != 2) {
        fprintf(stderr, "usage: RE216_SERVER port\n");
        return 1;
    }
    
	// -----------------------------------------------------------------
	// ------------------------ Variables ------------------------------
	    
    // Création de la socket
	int sckt = do_socket();  

    // Initialisation de la structure sockaddr_in6 pour l'adresse du server
	struct sockaddr_in6* serv_add = get_addr_info(atoi(argv[1]), NULL);
	
	// Liste chaînée des sockets connectées + pseudos
	struct user first_connected;
		memset(&first_connected, 0, sizeof(struct user));
	int nb_user = 0;
	// Liste chaînée des salons
	struct salon salons;
		memset(&salons, 0, sizeof(struct salon));
	// Liste chaînée pour l'envoi de fichiers
	struct file fichiers;
		memset(&fichiers, 0, sizeof(struct file));
	
	// Permet de stocker temporairement l'adresse d'un client
	struct sockaddr* adr_tmp;
	
	// Boolean pour savoir si un client a déjà quitté le serveur
	bool sbdy_quit = false;
	
	// Ici on stockera toutes les sockets des clients en écoute ( + le
	// listen du serveur)
	fd_set socks;
							
	int i, j; // Pour les loops
	
	int plus_grde_sckt;
	plus_grde_sckt = sckt;
	
	int retour_select;
	
	struct timeval tv;
	tv.tv_sec = 5;
	tv.tv_usec = 0;

	// Initialisation des regex
	init_reg();

	// -----------------------------------------------------------------
	// -----------------------------------------------------------------

    // On bind la socket sur le port TCP spécifié auparavant
    do_bind(sckt, serv_add);
    
    // Set to non blocking
    //setnonblocking(sckt);

	printf("Lancement du serveur Echo\n");

    // On fait en sorte que la socket écoute au plus MAX_CLIENTS clients
    listen_of_ambiance(sckt);
    
    
	// ---------------------- Premier set ------------------------------
	FD_ZERO(&socks);
	
	FD_SET(sckt,&socks);
	
	plus_grde_sckt = sckt;

    do { // Boucle principale
		
		if (verbose) {
			printf("\n -- Nouvelle itération -- \n");
			display_users_v(&first_connected);
		}
		
		retour_select = select( plus_grde_sckt + 1,
								&socks, NULL, NULL, NULL);
		
		if (retour_select < 0)
			error("Erreur lors du select");
			
		else {
			
			// Tout d'abord, on observe s'il y a de nouvelles connexions
			if (FD_ISSET(sckt,&socks)) {
				
				if (nb_user < MAX_CLIENTS) {
					
					adr_tmp = malloc(sizeof(struct sockaddr));
					
					j = do_accept(sckt, adr_tmp);
					
					new_client(&first_connected, j, adr_tmp);
					nb_user++;
					
					handle_client_message(j, string_connect, strlen(string_connect), OK_SERVEUR);
					
					printf("Nouveau client (%i) connecté\n", j);
					
					// La libération de la structure s'effectuera à la
					// suppression
				}
				else
					refus(sckt);
				
			}
			
			// Reception de données : on a vérifié si c'était une
			// nouvelle connexion juste avant, maintenant on regarde si
			// c'est de la réception de données sur une connexion déjà
			// ouverte
			if (serv_reception(&first_connected, &salons, &fichiers, &socks, string_connect, verbose, &nb_user))
				sbdy_quit = true;
			
		}
    
		// ----------------------- R.A.Z -------------------------------
		/*    / \    On doit faire la remise à zero à la fin de la
		 *   / ! \   boucle, sinon la condition du while sur
		 *  /_____\  plus_grde_sckt aura un "select" de retard
		 */
		FD_ZERO(&socks);
		
		FD_SET(sckt,&socks);
		
		plus_grde_sckt = sckt; 	// Il faut changer dynamiquement
								// plus_grde_sckt puisque des
								// descripteurs peuvent être fermés
		
		// On parcourt les connexions existantes pour les ajouter à
		// notre set.
		plus_grde_sckt = add_connex(&first_connected, &socks, sckt);
		
		// -------------------------------------------------------------
		
		//printf("plus grde = %i / sckt = %i / sbdy quit = %i\n", plus_grde_sckt, sckt, sbdy_quit);

		if (verbose) printf(" -- -------------------- -- \n");

    /* On regarde si plus_grde_sckt est égale à sckt => si oui, c'est
     * qu'il n'y a aucun client connecté.
     * La seconde condition concerne sbdy_quit : s'il est égale à true,
     * c'est que le serveur a déjà été connecté à des clients et que
     * l'un d'eux s'est déconnecté
     */
    } while( (sckt != plus_grde_sckt)
			 || !sbdy_quit );

    // Et on ferme la socket
    close_socket(sckt);
    
    // Libération des fichiers
    free_file(&fichiers);
    // Libération des utilisateurs
    free_user(&first_connected);
    
    // Libération regex
    free_reg();
	
	printf("\nExtinction du serveur. Merci d'avoir utilisé notre service de tchat\n");

    return 0;
}
