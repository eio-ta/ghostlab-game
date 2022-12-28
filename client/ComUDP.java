import java.net.*;
import java.util.Random;

public class ComUDP extends Thread {

    String pseudo; // Identifiant du client (8 caractères)
    int portUDP; // Port UDP du client

    final int SIZE_MESSAGE = 200; // Taille maximal d'un message

    public ComUDP(String pseudo) {
        this.pseudo = pseudo;
    }



    /*  COMMUNICATION AVEC LE SERVEUR ***************************************************************************/

    /* Retourne le port UDP et son Datagram correspondant */
    public DatagramSocket choix_portUDP() {
        DatagramSocket dso;
        try {
            Random rand = new Random();
            portUDP = rand.nextInt(8000 - 5000 + 1) + 5000;
            dso =  new DatagramSocket(portUDP);
        } catch(Exception e) {
            dso = choix_portUDP();
        }
        return dso;
    }
    
    /* Gestion de la communication avec le serveur */
    public void run() {
        try{
            DatagramSocket dso = choix_portUDP();
            System.out.print("Votre port UDP est : " + portUDP + ".\n");
            byte[] data = new byte[SIZE_MESSAGE];
            while(true) {
                DatagramPacket paquet= new DatagramPacket(data, data.length);
                dso.receive(paquet);

                String st = new String(paquet.getData(), 0, paquet.getLength());
                String mess = "";
                for(int i=0; i<st.length(); i++) {
                    if(st.charAt(i) != '+' && st.charAt(i) != '*') {
                        mess += st.charAt(i);
                    }
                }

                String request[] = mess.split(" ");

                String cont = "";
                for(int i=2; i<request.length; i++) {
                    if(i != request.length-1) cont += request[i] + " ";
                }
                cont += request[request.length-1];

                System.out.println("\n\n## Message de " + request[1] + " (en privé) : " + cont + "\n\n");
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
        } catch(Exception e){
            e.printStackTrace();
        }
    }
}