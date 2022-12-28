#include "serveur.h"

uint8_t n = 0; // Nombre de parties
liste_de_parties *pc = NULL; // Liste des parties
liste_de_joueurs *jc = NULL; // Liste des joueurs



/*  COMMUNICATION ENTRE LE SERVEUR ET LE CLIENT  ****************************************************************/

void *interaction(void *sock) {
    int client_sock = *((int *)sock);

    if(n == 0) {
        int size = strlen("GAMES ***") + 3;
        char message[size];
        snprintf(message, size, "GAMES %d***", n);
        send_request(message, client_sock);
    } else {
        int size = (strlen("OGAME  ***") + 6)*n + strlen("GAMES ***") + 3;
        char *message = malloc(sizeof(char) * size);
        sprintf(message, "GAMES %d***", n);
        for(int i=1; i<n+1; i++) {
            liste_de_parties *tmp = rechercher_une_partie(pc, i);
            char loop[strlen("OGAME  ***") + 6];
            snprintf(loop, strlen("OGAME  ***") + 6, "OGAME %d %d***", tmp->p->m, tmp->p->joueurs_inscrits);
            strcat(message, loop);
        }
        send_request(message, client_sock);
        free(message);
    }

    char pseudo[8];
    int portUDP;

    while(1) {
        char buffer[MAX_SIZE_MESSAGE];
        int read_size = recv(client_sock, buffer, MAX_SIZE_MESSAGE, 0);
        buffer[MAX_SIZE_MESSAGE - 1] = '\0';

        if(read_size != 0) {
            char *client_message = malloc(sizeof(char) *read_size + 1);
            strncpy(client_message, buffer, read_size);
            client_message[read_size] = '\0';
            printf("Message reçu : %s.\n", client_message);

            if(client_message != NULL) {
                char request[6];
                strncpy(request, client_message, 5);
                request[5] = '\0';
                if(strcmp(request, "NEWPL") == 0) {
                    int portUDP;
                    sscanf(client_message, "%s %s %d", request, pseudo, &portUDP);
                } else if(strcmp(request, "REGIS") == 0) {
                    int nb_partie;
                    sscanf(client_message, "%s %s %d %d", request, pseudo, &portUDP, &nb_partie);
                }
            }

            process_reply_server(client_message, client_sock, pseudo);
            free(client_message);

            char request[6];
            strncpy(request, client_message, 5);
            request[5] = '\0';

            if(strcmp(request, "IQUIT") == 0) {
                return NULL;
            }
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if(argc != 2) {
        printf("Nombre d'arguments en ligne de commande invalide.\n");
        return EXIT_FAILURE;
    }

    int socket_desc = socket(PF_INET, SOCK_STREAM, 0);
    if(socket_desc == -1) return EXIT_FAILURE;
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(atoi(argv[1]));

    if((bind(socket_desc, (struct sockaddr *)&server, sizeof(server))) == -1) {
        return EXIT_FAILURE;
    } else {
        if((listen(socket_desc, 0)) == -1) return EXIT_FAILURE;
        printf("Connexion sur le port %d...\n", atoi(argv[1]));
        int c = sizeof(struct sockaddr_in);

        while(1) {
            /* Connexion entre le serveur et le client */
            struct sockaddr_in client;
            int *client_sock = (int *)malloc(sizeof(int));
            *client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c);

            if(*client_sock == -1) {
                printf("Un client n'a pas réussi à se connecter.\n");
            } else {
                printf("Un client a réussi à se connecter.\n");
                pthread_t th;
                pthread_create(&th, NULL, interaction, client_sock);
            }
        }
    }

    return EXIT_SUCCESS;
}




/* PREPARE ET ENVOIE LA REPONSE DU SERVEUR AU CLIENT  ***********************************************************/

void send_request(char *server_reply, int client_sock) {
    send(client_sock, server_reply, strlen(server_reply), 0);
    printf("Message envoyé : %s.\n\n", server_reply);
}

void process_reply_server(char *client_message, int client_sock, char *pseudo) {
    if(client_message == NULL) {
        int size = (strlen("DUNNO***"));
        char message[size];
        snprintf(message, size, "DUNNO***");
        send_request(message, client_sock);
    } else {
        char request[6];
        strncpy(request, client_message, 5);
        request[5] = '\0';

        if(strcmp(request, "NEWPL") == 0) {
            req_NEWPL(client_message, request, client_sock);
        } else if(strcmp(request, "REGIS") == 0) {
            req_REGIS(client_message, request, client_sock);
        } else if(strcmp(request, "UNREG") == 0) {
            req_UNREG(client_message, pseudo, client_sock);
        } else if(strcmp(request, "SIZE?") == 0) {
            req_SIZE(client_message, request, client_sock);
        } else if(strcmp(request, "LIST?") == 0) {
            req_LIST(client_message, request, client_sock);
        } else if(strcmp(request, "GAME?") == 0) {
            req_GAME(client_message, request, client_sock);
        } else if(strcmp(request, "START") == 0) {
            req_START(pseudo, client_sock);
            printf("Le joueur %s est prêt à jouer !\n", pseudo);
        } else if(strcmp(request, "IQUIT") == 0) {
            req_IQUIT(client_sock);
        } else if((strcmp(request,"DOMOV") == 0) || (strcmp(request,"UPMOV") == 0) || (strcmp(request,"LEMOV") == 0) || (strcmp(request,"RIMOV") == 0)) {
            liste_de_joueurs *tmp = rechercher_un_joueur(pseudo, jc);
            liste_de_parties *tmp_2 = rechercher_une_partie(pc, tmp->m);

            req_deplacement(client_message, tmp_2->p, tmp->j, client_sock);
            ser_END(pseudo, client_sock);
        } else if(strcmp(request, "GLIS?") == 0) {
            req_GLIS(pseudo, client_sock);
        } else if(strcmp(request, "MALL?") == 0) {
            req_MALL(client_message, request, client_sock, pseudo);
        } else if(strcmp(request, "SEND?") == 0) {
            req_SEND(client_message, request, pseudo, client_sock);
        } else {
            int size = (strlen("DUNNO***"));
            char message[size];
            snprintf(message, size, "DUNNO***");
            send_request(message, client_sock);
        }
    }
}



/*  PREPARE LA REPONSE DU SERVEUR EN FONCTION DU MESSAGE DU CLIENT  *********************************************/

void req_NEWPL(char *client_message, char *request, int client_sock) {
    if(n + 1 >= 255) {
        int size = strlen("REGNO***");
        char message[size];
        snprintf(message, size, "REGNO***");
        send_request(message, client_sock);
    } else {
        n += 1;
        char pseudo[8];
        int portUDP;
        sscanf(client_message, "%s %s %d", request, pseudo, &portUDP);
        joueur *j = initialise_un_joueur(pseudo, portUDP, client_sock);
        jc = ajouter_un_joueur(j, jc, n);
        partie *p = initialise_une_partie(n);
        pc = ajouter_une_partie(p, pc);

        int size = (strlen("REGOK***") + 3);
        char message[size];
        snprintf(message, size, "REGOK %d***", n);
        send_request(message, client_sock);
    }
}

void req_REGIS(char *client_message, char *request, int client_sock) {
    char pseudo[8];
    int portUDP;
    int nb_partie;
    sscanf(client_message, "%s %s %d %d", request, pseudo, &portUDP, &nb_partie);
    if(nb_partie > n || nb_partie < 0) {
        int size = (strlen("REGNO***"));
        char message[size];
        snprintf(message, size, "REGNO***");
        send_request(message, client_sock);
    } else {
        liste_de_parties *tmp = rechercher_une_partie(pc, nb_partie);
        if(tmp->p->joueurs_inscrits + 1 > 4) {
            int size = (strlen("REGNO***"));
            char message[size];
            snprintf(message, size, "REGNO***");
            send_request(message, client_sock);
        } else {
            joueur *j = initialise_un_joueur(pseudo, portUDP, client_sock);
            jc = ajouter_un_joueur(j, jc, nb_partie);
            tmp->p->joueurs_inscrits += 1;
            printf("%s a rejoint la partie %d.\n", pseudo, nb_partie);
            int size = (strlen("REGOK***") + 3);
            char message[size];
            snprintf(message, size, "REGOK %d***", nb_partie);
            send_request(message, client_sock);
        }
    }
}

void req_UNREG(char *client_message, char *pseudo, int client_sock) {
    liste_de_joueurs *tmp = rechercher_un_joueur(pseudo, jc);
    liste_de_parties *tmp_2 = rechercher_une_partie(pc, tmp->m);
    tmp_2->p->joueurs_inscrits--;
    tmp->m = 0;
    int size = (strlen("UNROK***"));
    char message[size];
    snprintf(message, size, "UNROK***");
    send_request(message, client_sock);
}


void req_SIZE(char *client_message, char *request, int client_sock) {
    int nb_partie;
    sscanf(client_message, "%s %d", request, &nb_partie);
    if(nb_partie > n || nb_partie <= 0) {
        int size = (strlen("DUNNO***"));
        char message[size];
        snprintf(message, size, "DUNNO***");
        send_request(message, client_sock);
    } else {
        int size = (strlen("SIZE!   ***)") + 10);
        char *server_reply = malloc(sizeof(char) * size + 1);
        snprintf(server_reply, size, "SIZE! %d %u %u***", nb_partie,htole16(H),htole16(W));
        send_request(server_reply, client_sock);
        free(server_reply);
    }
}

void req_LIST(char *client_message, char *request, int client_sock) {
    int nb_partie;
    sscanf(client_message, "%s %d", request, &nb_partie);
    if(nb_partie > n || nb_partie <= 0) {
        int size = (strlen("DUNNO***"));
        char message[size];
        snprintf(message, size, "DUNNO***");
        send_request(message, client_sock);
    } else {
        liste_de_parties *tmp = rechercher_une_partie(pc, nb_partie);

        int size = (strlen("LIST!  ***)") + 6) + (strlen("PLAYR ***") + 8)*tmp->p->joueurs_inscrits;
        char *message = malloc(sizeof(char) * size);
        sprintf(message, "LIST! %d %d***", nb_partie, tmp->p->joueurs_inscrits);

        liste_de_joueurs *tmp_2 = jc;
        while(tmp_2 != NULL) {
            if(tmp_2->m == nb_partie) {
                char loop[strlen("PLAYR ***") + 8];
                snprintf(loop, strlen("PLAYR ***") + 8, "PLAYR %s***", tmp_2->j->pseudo);
                strcat(message, loop);
            }
            tmp_2 = tmp_2->suivant;
        }

        send_request(message, client_sock);
        free(message);
    }
}

void req_GAME(char *client_message, char *request, int client_sock) {
    int nb_partie = compte_parties_non_commencees(pc);

    if(nb_partie == 0) {
        int size = strlen("GAME! ***") + 3;
        char message[size];
        snprintf(message, size, "GAME! %d***", nb_partie);
        send_request(message, client_sock);
    } else {
        int size = (strlen("OGAME  ***") + 6)*nb_partie + strlen("GAME! ***") + 3;

        char *message = malloc(sizeof(char) * size);
        sprintf(message, "GAME! %d***", nb_partie);

        liste_de_parties *tmp = pc;
        while(tmp != NULL) {
            if((tmp->p->joueurs_inscrits) < MAX_JOUEURS) {
                char loop[strlen("OGAME  ***") + 6];
                snprintf(loop, strlen("OGAME  ***") + 6, "OGAME %d %d***", tmp->p->m, tmp->p->joueurs_inscrits);
                strcat(message, loop);
            }
            tmp = tmp ->suivant;
        }

        send_request(message, client_sock);
        free(message);
    }
}

void req_START(char *pseudo, int client_sock) {
    liste_de_joueurs *tmp = rechercher_un_joueur(pseudo, jc);
    liste_de_parties *tmp_2 = rechercher_une_partie(pc, tmp->m);
    tmp->pret += 1;
    int size = 9 + 15 + 4 + strlen("WELCOEEEEEE***");
    char message[size];
    snprintf(message, size, "WELCO %d %u %u %d %s %s***", tmp->m, htole16(H),htole16(W), F, tmp_2->p->adresse, tmp_2->p->port);

    if(tlm_est_pret(tmp_2, jc) == 1) {
        liste_de_joueurs *tmp_3 = jc;
        while(tmp_3 != NULL) {
            if(tmp_3->m == tmp_2->p->m) {
                char *submes = req_POST(tmp_3->j->pseudo);
                printf("hyufegzgeu%s\n\n", submes);
                char *res = malloc(sizeof(char) * (strlen(submes) + size));
                sprintf(res, "%s%s", message, submes);
                send_request(res, tmp_3->j->numeroSocket);
                free(submes);
                free(res);
            }
            tmp_3 = tmp_3->suivant;
        }

        print_labyrinth(tmp_2->p->tab, H, W);
    }
}

char *req_POST(char *pseudo) {
    liste_de_joueurs *tmp = rechercher_un_joueur(pseudo, jc);
    liste_de_parties *tmp_2 = rechercher_une_partie(pc, tmp->m);
    int size = 8 + 3 + 3 + strlen("POST   ***");
    char *message = malloc(sizeof(char) * size);

    srand(time(NULL));
    int x_alea = rand() % (H);
    int y_alea = rand() % (W);
    while(tmp_2->p->tab[x_alea][y_alea] != 0) {
        x_alea = rand() % (H);
        y_alea = rand() % (W);
    }
    tmp_2->p->tab[x_alea][y_alea] = tmp->j->numeroSocket;

    char *des = malloc(sizeof(char) *3);
    addZero(des, x_alea, 3, tmp->j->PosX);
    free(des);

    des = malloc(sizeof(char) *3);
    addZero(des, y_alea, 3, tmp->j->PosY);
    free(des);

    snprintf(message, size, "POSIT %s %s %s***", tmp->j->pseudo, tmp->j->PosX, tmp->j->PosY);

    return message;
}

void req_deplacement(char *client_message, partie *partie, joueur *joueur, int client_sock) {
    int bool = rand()%2;
    if(bool == 0) {
        ser_GHOST(partie, joueur->pseudo);
    }

    int i = 1; // curseur
    char *dep=malloc(sizeof(char)*3); // deplacement
    int bonus = 0;
    char request[6];
    sscanf(client_message, "%s %s", request, dep);
    int d = atoi(dep);
    int X = atoi(joueur->PosX);
    int Y = atoi(joueur->PosY);

    int newX = X;
    int newY = Y;

    if(strcmp(request, "UPMOV") == 0) {
        while((partie->tab[X-i][Y] == 0 || partie->tab[X-i][Y] == 2) && i <= d && (X-i) < H && (X-i) >= 0) {
            if(partie->tab[X-i][Y] == 2) {
                partie->tab[X-i][Y] = 0;
                bonus = atoi(joueur->p)+100;
                char *des = malloc(sizeof(char) * 4);
                addZero(des, bonus, 4, joueur->p);
                free(des);

                char *des1 = malloc(sizeof(char) * 3);
                addZero(des1, (X-i), 3, joueur->PosX);
                free(des1);

                char *des2 = malloc(sizeof(char) * 3);
                addZero(des2, Y, 3, joueur->PosY);
                free(des2);

                ser_SCORE(joueur);
                partie->f_a += 1;
            }
            i++;
        }
        newX = X-(i-1);
    } else if(strcmp(request, "DOMOV") == 0) {
        while((partie->tab[X+i][Y] == 0 || partie->tab[X+i][Y] == 2) && i <= d && (X+i) < H && (X+i) >= 0) {
            if(partie->tab[X+i][Y] == 2) {
                partie->tab[X+i][Y] = 0;
                bonus = atoi(joueur->p)+100;
                char *des = malloc(sizeof(char) * 4);
                addZero(des, bonus, 4, joueur->p);
                free(des);

                char *des1 = malloc(sizeof(char) * 3);
                addZero(des1, (X+i), 3, joueur->PosX);
                free(des1);

                char *des2 = malloc(sizeof(char) * 3);
                addZero(des2, Y, 3, joueur->PosY);
                free(des2);

                ser_SCORE(joueur);

                partie->f_a += 1;
            }
            i++;
        }
        newX = X+(i-1);
    } else if(strcmp(request, "LEMOV") == 0) {
        while((partie->tab[X][Y-i] == 0 || partie->tab[X][Y-i] == 2) && i<=d && (Y-i) < W && (Y-i) >= 0) {
            if(partie->tab[X][Y-i] == 2) {
                partie->tab[X][Y-i] = 0;
                bonus = atoi(joueur->p)+100;
                char *des = malloc(sizeof(char) * 4);
                addZero(des, bonus, 4, joueur->p);
                free(des);

                char *des1 = malloc(sizeof(char) * 3);
                addZero(des1, X, 3, joueur->PosX);
                free(des1);

                char *des2 = malloc(sizeof(char) * 3);
                addZero(des2, Y-i, 3, joueur->PosY);
                free(des2);

                ser_SCORE(joueur);

                partie->f_a += 1;
            }
            i++;
        }
        newY= Y-(i-1);
    }else if(strcmp(request, "RIMOV") == 0) {
      printf("------------------tzst2\n");
        while((partie->tab[X][Y+i] == 0 || partie->tab[X][Y+i] == 2) && i <= d && (Y+i) < W && (Y+i) >= 0) {
            if(partie->tab[X][Y+i] == 2) {
                partie->tab[X][Y+i] = 0;
                bonus = atoi(joueur->p)+100;
                char *des = malloc(sizeof(char) * 4);
                addZero(des, bonus, 4, joueur->p);
                free(des);

                char *des1 = malloc(sizeof(char) * 3);
                addZero(des1, X, 3, joueur->PosX);
                free(des1);

                char *des2 = malloc(sizeof(char) * 3);
                addZero(des2, Y+i, 3, joueur->PosY);
                free(des2);

                ser_SCORE(joueur);

                partie->f_a += 1;
            }
            i++;
        }
        newY = Y+(i-1);
    }

    partie->tab[X][Y] = 0;
    partie->tab[newX][newY] = joueur->numeroSocket;

    char *dest = malloc(sizeof(char) * 3);
    addZero(dest, newY, 3, joueur->PosY);
    free(dest);

    dest = malloc(sizeof(char) * 3);
    addZero(dest, newX, 3, joueur->PosX);
    free(dest);

    print_labyrinth(partie->tab, H, W);
    req_deplacement_2(joueur, bonus, client_sock);
}

void req_deplacement_2(joueur* joueur, int bonus, int client_sock) {
  int size = strlen("MOVE!  ***") + 6;
  int size1 =strlen("MOVEF   ***") + 9;
  if(bonus > 0) {
        char message[size1];
        snprintf(message, size1, "MOVEF %s %s %s***", joueur->PosX, joueur->PosY, joueur->p);
        send_request(message, client_sock);
  } else {
        char message[size];
        snprintf(message, size, "MOVE! %s %s***", joueur->PosX, joueur->PosY);
        send_request(message, client_sock);
    }
}

void req_GLIS(char *pseudo, int client_sock) {
    liste_de_joueurs *jo = rechercher_un_joueur(pseudo, jc);
    liste_de_parties *tmp = rechercher_une_partie(pc, jo->m);

    int size = (strlen("GLIS! ***)") + 3) + (strlen("GPLYR    ***") + 8 + 8 + 5)*tmp->p->joueurs_inscrits;
    char *message = malloc(sizeof(char) * size);
    sprintf(message, "GLIS! %d***", tmp->p->joueurs_inscrits);

    liste_de_joueurs *tmp_2 = jc;
    while(tmp_2 != NULL) {
        if(tmp_2->m == jo->m) {
            char loop[strlen("GPLYR    ***") + 8 + 8 + 5];
            snprintf(loop, strlen("GPLYR    ***") + 8 + 8 + 5, "GPLYR %s %s %s %s***", tmp_2->j->pseudo, tmp_2->j->PosX, tmp_2->j->PosY, tmp_2->j->p);
            strcat(message, loop);
        }
        tmp_2 = tmp_2->suivant;
    }

    send_request(message, client_sock);
    free(message);
}

void req_MALL(char *client_message, char *request, int client_sock, char *pseudo) {
   char t[200];
   int taille1=strlen(client_message+5)-3;
   char s[200];
   strcpy(t,"MESSA ");
    strcat(t, " " );
    strcat(t, pseudo);
    strcat(t," ");
    memcpy(s,client_message+5,taille1);
    s[taille1]='\0';

    strcat(t,s);
    strcat(t,"++++");

    int size = strlen("MALL!***")+1;
    char message[size];
    snprintf(message, size, "MALL!***");
    pthread_t th3;
    messageUDP *mu = initialise_un_message_UDP(pseudo, t, 0);


    pthread_create(&th3, NULL, &communication_Multi, (void *) mu);
    pthread_join(th3, NULL);
    free(mu);

    send_request(message, client_sock);
}

void req_SEND(char *client_message, char *request, char *pseudo, int client_sock) {


char t[200];
int taille1=strlen(client_message+13)-3;
char s[200];
strcpy(t,"MESSP ");
strcat(t, pseudo);
strcat(t," ");
memcpy(s,client_message+13,taille1);
 s[taille1]='\0';

 strcat(t,s);
 strcat(t,"++++");

   char dest[8];
    int taille = strlen(client_message+13)-3;
    char mess[taille];
    sscanf(client_message, "%s %s", request, dest);
    memcpy(mess, client_message+13, taille);

    liste_de_joueurs *src = rechercher_un_joueur(pseudo, jc);
    liste_de_joueurs *ja = rechercher_un_joueur(dest, jc);
    if(ja == NULL || ja->m != src->m) {
        int size = strlen("NSEND***")+1;
        char res[size];
        snprintf(res, size, "NSEND***");
        send_request(res, client_sock);
    } else {
        pthread_t th3;

      /*  int sizeUDP = strlen("MESSP  +++") + strlen(pseudo) + strlen(mess);
        char messUDP[sizeUDP];
        snprintf(messUDP, sizeUDP, "MESSP %s %s+++", pseudo, mess);
        printf("******le message est %s\n",mess);*/

        messageUDP *mu = initialise_un_message_UDP(pseudo, t, ja->j->portUDP);

        pthread_create(&th3, NULL, &communication_UDP, (void *) mu);
        pthread_join(th3, NULL);
        free(mu);

        int size = strlen("SEND!***")+1;
        char res[size];
        snprintf(res, size, "SEND!***");
        send_request(res, client_sock);
    }
}

void req_IQUIT(int client_sock) {
    int size = 9;
    char message[size];
    snprintf(message, size, "GOBYE***");
    message[size-1] = '\0';
    send_request(message, client_sock);
}

void ser_SCORE(joueur *j) {
    int size = strlen("SCORE    +++") + 8 + 8 + 5;
    char mess[size];
    snprintf(mess, size, "SCORE %s %s %s %s+++", j->pseudo, j->p, j->PosX, j->PosY);

    pthread_t th3;
    messageUDP *mu = initialise_un_message_UDP(j->pseudo, mess, 0);

    pthread_create(&th3, NULL, &communication_Multi, (void *) mu);
    pthread_join(th3, NULL);
    free(mu);
}

void ser_GHOST(partie *p, char *pseudo) {
    for(int i=0; i<H; i++) {
        for(int j=0; j<W; j++) {
            if(p->tab[i][j] == 2) {
                int x_alea = rand()%H;
                int y_alea = rand()%W;
                while(p->tab[x_alea][y_alea] != 0) {
                    x_alea = rand()%H;
                    y_alea = rand()%W;
                }
                p->tab[i][j] = 0;
                p->tab[x_alea][y_alea] = 2;

                int size = strlen("GHOST  +++") + 8;

                char posX[4];
                char *des1 = malloc(sizeof(char) * 3);
                addZero(des1, x_alea, 3, posX);
                free(des1);

                char posY[4];
                char *des2 = malloc(sizeof(char) * 3);
                addZero(des2, y_alea, 3, posY);
                free(des2);

                char mess[size];
                snprintf(mess, size, "GHOST %s %s+++", posX, posY);

                pthread_t th3;

                messageUDP *mu = initialise_un_message_UDP(pseudo, mess, 0);

                pthread_create(&th3, NULL, &communication_Multi, (void *) mu);
                pthread_join(th3, NULL);
                free(mu);
                return;
            }
        }
    }
}

void ser_END(char *pseudo, int client_sock) {
    liste_de_joueurs *ja = rechercher_un_joueur(pseudo, jc);
    liste_de_parties *pa = rechercher_une_partie(pc, ja->m);

    if(pa->p->f_a == F) {
        char id_gagnant[9];
        id_gagnant[8] = '\0';
        int max = 0;
        ja = jc;
        while(ja != NULL) {
            if(ja->m == pa->p->m) {
                if(atoi(ja->j->p) > max) {
                    max = atoi(ja->j->p);
                    memmove(id_gagnant, ja->j->pseudo, 8);
                }
            }
            ja = ja->suivant;
        }

        ja = rechercher_un_joueur(id_gagnant, jc);

        int size = strlen("ENDGA  +++") + 8 + 5;
        char mess[size];
        snprintf(mess, size, "ENDGA %s %s+++", id_gagnant, ja->j->p);

        messageUDP *mu = initialise_un_message_UDP(pseudo, mess, 0);

        pthread_t th3;
        pthread_create(&th3, NULL, &communication_Multi, (void *) mu);
        pthread_join(th3, NULL);
        free(mu);
    }
}



/*  MULTIDIFFUSION ET ENVOI EN UDP  *****************************************************************************/

void *communication_Multi(void *a) {
    messageUDP *udp = (messageUDP *) a;

    liste_de_joueurs *jo = rechercher_un_joueur(udp->pseudo, jc);
    liste_de_parties *pa = rechercher_une_partie(pc, jo->m);

    int sock = socket(PF_INET, SOCK_DGRAM, 0);
    struct addrinfo *first_info;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    int r = getaddrinfo(pa->p->adresse, pa->p->port, &hints, &first_info);
    if(r == 0) {
        struct addrinfo *info = first_info;
        if(info != NULL) {
            struct sockaddr *saddr = info->ai_addr;
            info = info->ai_next;
            sendto(sock, udp->mes, strlen(udp->mes), 0, saddr, (socklen_t)sizeof(struct sockaddr_in));
            printf("\nMessage envoyé (en UDP - Multidifusion) : %s.\n", udp->mes);
        }
    }

    close(sock);
    return NULL;
}

void *communication_UDP(void *a) {
    messageUDP *udp = (messageUDP *) a;

    int sock = socket(PF_INET, SOCK_DGRAM, 0);
    struct addrinfo *first_info;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype=SOCK_DGRAM;

    char *port = malloc(sizeof(char) * 5);
    snprintf(port, 5, "%d", udp->portUDP);

    int r = getaddrinfo("localhost", port, &hints, &first_info);
    if(r == 0) {
        struct sockaddr *saddr=first_info->ai_addr;
        if(first_info != NULL) {
            sendto(sock, udp->mes, strlen(udp->mes), 0, saddr, (socklen_t)sizeof(struct sockaddr_in));
            printf("\nMessage envoyé (en UDP) : %s.\n", udp->mes);
        }
    }
    close(sock);
    return NULL;
}
/* Respect des types demandés *****************************************************************************************/
void addZero(char *des, int nb, int nb_octets,char *res) {
  int length = sprintf(des, "%d", nb);
  int n = nb_octets - length;
  if(n>0) {
    strcpy(res,"0");
    for(int j=0;j<n-1;j++)
    {
		strcat(res,"0");
	}


 strcat(res,des);
 res[nb_octets]='\0';
 }
 }
