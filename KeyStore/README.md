# Key store demo

## generate java key store

~~~bash
keytool -genkey -alias hurley.local -keyalg RSA -keystore domain.jks -keysize 2048
~~~

### password: domain

## run

~~~bash
java Broker1 &
java Broker2
~~~
