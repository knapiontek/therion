# Key store demo

## run

~~~bash
java -Djavax.net.ssl.keyStore=foobar -Djavax.net.ssl.keyStorePassword=foobar Broker1 &
java -Djavax.net.ssl.trustStore=foobar -Djavax.net.ssl.trustStorePassword=foobar Broker2
~~~
