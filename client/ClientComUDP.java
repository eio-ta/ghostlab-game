import java.net.*;

public class ClientComUDP extends Thread {
    Joueur client = null;

    public ClientComUDP(Joueur client) {
        this.client = client;
    }

    public void run() {
        try {
            DatagramSocket ds = new DatagramSocket(client.getPortUDP());
            byte[] data = new byte[100];
            while (true) {
                DatagramPacket paquet = new DatagramPacket(data, data.length);
                ds.receive(paquet);
                String st = new String(paquet.getData(), 0, paquet.getLength());
                System.out.println("Vous avez recu un message  :" + st);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
