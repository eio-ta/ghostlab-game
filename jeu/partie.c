#include "partie.h"

int portDIFFUSION = 6000; // Port UDP initial pour une partie
int aDIFFUSION = 0; // Adresse initiale pour une partie



/* FONCTIONS  ***************************************************************************************************/

joueur *initialise_un_joueur(char *pseudo, int portUDP, int client_sock) {
    joueur *dest = malloc(sizeof(struct joueur) + 1);
    dest->pseudo = malloc(sizeof(char) *9);
    dest->pseudo[8] = '\0';
    snprintf(dest->pseudo, 8, "%s", pseudo);
    dest->portUDP = portUDP;
    dest->numeroSocket = client_sock;
    return dest;
}

liste_de_joueurs *ajouter_un_joueur(joueur *j, liste_de_joueurs *joueurs_connectes, int nb_partie) {
    liste_de_joueurs *element = malloc(sizeof(liste_de_joueurs));
    if(element == NULL) return NULL;
    element->j = j;
    element->m = nb_partie;
    element->pret = 0;
    element->suivant = NULL;
    if(joueurs_connectes == NULL) {
        return element;
    } else {
        liste_de_joueurs *tmp;
        tmp = joueurs_connectes;
        while(tmp->suivant != NULL) {
            tmp = tmp->suivant;
        }
        tmp->suivant = element;
        return joueurs_connectes;
    }
}

liste_de_joueurs *rechercher_un_joueur(char *pseudo, liste_de_joueurs *joueurs_connectes) {
    liste_de_joueurs *tmp = joueurs_connectes;
    while(tmp != NULL) {
        if(strcmp(tmp->j->pseudo, pseudo) == 0) {
            return tmp;
        } else {
            tmp = tmp->suivant;
        }
    }
    return NULL;
}

partie *initialise_une_partie(int m) {
    partie *dest = malloc(sizeof(struct partie) + 1);
    dest->m = m;
    initialise_labyrinth(dest->tab, H, W);
    placement_aleatoire_fantome(dest->tab, H, W);
    dest->joueurs_inscrits = 1;

    int a = aDIFFUSION;
    dest->adresse = malloc(sizeof(char) * 16);
    dest->adresse[15] = '\0';
    snprintf(dest->adresse, 15, "225.200.200.%d", a);
    aDIFFUSION ++;

    int p = portDIFFUSION;
    dest->port = malloc(sizeof(char) * 6);
    dest->port[5] = '\0';
    snprintf(dest->port, 5, "%d", p);
    portDIFFUSION ++;

    dest->f_a = 0;

    return dest;
}

liste_de_parties *ajouter_une_partie(partie *p, liste_de_parties *parties_commencees) {
    liste_de_parties *element = malloc(sizeof(liste_de_parties));
    if(element == NULL) return NULL;
    element->p = p;
    element->suivant = NULL;
    if(parties_commencees == NULL) {
        return element;
    } else {
        liste_de_parties *tmp;
        tmp = parties_commencees;
        while(tmp->suivant != NULL) {
            tmp = tmp->suivant;
        }
        tmp->suivant = element;
        return parties_commencees;
    }
}

liste_de_parties *rechercher_une_partie(liste_de_parties *parties_commencees, int indice) {
    liste_de_parties *tmp = parties_commencees;
    for(int i=0; i<indice-1 && tmp != NULL; i++) {
        tmp = tmp->suivant;
    }
    if(tmp == NULL) {
        return NULL;
    } else {
        return tmp;
    }
}

int compte_parties_non_commencees(liste_de_parties *parties_commencees) {
    int number = 0;
    liste_de_parties *tmp = parties_commencees;
    while(tmp != NULL) {
        if((tmp->p->joueurs_inscrits) < MAX_JOUEURS) {
            number++;
        }
        tmp = tmp->suivant;
    }
    return number;
}

int tlm_est_pret(liste_de_parties *pc, liste_de_joueurs *jc) {
    if(pc->p->joueurs_inscrits < MAX_JOUEURS) {
        return 0;
    } else {
        liste_de_joueurs *tmp = jc;
        while(tmp != NULL) {
            if(tmp->m == pc->p->m) {
                if(tmp->pret == 0) {
                    return 0;
                }
            }
            tmp = tmp->suivant;
        }
        return 1;
    }
}

messageUDP *initialise_un_message_UDP(char *pseudo, char *message, int portUDP) {
    messageUDP *mes = malloc(sizeof(struct messageUDP) + 1);

    mes->mes = malloc(sizeof(char) * strlen(message) + 1);
    snprintf(mes->mes, strlen(message), "%s", message);
    mes->mes[strlen(message)] = '\0';

    mes->pseudo = malloc(sizeof(char) * 9);
    snprintf(mes->pseudo, 8, "%s", pseudo);
    mes->pseudo[8] = '\0';

    mes->portUDP = portUDP;

    return mes;
}
