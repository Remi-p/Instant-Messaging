cmake_minimum_required(VERSION 2.8.3)

project(RE216_JALON06)


source_group("Headers" REGULAR_EXPRESSION ".*\\.h$")


include_directories( 
	commons
	server
	client)
#add here sources that are common between server and client
SET(SRC_COMMONS
	commons/network.c
	commons/contrib.c
	commons/file.c
)

#add here source files you need to compile and link to the client app
SET(SRC_CLIENT
client/client.c
client/client_lib.c
)


add_executable(RE216_JALON06_CLIENT ${SRC_CLIENT} ${SRC_COMMONS})


#add here source files you need to compile and link to the server app
SET(SRC_SERVER
server/callbacks.c
server/server_app.c
server/server_lib.c
server/user.c
server/salon.c
)


add_executable(RE216_JALON06_SERVER  ${SRC_SERVER} ${SRC_COMMONS})
