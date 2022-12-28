#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>
#include <inttypes.h>



/*  FONCTIONS PRINCIPALES  **************************************************************************************/

/* Initialise un labyrinthe */
void initialise_labyrinth(int **laby, uint16_t h, uint16_t w);

/* Affiche un labyrinthe */
void print_labyrinth(int **laby, uint16_t h, uint16_t w);

/* Libère la mémoire d'un tableau */
void free_tab(int **tab, int h);



/*  FONCTIONS AUXILIAIRES  **************************************************************************************/

/* Vérifie si une case (x, y) est un mur ou pas */
int is_not_wall(int **laby, int x, int y);

/* Retourne le nombre de voisins qui sont des mures de la case (j,j) */
int adjacents(int **adj, int **laby, int i, int j, uint16_t h, uint16_t w);

/* Place un joueur aléatoirement sur une case vide en début de partie */
void placement_aleatoire_joueur(int client_sock, int **laby, int x, int y);

/* Place les fantômes en début de partie */
void placement_aleatoire_fantome(int **laby, uint16_t h, uint16_t w);

/*  FONCTIONS  **************************************************************************************************/

/* Retourne le chiffre minimum entre a et b */
int min(int a, int b);