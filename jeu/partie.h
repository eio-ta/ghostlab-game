#include "labyrinthe.h"

#define H 10 // Hauteur d'un labyrinthe
#define W 20 // Largeur d'un labyrinthe
#define F 8 // Nombre de fantômes
#define MAX_JOUEURS 1 // Max de joueurs dans une partie


/*  STRUCTURE POUR LES JOUEURS  *********************************************************************************/

typedef struct joueur {
    int numeroSocket; // Numéro de socket d'un client
    char *pseudo; // Identifiant d'un client
    int portUDP; // PORT UDP d'un client
    char p[5]; // points
    char PosX[4]; // Position X d'un client
    char PosY[4]; // Position Y d'un client
} joueur;

typedef struct liste_de_joueurs {
    joueur *j;
    uint8_t m; // Numéro de la partie
    int pret; // 0 = Pas Prêt à Jouer
    struct liste_de_joueurs *suivant;
} liste_de_joueurs;


/*  STRUCTURE POUR LES PARTIES  *********************************************************************************/

typedef struct partie {
    uint8_t m; // Numéro de la partie
    int *tab[H]; // Tableau des labyrinthes
    int joueurs_inscrits; // Nombre Joueurs inscrits
    char *adresse; // Adresse de multidifusion de la partie
    char *port; // Port UDP de la partie
    int f_a; // Fantome attrapés
} partie;

typedef struct liste_de_parties {
    partie *p;
    struct liste_de_parties *suivant;
} liste_de_parties;



/*  STRUCTURES POUR LES MESSAGES UDP  ***************************************************************************/

typedef struct messageUDP {
    char *pseudo; // Identifiant de la source
    char *mes; // Message
    int portUDP; // Port UDP
} messageUDP;



/* FONCTIONS  ***************************************************************************************************/

/* Initialise un joueur */
joueur *initialise_un_joueur(char *pseudo, int portUDP, int numeroSocket);

/* Ajoute un joueur dans la liste */
liste_de_joueurs *ajouter_un_joueur(joueur *j, liste_de_joueurs *joueurs_connectes, int nb_partie);

/* Recherche un joueur dans la liste */
liste_de_joueurs *rechercher_un_joueur(char *pseudo, liste_de_joueurs *joueurs_connectes);

/* Initialise une partie dans la liste*/
partie *initialise_une_partie(int m);

/* Ajoute une partie dans la liste */
liste_de_parties *ajouter_une_partie(partie *p, liste_de_parties *parties_commencees);

/* Recherche une partie dans la liste */
liste_de_parties *rechercher_une_partie(liste_de_parties *parties_commencees, int indice);

/* Compte le nombre de parties non commencees */
int compte_parties_non_commencees(liste_de_parties *parties_commencees);

/* Vérifie si tout le monde est prêt dans une partie */
int tlm_est_pret(liste_de_parties *pc, liste_de_joueurs *jc);

/* Initialise un message UDP */
messageUDP *initialise_un_message_UDP(char *pseudo, char *message, int portUDP);
