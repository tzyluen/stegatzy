CC=gcc
CFLAGS=-Wall -g
LIBS=-lmcrypt
OUTPUT=exec

default: aes exec

aes:
	$(CC) $(CFLAGS) $(LIBS) -c aes.c

exec:
	$(CC) $(CFLAGS) $(LIBS) aes.o exec.c -o $(OUTPUT)

stitch:
	./$(OUTPUT) -s -c xeyes -i secret.long

unstitch:
	./$(OUTPUT) -u -c xeyes -t -x decrypted.text 

clean:
	rm -f *.o a.out exec encrypted decrypted.text xeyes
	cp xeyes.bak xeyes
