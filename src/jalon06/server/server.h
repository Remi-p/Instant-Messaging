// ---------------------------------------------------------------- Help
//~ /////////////////////////////////////////////////////////////////////////////////////////
//~ //BORDEAUX INP ENSEIRB-MATMECA
//~ //DEPARTEMENT TELECOM
//~ //RE216 PROGRAMMATION RESEAUX
//~ //{daniel.negru,joachim.bruneau_-_queyreix,nicolas.herbaut}@ipb.fr
//~ ////////////////////////////////////////////////////////////////////////////////////////
//~ 
//~ #ifndef SERVER_H_QSGPNSGPOQNSPGOSNDFG
//~ #define SERVER_H_QSGPNSGPOQNSPGOSNDFG
//~ 
//~ 
//~ /**
 //~ * handle the event when the server socket receives a new connection
 //~ * @return the new client socket fd
 //~ */
//~ int handle_server_socket_event(const int server_socket_fd);
//~ 
//~ /**
 //~ * handle the event when a client write something to us
 //~ */
//~ void handle_client_socket_event(int client_socket_fd);
//~ 
//~ /**
 //~ * parse a client input and react accordingly
 //~ */
//~ void parse_client_input(const char buffer[256], const int client_socket_fd);
//~ 
//~ /**
 //~ * Send a message to all users, except on the fd for the sender.
 //~ */
//~ void send_broadcast_by_user_name(const char buffer[256], const char* uname_src);
//~ void send_broadcast_by_fd(const char buffer[256], const int fd);
//~ 
//~ /**
 //~ * Send a message to a user by username
 //~ */
//~ void send_unicast(const char buffer[256], const char *uname,const char* uname_src);
//~ 
//~ /**
 //~ * Send a message to users by their usernames
 //~ */
//~ void send_multicast(const char buffer[256], const char **unames,const char* uname_src);
//~ 
//~ /**
 //~ * main function that understands the type of packets
 //~ */
//~ void process_client_request(const char buffer[256], const int client_socket_fd);
//~ 
//~ 
//~ #endif //SERVER_H_QSGPNSGPOQNSPGOSNDFG
// ---------------------------------------------------------------------
