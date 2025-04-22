CC = gcc
CFLAGS = -Wall -Wextra

all: scanner parser

scanner: scanner.o token_strings.o
	$(CC) $(CFLAGS) -o scanner scanner.o token_strings.o

scanner.o: scanner.c tokens.h
	$(CC) $(CFLAGS) -c scanner.c

token_strings.o: token_strings.c tokens.h
	$(CC) $(CFLAGS) -c token_strings.c

parser: parser.o token_strings.o
	$(CC) $(CFLAGS) -o parser parser.o token_strings.o

parser.o: parser.c tokens.h
	$(CC) $(CFLAGS) -c parser.c

clean:
	rm -f scanner parser tokens.txt tree.txt

run: all
	./scanner
	./parser

.PHONY: all clean run