
import java.net.*;
import java.io.*;

import javax.net.ssl.*;

public class Broker2
{
    public static void main(String[] args) throws IOException
    {
        Socket socket = null;
        try {
        	socket = new Socket("localhost", 9999);
            DataInputStream is = new DataInputStream(socket.getInputStream());
        	DataOutputStream os = new DataOutputStream(socket.getOutputStream());

        	String hello = "Hello";
            os.writeUTF(hello);
            System.out.println("Broker2: sent: " + hello);
            String received = is.readUTF();
            System.out.println("Broker2: received: " + received);

            String startTLS = "StartTLS";
            os.writeUTF(startTLS);
            System.out.println("Broker2: sent: " + startTLS);
            
            SSLSocketFactory sslSocketFactory = (SSLSocketFactory)SSLSocketFactory.getDefault();
            SSLSocket sslSocket = (SSLSocket)sslSocketFactory.createSocket(
            		socket,
            		socket.getInetAddress().getHostAddress(),
            		socket.getPort(),
            		true);

            is = new DataInputStream(sslSocket.getInputStream());
            os = new DataOutputStream(sslSocket.getOutputStream());

            String helloTLS = "HelloTLS";
            os.writeUTF(helloTLS);
            System.out.println("Broker2: sent: " + helloTLS);
            received = is.readUTF();
            System.out.println("Broker2: received: " + received);
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            if (socket != null) {
            	socket.close();
            }
        }
    }
}
