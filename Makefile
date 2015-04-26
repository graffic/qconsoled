#
#	Qconsoled (by TnT clan) 
#

# Flags de Optimizacion normales
#CFLAGS=-O2 -Wall

# Flags de Optimizacion para Pentium 
CFLAGS=-O6 -Wall -mpentium 

# Flags de debug
DEBUG=-debug -g

CC=gcc
OBJETOS=inet.o fd_opc.o qserver.o principal.o

all: clean Qconsoled

Qconsoled: $(OBJETOS)
	$(CC) $(CFLAGS) $(OBJETOS) -o $@ 

clean:
	rm -f *~ core *.bp *.greg *.o
