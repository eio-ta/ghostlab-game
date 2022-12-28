INFORMATIONS GHOSTLAB

Identifiants et membres du groupe :
   1. Zahra Alliche
   2. Zina Kacete
   3. Elody Tang



COMPILATIONS ET EXECUTION :

- Compiler le projet : make

- Exécuter le projet :
   - Pour lancer le serveur : ./serveur [PORT TCP DU SERVEUR]

   - Pour lancer les clients :
        1. Aller dans le dossier client : cd client
        2. Lancer la commande : java Client [PSEUDO] [PORT TCP DU SERVEUR]

[PORT TCP DU SERVEUR] doit être le même port dans toutes les commandes.
Le client peut être lancé plusieurs fois, avec des Identifiants différents.
Le serveur est le client doivent se lancer sur deux terminaux différents.


Pour faciliter le déplacement des objets dans le plateau de jeu et
comprendre l'envoi des messages, le plateau de jeu est visible depuis le
serveur. Cachez-le pour jouer !



ARCHITECTURES DU PROJET :

Il y a trois dossiers qui représentent trois parties différentes de notre
projet :
   1. Le dossier serveur (en C) qui gère toutes les facettes du serveur
   (TCP ou UDP). Il se lance en TCP et crée des threads UDP quand il a
   besoin d'envoyer des messages en UDP.

   2. Le dossier jeu (en C) qui gère le toute la partie du labyrinthe :
   sa création, son initialisation, le placement des fantômes, les
   mouvements des joueurs.

   3. Le dossier client (en Java) qui gère la partie TCP et UDP du Client.
   Le Client se lance et crée un client UDP et TCP en même temps. Le
   thread en multidiffusion se lance seulement lorsque le client est prêt
   à jouer (le client s'inscrit dans une partie puis envoie "START".)