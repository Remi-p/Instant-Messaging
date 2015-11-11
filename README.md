Benjamin Bonnotte, Rémi Perrot - T2-G1 - 2015

# Consignes

Le projet a pour objectif la réalisation d'une application de chat client/serveur en C permettant:
* d'échanger des messages entre 2 utilisateurs, entre plusieurs utilisateurs, ou à destination de la totalité des utilisateurs connectés sur le réseau;
* de s'envoyer des fichiers.

# Liste des points requis par le projet

## Jalon 01 : Etablissement d'un modèle client/serveur "Echo" sur TCP

1. Création d'un client qui se connecte en TCP à un serveur renseigné par un port et une adresse IPv4

2. Création d'un server avec une socket d'écoute qui accepte la connexion et gère les données entrantes.

3. Le client doit pouvoir prendre une chaîne de caractère en entrée, l'envoyer au serveur et recevoir de ce dernier la même chaîne de caractère.

4. La connexion doit se couper lorsque le client envoi '/quit'. Les sockets crées doivent être fermées et la mémoire allouée aux structures de données doit être libérée.

## Jalon 02 : Serveur Multi-client

1. Les clients se connectent au serveur renseigné par un port et une adresse IPv4.

```
%localhost > ./client port_number IP_address
```

2. Le client doit avoir la possibilité d'envoyer et recevoir des données au/depuis le serveur

3. La connexion doit se couper lorsque le client envoie '/quit'. Les sockets créées doivent être fermées et la mémoire allouée aux structures de données doit être libérée.

```
%> /quit
%> [Server] : You will be terminated
%> Connection terminated
%localhost >
```

4. Le serveur, en recevant une chaîne de caractère depuis un client, doit répéter cette chaîne uniquement à ce même client.

```
% > Hello World!
% > [Server] : Hello World!
```

5. Le serveur doit pouvoir maintenir 20 (et pas plus) connexions simultanées sans "crasher". Le 21ième client devra se voir refuser la connexion.

```
%localhost > ./client port_number IP_address
%> Server cannot accept incoming connections anymore. Try again later.
```

## Jalon 03 : Gestion utilisateurs

Note : à partir de ce jalon, uniquement les points requis étaient listés (sans explication sur la marche à suivre).

1. Une fois la connexion établie avec le serveur, le client DOIT s'identifier par son pseudo (commande `/nick`)

2. Le serveur DOIT gérer plusieurs utilisateurs et plusieurs connexions. Les utilisateurs (pseudo) et leurs infos liées à leur connexion (numéro de socket, socket fd) sont stockés par le serveur.

3. Le serveur DOIT tenir compte d'un utilisateur ou de son changement de pseudo.

4. un client DOIT pouvoir obtenir du serveur la liste des utilisateurs connectés. (commande `/who`)

5. un client DOIT pouvoir obtenir du serveur des informations sur un utilisateur en particulier (commande `/whois`)

6. Le serveur DOIT garder son rôle répétitif

## Jalon 04 : Création de l'application de Chat

1. : Un utilisateur DOIT pouvoir envoyer un message à tous les autres utilisateurs (**broadcast**).

2. : Un message envoyé NE DOIT pas être retransmis à l'expéditeur.

3. : Un utilisateur DOIT pouvoir envoyer un message privé à un autre utilisateur (**unicast**).

4. : Un utilisateur DOIT pouvoir créer un salon.

5. : Le serveur DOIT retourner un message d'erreur à l'utilisateur qui demande la création d'un salon déjà existant

6. : Un utilisateur DOIT pouvoir rejoindre et quitter un salon.

7. : Le serveur DOIT détruire le salon lorsque son dernier occupant le quitte.

8. : Un message envoyé dans un salon NE DOIT PAS être transmis à d'autres utilisateurs que ceux présents dans le salon (**multicast**).

## Jalon 05 : Transfer de fichier

1. : Un utilisateur (*l'émetteur*) DOIT pouvoir envoyer un fichier à un autre utilisateur (**le récepteur**)

2. : Lors du transfert d'un fichier, le récepteur DOIT donner son approbation

3. : Lors du transfert d'un fichier, le récepteur et l'émetteur doivent avoir confirmation que l'envoi s'est déroulé correctement.

## Jalon 06 : IPv6

C'est ce jalon, qui ne possède pas de point requis spécifique, qui est sur ce *repository*.

# Points notables

* Boolean verbose dans `server_lib.h` qui affiche des informations supplémentaires liées au code quand égale à 1
* Messages du serveur enregistrés dans les fichiers `server_lib.h` (mot-clé extern) et `server_lib.c` pour être accessibles des autres fichiers
* Fonctions de callback pour la gestion des commandes du serveur, dans `callbacks.c`
* Expressions régulières pour l'analyse des commandes reçues/envoyées (`client_lib.c`, `callbacks.c`). Réutilisation des mêmes regex pour les commandes similaires
* Codes de retour (inspirés des protocoles FTP, HTTP, etc.) pour les échanges dans `network.h`
