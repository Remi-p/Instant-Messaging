// ================ Programmation réseau : Jalon 06 ================= *\
//	Bonnotte Benjamin						       		     . \|/ /,  /
//	Perrot Remi			    ===============				     \\`''/_-- /
//	RE216				    |   Fichier   |			  Bordeaux INP --- /
//	Octobre 2015		    |    file.h   |			  ENSEIRB  ,..\\`  /
//						    ===============			  MATMECA / | \\`  /
// ================================================================== */

#ifndef FILE_H_

	#define FILE_H_

	#include "../server/user.h"

	struct file{
		char* path;
		
		struct sockaddr_in6* src;
		int src_sckt;
		
		struct sockaddr_in6* dest;
		int dest_sckt;
		
		bool accepted;
		bool finished; // Deprecated
		
		struct file* next;
	};
	
	typedef struct file* pfile;
	
	// Maj du numéro de socket correspondant au fichier
	void socket_maj(const pfile first, const struct sockaddr_in6* adr_tmp, const int sckt);
	
	// Display information on file structure
	void file_v(const pfile present);
	
	// Retourne "true" quand des fichiers sont accepté et pas encore
	// terminés
	bool still_not_finish(const pfile first);

	// Renvoi le fichier qui a été supprimé
	pfile get_and_delete(const pfile first, const char* seria);

	// Sérialisation de la structure suivant le format_serialise
	char* serialise(const pfile present);
	
	// Mets à jour les attributs d'un fichier 
	// (true si trouvé, false sinon)
	bool remplace_fichier(const pfile fichiers, const pfile present);

	// Renvoi la structure depuis la chaîne sérialisée
	pfile unserialise(const char* output);
	
	// Renvoie les informations nécessaires depuis une structure
	char* todata(const pfile present, int* length);
	
	// Renvoi les caractères en tant que structure
	pfile tofile(char* output);
	
	// Récupère une structure de fichier
	pfile get_file(const pfile first, const puser src, const puser dest, const char* path, pfile *past);
	
	// Ajoute un nouveau fichier dans la liste
	void new_fichier(const pfile first, const puser src, const puser dest, char* path);
	
	// Libère toutes les structures
	void free_file(const pfile first);
	
	// Renvoi la structure de fichier pour laquelle l'utilisateur doit répondre
	pfile waiting_for_user(const pfile first, const int sckt);

	// Supprime la structure avec accepted à false et ->path = path
	void file_refused(const pfile first, const char* path);

	// Supprime la structure avec accepted à false et dest_sckt = sckt,
	// et renvoie le path
	char* file_refused_path(const pfile first, const int sckt);
	
	// Assigne 'true' à accepted de la structure
	// Renvoi la structure sérialisée
	char* file_accepted(const pfile first, const int sckt, pfile* fichier);
	
	// Supprime une structure de fichier terminé
	void file_finished(const pfile first, const puser src, const puser dest, const char* path);
	
	// Supprime une structure de fichier terminé, par la socket
	void file_finished_sckt(const pfile first, const int sckt);
	
	// Supprime une structure de fichier. Si past = NULL => première struct.
	void delete_file(const pfile past, const pfile present);

#endif /* CONTRIB_H_ */
