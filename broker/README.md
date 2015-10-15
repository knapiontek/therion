# A small POC showing how to upgrade an existing socket to SSL

## running

java -Djavax.net.ssl.keyStore=foobar -Djavax.net.ssl.keyStorePassword=foobar UpgradingEchoServer &

java -Djavax.net.ssl.trustStore=foobar -Djavax.net.ssl.trustStorePassword=foobar UpgradingEchoClient
