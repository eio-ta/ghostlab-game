#include "labyrinthe.h"



/* FONCTIONS PRINCIPALES ***************************************************************************************/

void initialise_labyrinth(int **laby, uint16_t h, uint16_t w) {
   for(int i=0; i<h; i++) {
      laby[i] = (int*) malloc(w * sizeof(int));
      for(int j=0; j<w; j++) {
         laby[i][j] = 1;
      }
   }

   if(min(h, w) == h) {
      for(int k=1; k<w; k++) {
         int x = h/2;
         int y = k;
         laby[x][y] = 0;
         int *adj[4];
         int value = adjacents(adj, laby, x, y, h, w);

         while(value != 0) {
            int alea = (rand() % (value - 1 + 1)) + 1;
            int tmp_x = adj[alea-1][0];
            int tmp_y = adj[alea-1][1];
            x = tmp_x;
            y = tmp_y;
            laby[x][y] = 0;
            free_tab(adj, value);
            value = adjacents(adj, laby, x, y, h, w);
         }
         free_tab(adj, value);
      }
   } else {
      for(int k=0; k<h; k++) {
         int x = k;
         int y = w/2;
         laby[x][y] = 0;
         int *adj[4];
         int value = adjacents(adj, laby, x, y, h, w);

         while(value != 0) {
            int alea = (rand() % (value - 1 + 1)) + 1;
            int tmp_x = adj[alea-1][0];
            int tmp_y = adj[alea-1][1];
            x = tmp_x;
            y = tmp_y;
            laby[x][y] = 0;
            free_tab(adj, value);
            value = adjacents(adj, laby, x, y, h, w);
         }
         free_tab(adj, value);
      }
   }
}

void free_tab(int **tab, int h) {
   for(int i=0; i<h; i++) {
      free(tab[i]);
   }
}

void print_labyrinth(int **laby, uint16_t h, uint16_t w) {
   for(int i=0; i<h; i++) {
      for(int j=0; j<w; j++) {
         switch(laby[i][j]) {
            case 0: printf("⬜  "); break;
            case 1: printf("⬛  "); break;
            case 2: printf("🟥  "); break; // Fantôme
            default: printf("🟦  "); break; // Joueurs
         }
      }
      printf("\n\n");
   }
   printf("\n");
}



/* FONCTIONS AUXILIAIRES ****************************************************************************************/

int is_not_wall(int **laby, int x, int y) {
   return laby[x][y];
}

int adjacents(int **adj, int **laby, int i, int j, uint16_t h, uint16_t w) {
   int value = 0;
   if(j-1 >= 0 && is_not_wall(laby, i, j-1) != 0) {
      adj[value] = (int*) malloc(2 * sizeof(int));
      adj[value][0] = i,
      adj[value][1] = j-1;
      value++;
   }
   if(i-1 >= 0 && is_not_wall(laby, i-1, j) != 0) {
      adj[value] = (int*) malloc(2 * sizeof(int));
      adj[value][0] = i-1,
      adj[value][1] = j;
      value++;
   }
   if(i+1 < h && is_not_wall(laby, i-1, j) != 0) {
      adj[value] = (int*) malloc(2 * sizeof(int));
      adj[value][0] = i+1,
      adj[value][1] = j;
      value++;
   }
   if(j+1 < w && is_not_wall(laby, i, j+1) != 0) {
      adj[value] = (int*) malloc(2 * sizeof(int));
      adj[value][0] = i,
      adj[value][1] = j+1;
      value++;
   }
   return value;
}


void placement_aleatoire_joueur(int client_sock, int **laby, int x, int y) {
   laby[x][y] = client_sock;
}

void placement_aleatoire_fantome(int **laby, uint16_t h, uint16_t w) {
   for(int i=0; i<8; i++) {
      int x_alea = rand() % (h);
      int y_alea = rand() % (w);
      while(laby[x_alea][y_alea] != 0) {
         x_alea = rand() % (h);
         y_alea = rand() % (w);
      }

      laby[x_alea][y_alea] = 2;
   }
}



/*  FONCTIONS  **************************************************************************************************/

int min(int a, int b) {
    return ( a < b ? a : b);
}
