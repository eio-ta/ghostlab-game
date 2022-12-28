import java.io.*;
import java.net.Socket;
import java.util.Scanner;

public class ClientComTCP extends Thread {
    int port; // le port TCP du serveur
    Joueur client = null;
    int[][] jeu;

    public ClientComTCP(int port, Joueur client) {
        this.client = client;
        this.port = port;
        this.jeu = new int[10][20];
    }









    public static void afficher_jeu(int jeu[][]) {
        for(int i=0; i<10; i++) {
            for(int j=0; j<20; j++) {
                switch(jeu[i][j]) {
                    case 0: System.out.print("⬜  "); break;
                    case 1: System.out.print("⬛  "); break;
                    case 2: System.out.print("🟥  "); break; // Fantôme
                    default: System.out.print("🟦  "); break; // Joueurs
                }
            }
            System.out.println("\n");
        }
    }




















    /*  LIRE ET AFFICHER LA REPONSE DU SERVEUR  *****************************************************************/

    public static char[] read_reply_server(Socket socket) {
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

    public static void print_reply(String message, int jeu[][]) {
        String request[] = message.split(" ");


        switch(request[0]) {
            case "GAMES" : System.out.println("Il y a " + request[1] + " partie(s)."); break;
            case "OGAME" : System.out.println("Partie " + request[1] + " (" + request[2] + "/4)"); break;
            case "REGOK" : System.out.println("Vous êtes inscrit à la partie " + request[1]); break;
            case "REGNO" : System.out.println("Erreur : vous ne pouvez pas vous inscrire à cette partie."); break;
            case "UNROK" : System.out.println("Vous êtes bien désinscris à la partie."); break;
            case "SIZE!" : System.out.println("La partie " + request[1] + " a une taille de " + request[2] + "x" + request[3] + "."); break;
            case "LIST!" : System.out.println("Il y a " + request[2] + " joueur(s) dans la partie " + request[1] + "."); break;
            case "PLAYR" : System.out.println("Joueur" + request[1] + "."); break;
            case "GAME!" : System.out.println("Il y a " + request[1] + " partie(s) non commencée(s).");
            case "WELCO" :
                System.out.println("Tous les joueurs sont prêts. La partie " + request[1] + " va démarrer.");
                System.out.println("Le plateau a une taille " + request[2] + "x" + request[3] + ".");
                System.out.println("Il y a " + request[4] + " fantômes à trouver.");
                break;
            case "POSIT" :
                System.out.println("Vous êtes sur la case " + request[2] + "x" + request[3] + ".\n");

                jeu[Integer.parseInt(request[2])][Integer.parseInt(request[3])] = 3;
                afficher_jeu(jeu);
                break;
            case "SEND!" :
                    System.out.println("Votre message a bien été envoyé !") ;
                    break;
            case "NSEND" :
                     System.out.println("Il y  a eu une erreur, le message \n n'a pas pu etre envoyé !");
                     break;
            case "MOVEF" :
                System.out.println(" Votre nouvelle position : x= " + request[1] + "y = " + request[2]
                + "\nBravo ! Vous avez capturé un fonctome ! "

                +"\n Votre score est : p= "+ request[3]);
                break;

            case "MOVE!" :
                 System.out.println(" Votre nouvelle position : x= " + request[1] + "y = " + request[2]);
                 break;

             case "GOBYE" :
                System.out.println("\nMerci d'avoir joué ! À bientot !")  ;
                break;

             case "MALL!" :
                System.out.println("\nVotre message a bien été diffusé !");
                break;


            default : break;
        }

    }


    /*  ENVOYER LA REQUETE AU SERVEUR  **************************************************************************/

    public static void print_request_client(int etape) {
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

        }
    }

    public static String choose_request_client(Joueur joueur, int etape) {
        print_request_client(etape);
        Scanner sc = new Scanner(System.in);
        int choix = sc.nextInt();
        sc.nextLine();

        switch(choix) {
            case 1:
                System.out.println("\nQuelle partie voulez-vous rejoindre ?");
                choix = sc.nextInt();
                return ("REGIS" + " " + joueur.getPseudo() + " " + Integer.toString(joueur.getPortUDP()) + " " + choix + "***");
            case 2:
                return ("NEWPL " + joueur.getPseudo() + " " + Integer.toString(joueur.getPortUDP()) + "***");
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
                System.out.println("\nQuelle la distance à parcourie à droite ?");
                choix=sc.nextInt();
                return("RIMOV "+choix+"***");

            case 9 :
                System.out.println("\nQuelle la distance à parcourie à gauche ?");
                choix=sc.nextInt();
                return("LEMOV "+choix+"***");

            case 10 :
                System.out.println("\nQuelle la distance à parcourie en haut ?");
                choix=sc.nextInt();
                return("UPMOV "+choix+"***");

            case 11 :
                System.out.println("\nQuelle la distance à parcourie en bas ?");
                choix=sc.nextInt();
                return("DOMOV "+choix+"***");
            case 13 :
             System.out.println("\n Quel message voulez vous envoyer à tout le monde?");
              String msge=sc.nextLine();
              return("MALL? "+msge+"***");
            case 14 :
                System.out.println("\nA quel joueur voulez vous envoyer le message ?");
                String destinataire = sc.nextLine();
                System.out.println("\nQuel est votre message ?");
                String messagee = sc.nextLine();
                return ("SEND? "+ destinataire +" "+messagee+"***");
            default: return "";
        }
    }

    public static void send_request(String message, int port, Socket socket) {
        try {
            BufferedReader reader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            PrintWriter pw = new PrintWriter(new OutputStreamWriter(socket.getOutputStream()));
            pw.write(message);
            pw.flush();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }


    /*  COMMUNICATION ENTRE LE CLIENT TCP ET UDP  ***************************************************************/

    public static void communication(int port, Joueur client, int[][] jeu) {
        try {
            Socket socket = new Socket("localhost", port);

            /* Le joueur vient de se connecter au serveur, il n'est pas encore prêt à jouer */
            String message = String.valueOf(read_reply_server(socket));
            String request[] = message.split(("\\*+"));
            for(int i=0; i<request.length; i++) {
                print_reply(request[i], jeu);
            }
            String demande = choose_request_client(client, 1);
            while(!demande.equals("START***")) {
                send_request(demande, port, socket);
                message = String.valueOf(read_reply_server(socket));
                String request_2[] = message.split("\\*+");
                print_reply(request_2[0], jeu);
                if(request_2[0].equals("REGNO") || request_2[0].equals("UNGOK")) {
                    demande = choose_request_client(client, 1);
                } else if((request_2[0].substring(0, 5)).equals("GAME!")) {
                    message = String.valueOf(read_reply_server(socket));
                    String request_3[] = message.split(("\\*+"));
                    for(int i=0; i<request_3.length; i++) {
                        print_reply(request_3[i], jeu);
                    }
                    demande = choose_request_client(client, 2);
                } else if((request_2[0].substring(0, 5)).equals("LIST!")) {
                    message = String.valueOf(read_reply_server(socket));
                    String request_3[] = message.split(("\\*+"));
                    for(int i=0; i<request_3.length; i++) {
                        print_reply(request_3[i], jeu);
                    }
                    demande = choose_request_client(client, 2);
                } else {
                    demande = choose_request_client(client, 2);
                }
            }
            send_request(demande, port, socket);
            message = String.valueOf(read_reply_server(socket));
            String request_2[] = message.split("\\*+");
            String request_3[] = request_2[0].split(" ");
            while(!request_3[0].equals("WELCO")) {
                message = String.valueOf(read_reply_server(socket));
                request_2 = message.split("\\*+");
                request_3 = request_2[0].split(" ");
            }


            System.out.println("    " + request_2[0]);
            print_reply(request_2[0], jeu);
            message = String.valueOf(read_reply_server(socket));

            String request_4[] = message.split("\\*+");

            print_reply(request_4[0], jeu);
            System.out.println("iam here"+ request_4[0]);



            while(!demande.equals("IQUIT***")) {

                demande = choose_request_client(client, 3);
                send_request(demande, port, socket);
                 message = String.valueOf(read_reply_server(socket));

                request_2 = message.split("\\*+");

                print_reply(request_2[0], jeu);




                }
                 message = String.valueOf(read_reply_server(socket));

                 request_2 = message.split("\\*+");

                print_reply(request_2[0], jeu);

        } catch(Exception e) {
            e.printStackTrace();
        }
    }

    public void run() {
        try {
            for(int i=0; i<10; i++) {
                for(int j=0; j<20; j++) {
                    jeu[i][j] = 1;
                }
            }
            communication(port, client, jeu);
        } catch(NumberFormatException ex) {
            ex.printStackTrace();
        }
    }
}
