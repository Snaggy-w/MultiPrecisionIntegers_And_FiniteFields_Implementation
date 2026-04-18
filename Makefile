CC = gcc
CFLAGS = -Wall $(DEFS) -DWORD_SIZE=$(WORD_SIZE)
LDFLAGS = -lm

OBJS = ex1.o rsa.o ex2.o

ifeq ($(MULPR),1)
DEFS += -DMULPR
endif
WORD_SIZE ?= 4

all: test-znz rsa-demo f128-demo generator f128-zech-demo

ex1.o: ex1.c ex1.h ex1-mulpr.h
	$(CC) $(CFLAGS) -c ex1.c

rsa.o: rsa.c rsa.h ex1.h
	$(CC) $(CFLAGS) -c rsa.c

ex2.o: ex2.c ex2.h
	$(CC) $(CFLAGS) -c ex2.c

ex2-zechlog.o: ex2-zechlog.c ex2.h ex2-zechlog.h
	$(CC) $(CFLAGS) -c ex2-zechlog.c

test-znz: test-znz.c ex1.o
	$(CC) $(CFLAGS) test-znz.c ex1.o -o test-znz $(LDFLAGS)

rsa-demo: rsa-demo.c rsa.o ex1.o
	$(CC) $(CFLAGS) rsa-demo.c rsa.o ex1.o -o rsa-demo $(LDFLAGS)

generator: generator.c ex2.o
	$(CC) $(CFLAGS) generator.c ex2.o -o generator $(LDFLAGS)

f128-demo: f128-demo.c ex2.o
	$(CC) $(CFLAGS) f128-demo.c ex2.o -o f128-demo $(LDFLAGS)

f128-zech-demo: f128-zech-demo.c ex2.o ex2-zechlog.o
	$(CC) $(CFLAGS) f128-zech-demo.c ex2.o ex2-zechlog.o -o f128-zech-demo $(LDFLAGS)

clean:
	rm -f *.o test-znz rsa-demo f128-demo f128-zech-demo generator
