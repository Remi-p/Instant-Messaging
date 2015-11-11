// ================ Programmation réseau : Jalon 06 ================= *\
//	Bonnotte Benjamin						       		     . \|/ /,  /
//	Perrot Remi			    ===============				     \\`''/_-- /
//	RE216				    |   Fichier   |			  Bordeaux INP --- /
//	Octobre 2015		    |  contrib.c  |			  ENSEIRB  ,..\\`  /
//						    ===============			  MATMECA / | \\`  /
// ================================================================== */

#include "contrib.h"
#include "callbacks.h"

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <sys/stat.h>

// Display printf
bool verbose = true;

// Segmentation fault
void gdb_stop() {	
	// http://stackoverflow.com/questions/18986351/what-is-the-simplest-standard-conform-way-to-produce-a-segfault-in-c
	const char *s = NULL;
	printf( "%c\n", s[0] );
}

// Test si le fichier est bien un fichier
int is_regular_file(const char *path) {
	//http://stackoverflow.com/questions/4553012/checking-if-a-file-is-a-directory-or-just-a-file
	
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

// Récupère la taille du fichier
off_t file_size(const int fd) {
	
    struct stat path_stat;
	fstat(fd, &path_stat);
	return path_stat.st_size;
}

// Retourne l'adresse ip en char*
char * get_ip_from_sockaddr(struct sockaddr_in6* addr) {
	
	// Buffer
	char* ipAddress;
	
	if (addr->sin6_family == AF_INET6) {
		ipAddress = malloc(INET6_ADDRSTRLEN*sizeof(char));
		// Conversion
		inet_ntop(AF_INET6, &(addr->sin6_addr), ipAddress, INET6_ADDRSTRLEN);
	}
	else { // IPv4
		ipAddress = malloc(INET_ADDRSTRLEN*sizeof(char));
		
		// Conversion
		inet_ntop(AF_INET, &(addr->sin6_addr), ipAddress, INET_ADDRSTRLEN);
	}
	
	return ipAddress;
}

// Affiche d'un message d'erreur et fermeture du programme
void error(const char *msg) {
    perror(msg);
    exit(1);
}

// Caractère de début de ligne
void start_line() {
	write(STDOUT_FILENO, "> ", 3);
}

// Changement de couleur synchrone
void color(const char* couleur) {
	write(STDOUT_FILENO, couleur, strlen(couleur));
}

// Renvoi la chaîne de caractère trouvée
char* get_string_reg(const regmatch_t pmatch, const char* output) {
	
	// Début et fin de la chaîne de caractère à copier
	int deb_string = pmatch.rm_so;
	int fin_string = pmatch.rm_eo;
	size_t taille_string = fin_string - deb_string;
	
	// Buffer
	char *string = malloc((taille_string+1)*sizeof(char));
	string[taille_string] = '\0';
	
	// Copies
	strncpy(string, &output[deb_string], taille_string);
	
	return string;
	
}

// Retourne le nom du fichier
char* nom_fichier(const char* path) {

	// http://www.tutorialspoint.com/c_standard_library/c_function_strtok.htm

	// On copie la chaîne path : en effet, on souhaite la conserver,
	// et strtok remplace le délimiteur "/" par \0.
	char path_split[strlen(path)];
	strcpy(path_split, path);

	char *token; char* previous;
	char s[2] = "/";
	/* get the first token */
	token = strtok(path_split, s);

	/* walk through other tokens */
	while( token != NULL ) {
	  previous = token;
	  token = strtok(NULL, s);
	}

	// Sauvegarde du nom de fichier en mémoire
	char *filename = malloc(sizeof(char) * (strlen(previous)+1) );
	// le +1 désigne le caractère de fin de chaîne; cf man strlen :
	// 		"The  strlen() function calculates the length of the string 
	// 		s, excluding the terminating null byte ('\0').

	strcpy(filename, previous);

	// On renvoie le dernier segment délimité par "/"
	return filename;
   	
}

//http://stackoverflow.com/questions/3068397/finding-the-length-of-an-integer-in-c
// Taille d'un entier (utilisé pour un port : <= 65535)
int len_int(unsigned x) {
	
    if(x>=10000) return 5;
    if(x>=1000) return 4;
    if(x>=100) return 3;
    if(x>=10) return 2;
    return 1;
    
}

// Compare les premiers caractères de la chaîne pour voir si "/quit" est
// entré
bool notquit(char* input) {
	
	// Si l'adresse de input est nulle
	if (input == NULL)
		return true;
	
	// Si "q" est entré => quit               /!\ Pas en prod
	//~ if (strlen(input) == 1 && input[0] == 'q')
	//~ 	return false;

	char * quit = "/quit";
	int j;
	
	for (j = 0; j < strlen(quit); j++ ) {
		if (quit[j] != input[j])
			return true;
	}
	
	return false;
}

// Assigne la chaîne de caractère "/quit" au tableau (utilisé dans client)
void toquit(char *str) {
	memset(str,0,TAILLE_MSG*sizeof(char));
	
	str[0] = '/';
	str[1] = 'q';
	str[2] = 'u';
	str[3] = 'i';
	str[4] = 't';
	str[5] = '\0';
}

// Stocke l'entrée de l'utilisateur dans input
ssize_t readline(char **input) {
	
	size_t len = NULL;
	ssize_t gtln = 0;
	
	// Pour que getline alloue dynamiquement la mémoire nécessaire, il
	// faut que input[0] et len soient NULL.
	if (input[0] != NULL) {
		free(input[0]);
		input[0] = NULL;
	}
	
	gtln = getline(input, &len, stdin);
	
	if (gtln < 0) {
		error("Voici l'erreur concernant la lecture de ligne ");
	}
	
	return gtln;
}
