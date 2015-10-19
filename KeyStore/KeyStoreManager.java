import java.io.File;
import java.security.cert.X509Certificate;

import sun.security.tools.keytool.CertAndKeyGen;
import sun.security.x509.X500Name;

class KeyStoreManager {
	static String keystoreFile = "domain.jks";

	static void create() throws Exception {
		CertAndKeyGen keyGen = new CertAndKeyGen("RSA", "SHA1WithRSA", null);
		keyGen.generate(1024);

		X509Certificate[] chain = new X509Certificate[1];
		chain[0] = keyGen.getSelfCertificate(new X500Name("CN=ROOT"), (long) 365 * 24 * 3600);

		System.out.println("Certificate : " + chain[0].toString());
	}

	static void clean() throws Exception {
		new File(keystoreFile).delete();
	}
}
