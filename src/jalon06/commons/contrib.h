// ================ Programmation réseau : Jalon 06 ================= *\
//	Bonnotte Benjamin						       		     . \|/ /,  /
//	Perrot Remi			    ===============				     \\`''/_-- /
//	RE216				    |   Fichier   |			  Bordeaux INP --- /
//	Octobre 2015		    |  contrib.h  |			  ENSEIRB  ,..\\`  /
//						    ===============			  MATMECA / | \\`  /
// ================================================================== */

#ifndef CONTRIB_H_

	#define CONTRIB_H_

	#define TAILLE_MSG 128

	#include <sys/types.h>
	#include <regex.h>
	
	#include <arpa/inet.h>

	typedef char bool;
	#define true 1
	#define false 0
	
	extern bool verbose;

	// Segmentation fault
	void gdb_stop();
	
	// Retourne l'adresse ip en char*
	char * get_ip_from_sockaddr(struct sockaddr_in6* addr);
	
	// Renvoi la chaîne de caractère trouvée
	char* get_string_reg(const regmatch_t pmatch, const char* output);

	// Affiche l'erreur sur la sortie standard
	void error(const char *msg);
	
	// Test si le fichier est bien un fichier
	int is_regular_file(const char *path);
	
	// Récupère la taille du fichier
	off_t file_size(const int fd);

	// Changement de couleur synchrone
	void color(const char* couleur);

	// Affiche le caractère de début de ligne
	void start_line();
	
	// Taille d'un entier (utilisé pour un port : <= 65535)
	int len_int(unsigned x);
	
	// Retourne le nom du fichier
	char* nom_fichier(const char* path);

	// Compare les premiers caractères de la chaîne pour voir si "/quit" est entré
	bool notquit(char* input);
	
	// Assigne la chaîne de caractère "/quit" au tableau
	void toquit(char *input);

	// Stocke l'entrée de l'utilisateur dans input
	ssize_t readline(char **input);

#endif /* CONTRIB_H_ */
