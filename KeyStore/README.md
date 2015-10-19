# Key store demo

## generate java key store

~~~bash
keytool -genkey -alias hurley.local -keyalg RSA -keystore domain.jks -keysize 2048
keytool -genkeypair -alias rsakey -keyalg rsa -storepass domain -keystore mytestkeys.jks -storetype JKS -dname  "CN=ROOT"
~~~

### password: domain

## run

~~~bash
java Broker1 &
java Broker2
~~~
