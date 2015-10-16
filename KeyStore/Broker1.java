
import java.net.*;
import java.io.*;
import javax.net.ssl.*;

public class Broker1 {
	public static void main(String[] args) {
		System.setProperty("javax.net.ssl.keyStore", "domain.jks");
		System.setProperty("javax.net.ssl.keyStorePassword", "domain");

		try (ServerSocket serverSocket = new ServerSocket(8888)) {
			System.out.println("Ready ...");

			try (Socket clientSocket = serverSocket.accept()) {
				System.out.println("Connection accepted");

				DataInputStream is = new DataInputStream(clientSocket.getInputStream());
				DataOutputStream os = new DataOutputStream(clientSocket.getOutputStream());

				String hello = is.readUTF();
				os.writeUTF("Broker1: " + hello);

				String startTLS = is.readUTF();
				if (!startTLS.equals("StartTLS")) {
					throw new Exception("Broker1: received unexpected: " + startTLS);
				}

				SSLSocketFactory sslSocketFactory = (SSLSocketFactory) SSLSocketFactory.getDefault();
				try (SSLSocket sslSocket = (SSLSocket) sslSocketFactory.createSocket(clientSocket,
						clientSocket.getInetAddress().getHostAddress(), clientSocket.getPort(), true)) {
					sslSocket.setUseClientMode(false);
					sslSocket.startHandshake();

					is = new DataInputStream(sslSocket.getInputStream());
					os = new DataOutputStream(sslSocket.getOutputStream());

					String helloTLS = is.readUTF();
					os.writeUTF("Broker1: " + helloTLS);

					System.out.println("StartTLS accepted");
				}
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
}
