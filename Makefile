CC = gcc
CFLAGS = -Wall -Werror #-Wextra

all: pfsim
pfsim: pfsim.o
pfsim.o: pfsim.c

clean:
	rm -f pfsim pfsim.o
