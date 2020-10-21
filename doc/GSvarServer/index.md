## GSvarServer

A certificate-key pair has to be generated:

sudo openssl req -x509 -nodes -days 365 -newkey rsa:2048 -keyout dev-selfsigned.key -out dev-selfsigned.crt\n

