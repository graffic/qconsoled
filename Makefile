#
#	Qconsoled (by TnT clan) 
#

# Flags de Optimizacion normales
CFLAGS=-Wall -g -m486

# Flags de Optimizacion para Pentium 
#CFLAGS=-O6 -Wall -mpentium 

# Flags de debug
DEBUG=-debug -g

CC=gcc
OBJETOS=cliente_txt.o senyal.o principal.o conf.o qserver.o fd_opc.o inet.o

all: Qconsoled

Qconsoled: $(OBJETOS)
	$(CC) $(CFLAGS) $(OBJETOS) -o $@ 

clean:
	rm -f *~ core *.bp *.greg *.o
