CC=gcc
CCFLAGS=-g -Wall
LDFLAGS=-lasound -lm
all:recordc play

recordc:alsa_record.c
	$(CC) alsa_record.c $(CCFLAGS) $(LDFLAGS) -o recordc
play:alsa_play.c
	$(CC) alsa_play.c $(CCFLAGS) $(LDFLAGS) -o play

clean:
	rm recordc records play
