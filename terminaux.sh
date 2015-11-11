#!/bin/sh

# Lancement des terminaux pour tester le tchat

DIR=~/Cours/RE216/jalon06/build/jalon06
GEO=66x18
WAIT=1
cd $DIR;
make;
lxterminal --working-directory=$DIR --geometry=$GEO;
lxterminal --working-directory=$DIR --geometry=$GEO;
lxterminal --working-directory=$DIR --geometry=$GEO;
lxterminal --working-directory=$DIR --geometry=$GEO;
#./RE216_JALON05_SERVER $1 &
#lxterminal --working-directory=$DIR --geometry=$GEO --command="./RE216_JALON05_CLIENT 127.0.0.1 $1" &
#lxterminal --working-directory=$DIR --geometry=$GEO --command="./RE216_JALON05_CLIENT 127.0.0.1 $1" &
#lxterminal --working-directory=$DIR --geometry=$GEO --command="./RE216_JALON05_CLIENT 127.0.0.1 $1";
