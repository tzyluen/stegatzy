CC=gcc
CFLAGS=-Wall -g
LIBS=-lmcrypt
OUTPUT=hideinbin

default: aes hideinbin

aes:
	$(CC) $(CFLAGS) $(LIBS) -c aes.c

hideinbin:
	$(CC) $(CFLAGS) $(LIBS) aes.o hideinbin.c -o $(OUTPUT)

stitch:
	./$(OUTPUT) -s -c assets/xeyes -i secret.long

unstitch:
	./$(OUTPUT) -u -c assets/xeyes -t -x decrypted.text 

clean:
	rm -f *.o a.out hideinbin encrypted decrypted.text assets/xeyes
	cp assets/xeyes.bak assets/xeyes
