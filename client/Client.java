public class Client {

    /* Limite l'identifiant du client à 8 caractères */
    public static String limitsPseudo(String pseudo) {
        String res = "";
        int size = pseudo.length();
        if(size > 8) {
            for(int i=0; i<8; i++) {
                res += pseudo.charAt(i);
            }
        } else if(size < 8) {
            res = pseudo;
            for(int i=size; i<8; i++) {
                res += " ";
            }
        } else {
            res = pseudo;
        }
        return res;
    }

    /* Affiche un message de bienvenue */
    public static void afficheBienvenue(String pseudo) {
        System.out.println("#####################################");
        System.out.println("# Bienvenue sur le jeu de GHOSTLAB. #");
        System.out.println("#####################################");
        System.out.println();
        System.out.println("Vous vous êtes connecté en tant que " + pseudo + ".");
        System.out.println();
    }

    /* Méthode main : lance les threads du client */
    public static void main(String[] args) {
        try {
            String pseudo;
            int portTCP;
            if(args.length == 2) {
                pseudo = limitsPseudo(args[0]);
                portTCP = Integer.parseInt(args[1]);
            } else {
                System.out.println("Erreur : Mauvais arguments.");
                return;
            }

            ComUDP udp = new ComUDP(pseudo);
            udp.start();

            ComTCP tcp = new ComTCP(pseudo, portTCP, udp);
            afficheBienvenue(pseudo);
            tcp.start();

        } catch (Exception e) {
            System.out.println("Erreur : Mauvais arguments.");
            return;
        }
    }
}