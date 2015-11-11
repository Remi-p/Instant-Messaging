// ---------------------------------------------------------------- Help
//~ #ifndef CLIENT_H_sqdfqsdfqsdf
//~ #define CLIENT_H_sqdfqsdfqsdf
//~ 
//~ #include <sys/types.h>
//~ #include <sys/socket.h>
//~ #include <netdb.h>
//~ 
//~ /**
 //~ * a function that given the address and the port fill the addrinfo
 //~ */
//~ void get_addr_info(const char* address, const char* port,
		//~ struct addrinfo** res);
//~ 
//~ /**
 //~ * a function that is called when we receive a message from the server
 //~ */
//~ void handle_server_message(char buffer[256]);
//~ 
//~ /**
 //~ * a function called with user input
 //~ */
//~ void handle_client_message(const int socket, char buffer[256]);
//~ 
//~ /**
 //~ * function called when a file is received
 //~ */
//~ void handle_file_receive(const char *file_path, const char* addr,
		//~ const char* port);
//~ 
//~ /**
 //~ * function called when we need to send a file
 //~ */
//~ void handle_send_file(const int socket, const char* file_path);
//~ 
//~ #endif /* CLIENT_H_ */
// ---------------------------------------------------------------------
