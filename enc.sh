

openssl enc -aes-256-cbc -salt in FILE -out FILE.enc
openssl enc -aes-256-cbc -d in FILE.enc -out FILE
