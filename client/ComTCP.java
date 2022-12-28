import java.io.*;
import java.net.Socket;
import java.util.Scanner;

public class ComTCP extends Thread {

    String pseudo; // Identifiant du client (8 caractères)
    int portTCP; // Port TCP pour se connecter au serveur
    ComUDP udp; // Communication UDP du client
    int portUDP; // Port UDP du client

    int[][] jeu; // Jeu du plateau
    int X = 0; // Position X du joueur
    int Y = 0; // Position Y du plateau
    int D = 0; // Déplacement du joueur


    public ComTCP(String pseudo, int portTCP, ComUDP udp) {
        this.pseudo = pseudo;
        this.portTCP = portTCP;
        this.udp = udp;

        this.portUDP = udp.portUDP;
        this.jeu = new int[10][20];
    }



    /*  AFFICHER LE JEU ET DEPLACER LE CLIENT *******************************************************************/

    /* Affiche le plateau de jeu */
    public void afficher_jeu() {
        for(int i=0; i<jeu.length; i++) {
            for(int j=0; j<jeu[i].length; j++) {
                switch(jeu[i][j]) {
                    case 0: System.out.print("⬜  "); break;
                    case 1: System.out.print("⬛  "); break;
                    case 2: System.out.print("🟥  "); break; // Fantôme
                    default: System.out.print("🟦  "); break; // Joueurs
                }
            }
            System.out.println("\n");
        }
        System.out.println();
    }

    /* Colorie le chemin que prend le client */
    public void colorier_jeu(int newX, int newY) {
        if(D == 8) {
            for(int y=Y; y<=newY; y++) {
                jeu[X][y] = 0;
            }
        } else if(D == 9) {
            for(int y=Y; y>=newY; y--) {
                jeu[X][y] = 0;
            }
        } else if(D == 10) {
            for(int x=X; x>=newX; x--) {
                jeu[x][Y] = 0;
            }
        } else if(D == 11) {
            for(int x=X; x<=newX; x++) {
                jeu[x][Y] = 0;
            }
        }
        jeu[newX][newY] = 3;
    }



    /*  ENVOYER ET RECEVOIR DES MESSAGES DU SERVEUR *************************************************************/

    /* Retourne le message du serveur dans la socket */
    public char[] read_reply_server(Socket socket) {
        try {
            InputStream sin = socket.getInputStream();
            byte[] data = new byte[1024];
            int c = sin.read(data);
            char[] message = new char[c];
            for(int i=0; i<c; i++) {
                message[i] = (char) data[i];
            }
            return message;
        } catch (Exception e) {
            e.printStackTrace();
        }
        return null;
    }

    /* Envoie le message du client dans la socket */
    public void send_request(String message, Socket socket) {
        try {
            BufferedReader reader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            PrintWriter pw = new PrintWriter(new OutputStreamWriter(socket.getOutputStream()));
            pw.write(message);
            pw.flush();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /* Affiche la réponse du serveur */
    public void afficher_rep_server(String message) {
        String request[] = message.split(" ");

        switch(request[0]) {
            case "OGAME" : System.out.println("Partie " + request[1] + " (" + request[2] + "/2)"); break;
            case "REGOK" : System.out.println("Vous êtes inscrit à la partie " + request[1]); break;
            case "REGNO" : System.out.println("Erreur : vous ne pouvez pas vous inscrire à cette partie."); break;
            case "UNROK" : System.out.println("Vous êtes bien désinscris à la partie."); break;
            case "SIZE!" : System.out.println("La partie " + request[1] + " a une taille de " + request[2] + "x" + request[3] + "."); break;
            case "PLAYR" : System.out.println("Joueur " + request[1] + "."); break;
            case "WELCO" :
                System.out.println("Tous les joueurs sont prêts. La partie " + request[1] + " va démarrer.");
                System.out.println("Le plateau a une taille " + request[2] + "x" + request[3] + ".");
                System.out.println("Il y a " + request[4] + " fantômes à trouver.");
                RecUDP rec = new RecUDP(request[6], request[5]);
                rec.start();
                break;
            case "POSIT" :
                System.out.println("Vous êtes sur la case " + request[2] + "x" + request[3] + ".\n");
                X = Integer.parseInt(request[2]);
                Y = Integer.parseInt(request[3]);
                jeu[X][Y] = 3;
                break;
            case "MOVEF" :
                System.out.println("Votre nouvelle position est " + request[1] + "x" + request[2] + ".");
                System.out.println("Bravo ! Vous avez capturé un fantôme !");
                System.out.println("Votre score est : " + request[3] + " points.\n\n");
                colorier_jeu(Integer.parseInt(request[1]), Integer.parseInt(request[2]));
                X = Integer.parseInt(request[1]);
                Y = Integer.parseInt(request[2]);
                break;
            case "MOVE!" :
                System.out.println("Votre nouvelle position est " + request[1] + "x" + request[2] + ".\n\n");
                colorier_jeu(Integer.parseInt(request[1]), Integer.parseInt(request[2]));
                X = Integer.parseInt(request[1]);
                Y = Integer.parseInt(request[2]);
                break;
            case "GPLYR" :
                System.out.println(request[1] + " (" + request[2] + ", " + request[3] + ") : " + request[4] + " points.");
                break;
            case "SEND!" : System.out.println("\nVotre message a bien été envoyé !"); break;
            case "NSEND" : System.out.println("Erreur : le message n'a pas pu etre envoyé !"); break;
            case "GOBYE" : System.out.println("\nMerci d'avoir joué ! À bientot !"); break;
            default : break;
        }
    }

    /* Affiche les options disponibles du client */
    public void afficheOption(int etape) {
        System.out.println("\nQue voulez-vous faire ?");
        if(etape == 1) {
            System.out.println("(1) Rejoindre une partie.");
            System.out.println("(2) Créer une nouvelle partie.");
            System.out.println("(4) Demander la taille d'une partie.");
            System.out.println("(5) Demander la liste des joueurs d'une partie.");
            System.out.println("(6) Demander la liste des parties qui n'ont pas commencé.");
        } else if(etape == 2) {
            System.out.println("(3) Se désinscrire de la partie.");
            System.out.println("(4) Demander la taille d'une partie.");
            System.out.println("(5) Demander la liste des joueurs d'une partie.");
            System.out.println("(6) Demander la liste des parties qui n'ont pas commencé.");
            System.out.println("(7) Commencer une partie.");              
        } else {
            System.out.println("(8) Aller à droite.");
            System.out.println("(9) Aller à gauche.");
            System.out.println("(10) Aller vers le haut.");
            System.out.println("(11) Aller vers le bas.");
            System.out.println("(12) Voir la liste des joueurs.");
            System.out.println("(13) Envoyer un message à tous les joueurs.");
            System.out.println("(14) Envoyer un message à un joueur en particulier.");
            System.out.println("(15) Voir le plateau de jeu.");
            System.out.println("(16) Quitter la partie.");
        }
    }

    /* Retourne le message du client */
    public String demandeOption(int i) {
        afficheOption(i);
        Scanner sc = new Scanner(System.in);
        int choix = sc.nextInt();
        sc.nextLine();

        switch(choix) {
            case 1:
                System.out.println("\nQuelle partie voulez-vous rejoindre ?");
                choix = sc.nextInt();
                return ("REGIS" + " " + pseudo + " " + portUDP + " " + choix + "***");
            case 2:
                return ("NEWPL " + pseudo + " " + portUDP + "***");
            case 3:
                return ("UNREG***");
            case 4:
                System.out.println("\nDe quelle partie voulez-vous connaître la taille ?");
                choix = sc.nextInt();
                return ("SIZE?" + " " + choix + "***");
            case 5:
                System.out.println("\nDe quelle partie voulez-vous connaître la liste des joueurs inscrits ?");
                choix = sc.nextInt();
                return ("LIST?" + " " + choix + "***");
            case 6:
                return ("GAME?***");
            case 7:
                System.out.println("\nBravo ! Vous allez bientôt pouvoir jouer. Patientez...");
                return ("START***");
            case 8 :
                System.out.println("\nA quelle distance (droite) ?");
                D = 8;
                choix = sc.nextInt();
                return("RIMOV "+ choix + "***");
            case 9 :
                System.out.println("\nA quelle distance (gauche) ?");
                D = 9;
                choix = sc.nextInt();
                return("LEMOV " + choix + "***");
            case 10 :
                D = 10;
                System.out.println("\nA quelle distance (haut) ?");
                choix = sc.nextInt();
                return("UPMOV " + choix + "***");
            case 11 :
                D = 11;
                System.out.println("\nA quelle distance (bas) ?");
                choix = sc.nextInt();
                return("DOMOV " + choix + "***");
            case 12 :
                return ("GLIS?***");
            case 13 :
                System.out.println("\nQue voulez-vous envoyer ?");
                String mess = sc.nextLine();
                return ("MALL? " + mess + "***");
            case 14 : 
                System.out.println("\nA qui ?");
                String destinataire = sc.nextLine();
                System.out.println("\nQuel est votre message ?");
                String messagee = sc.nextLine();
                return ("SEND? "+ destinataire + " " + messagee + "***");
            case 15:
                afficher_jeu();
                return demandeOption(3);
            case 16:
                return ("IQUIT***");
            
            default: return "";
        }
    }



    /*  COMMUNICATION ENTRE LE CLIENT TCP ET UDP  ***************************************************************/

    /* Gestion de la communication avec le serveur */
    public void communication() {
        try {
            Socket socket = new Socket("localhost", portTCP);

            /* Récupération des messages GAMES et OGAME */
            String server_message = String.valueOf(read_reply_server(socket));
            String request[] = server_message.split(("\\*+"));
            for(int i=0; i<request.length; i++) {
                afficher_rep_server(request[i]);
            }

            int e = 1;
            String req = demandeOption(e);

            while(true) {
                send_request(req, socket);
                
                server_message = String.valueOf(read_reply_server(socket));
                String request_2[] = server_message.split(("\\*+"));
                for(int i=0; i<request_2.length; i++) {
                    afficher_rep_server(request_2[i]);
                }

                if(req.substring(0, 5).equals("NEWPL") || req.substring(0, 5).equals("REGIS")) {
                    e = 2;
                } else if(req.substring(0, 5).equals("UNREG")) {
                    e = 1;
                } else if(req.substring(0, 5).equals("START")) {
                    e = 3;
                } else if(req.substring(0, 5).equals("IQUIT")) {
                    socket.close();
                    System.exit(0);
                    return;
                }

                req = demandeOption(e);
            }
        } catch(Exception e) {
            e.printStackTrace();
        }
    }

    /* Tourne le programme du client */
    public void run() {
        try {
            for(int i=0; i<10; i++) {
                for(int j=0; j<20; j++) {
                    jeu[i][j] = 1;
                }
            }

            communication();
        } catch(NumberFormatException ex) {
            ex.printStackTrace();
        }
    }
}