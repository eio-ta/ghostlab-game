#include <unistd.h>
#include <sys/socket.h>
#include <time.h>
#include <pthread.h>
#include <netinet/in.h>
#include <netdb.h>

#include "../jeu/partie.h"

#define MAX_SIZE_MESSAGE 200 // Taille d'un message maximale



/*  COMMUNICATION ENTRE LE SERVEUR ET LE CLIENT  ****************************************************************/

/* Gère la communication avec le client */
void *interaction(void *sock);



/* PREPARE ET ENVOIE LA REPONSE DU SERVEUR AU CLIENT  ***********************************************************/

/* Envoie la réponse du serveur dans la socket en TCP */
void send_request(char *server_reply, int client_sock);

/* Prépare la réponse du serveur en fonction de la requête du client */
void process_reply_server(char *client_message, int client_sock, char *pseudo);



/*  PREPARE LA REPONSE DU SERVEUR EN FONCTION DU MESSAGE DU CLIENT  *********************************************/

void req_NEWPL(char *client_message, char *request, int client_sock);
void req_REGIS(char *client_message, char *request, int client_sock);
void req_UNREG(char *client_message, char *pseudo, int client_sock);
void req_SIZE(char *client_message, char *request, int client_sock);
void req_LIST(char *client_message, char *request, int client_sock);
void req_GAME(char *client_message, char *request, int client_sock);
void req_START(char *pseudo, int client_sock);
char *req_POST(char *pseudo);

void req_deplacement(char *client_message, partie *partie, joueur *joueur, int client_sock);
void req_deplacement_2(joueur* joueur, int bonus, int client_sock);
void req_GLIS(char *pseudo, int client_sock);
void req_MALL(char *client_message, char *request, int client_sock, char *pseudo);
void req_SEND(char *client_message, char *request, char *pseudo, int client_sock);
void req_IQUIT(int client_sock);

void ser_SCORE(joueur *j);
void ser_GHOST(partie *p, char *pseudo);
void ser_END(char *pseudo, int client_sock);
void addZero(char *des, int nb, int nb_octets,char *res);



/*  MULTIDIFFUSION ET ENVOI EN UDP  *****************************************************************************/

/* Envoi d'un message en multidiffusion en UDP */
void *communication_Multi(void *a);

/* Envoi d'un message en UDP à un client */
void *communication_UDP(void *a);
