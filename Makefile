CC ?= gcc
CCFLAGS ?= -Wall -c 
JC ?= javac

all : serveur Client.class

serveur: serveur.o partie.o labyrinthe.o
	$(CC) -pthread -o serveur serveur.o partie.o labyrinthe.o

serveur.o: server/serveur.c jeu/partie.c jeu/labyrinthe.c
	$(CC) $(CCFLAGS) -pthread server/serveur.c jeu/partie.c jeu/labyrinthe.c

Client.class: client/Client.java client/ComTCP.java client/ComUDP.java client/RecUDP.java
	$(JC) client/Client.java client/ComTCP.java client/ComUDP.java client/RecUDP.java

distclean:
	rm -f *.o serveur client/*.class