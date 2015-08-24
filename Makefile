#
#
CC=gcc
CFLAGS=-g

# Linux and OS X machines have different library inclusion requirements
# than some other unixes.
ifneq ($(OSTYPE),linux)
ifneq ($(OSTYPE),darwin)
LIB= -lsocket -lnsl
endif
endif

all: listen speak

listen:	listen.o
	$(CC) $(CFLAGS) -o $@ listen.o $(LIB)

speak:	speak.o
	$(CC) $(CFLAGS) -o $@ speak.o $(LIB)

listen.o:	listen.c

speak.o:	speak.c 

clean:
	\rm -f listen speak

squeaky:
	make clean
	\rm -f listen.o speak.o

tar:
	cd ..; tar czvf socket.tar.gz socket/Makefile socket/listen.c socket/speak.c socket/README; cd socket; mv ../socket.tar.gz .

