import java.io.*;
import java.net.*;

public class ReceiveMultidiff extends Thread

{

public ReceiveMultidiff() {}

    public void run (){



        try
        {
/* MulticastSocket(int port)  int port : le port sera le port de multi-diffusion */
            MulticastSocket mso = new MulticastSocket (6000);
/* elle permet de rejoindre le groupe correspondant à l'adresse de multidiffusion donné en argument */
            mso.joinGroup(InetAddress.getByName( "225.1.2.4"));
            byte [] data = new byte [100];
            DatagramPacket paquet = new DatagramPacket(data, data.length);
         while(true)
        {

            mso.receive(paquet);
            String st = new String(paquet.getData(),0,paquet.getLength());
            System.out.println("msge recu  :"  +st);
        }

        }
        catch (Exception e)
        {
                 e.printStackTrace();

        }

    }

}
