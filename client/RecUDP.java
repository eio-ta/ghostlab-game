import java.net.*;

public class RecUDP extends Thread {
    String port; // Port de la partie du jeu (multidiffusion)
    String adr; // Adresse de la partie du jeu (multidiffusion)

    public RecUDP(String p, String a) {
        this.port = p;
        this.adr = a;
    }



    /* COMMUNICATION AVEC LE SERVEUR  ***************************************************************************/

    /* Affiche la réponse du serveur */
    public void afficher_rep_server(String message) {
        String request[] = message.split(" ");

        switch(request[0]) {
            case "MESSA" :

                String cont = "";
                for(int i=2; i<request.length; i++) {
                    if(i != request.length-1) cont += request[i] + " ";
                }
                cont += request[request.length-1];

                System.out.println("\n\n## Message de " + request[1] + " (à tout le monde) : " + cont + "\n\n");
                break;
            case "SCORE" :
                System.out.println("\n\n## " + request[1] + " a attrapé un fantôme (" + request[3] + "x" + request[4] + ").");
                System.out.println("## " + request[1] + " a maintenant " + request[2] + " points.\n\n");
                break;
            case "GHOST" :
                System.out.println("\n\n## Un fantôme se déplace (" + request[1] + "x" + request[2] + ") !\n\n");
                break;
            case "ENDGA" :
                System.out.println("\n\n## La partie est terminée !");
                System.out.println("## Le gagnant est " + request[1] + " avec " + request[2] + " points.");
                System.out.println("## Merci d'avoir joué ! À bientot !\n\n");
                break;
            default : break;
        }
    }

    /* Gère la communication avec le serveur */
    public void run() {
        try {
            MulticastSocket mso = new MulticastSocket(Integer.parseInt(port));
            mso.joinGroup(InetAddress.getByName(adr));
            byte[] data = new byte[100];
            DatagramPacket paquet = new DatagramPacket(data, data.length);
            while(true) {
                mso.receive(paquet);
                String st = new String(paquet.getData(), 0, paquet.getLength());
                
                String mess = "";
                for(int i=0; i<st.length(); i++) {
                    if(st.charAt(i) != '+' && st.charAt(i) != '*') {
                        mess += st.charAt(i);
                    }
                }

                afficher_rep_server(mess);

                if(mess.substring(0, 5).equals("ENDGA")) {
                    mso.close();
                    System.exit(0);
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
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
