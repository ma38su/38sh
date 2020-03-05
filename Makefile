CFLAGS=-Wall -std=c11 -g -static
SRCS=main.c token.c parse.c exec.c debug.c
OBJS=main.o token.o parse.o exec.o debug.o
#SRCS=$(wildcard *.c)
#OBJS=$(SRCS:.c=.o)

38sh: $(OBJS) vector.o
	$(CC) -g -o 38sh $(OBJS) vector.o $(LDFLAGS)

$(OBJS): 38sh.h $(SRCS)

vector.o: vector.h vector.c

caterr: test/caterr.c
	$(CC) -o test/caterr test/caterr.c

test: 38sh test.sh caterr
	bash test.sh

clean:
	rm -f 38sh *.s *.o *~ tmp/* .*.c test/caterr

.PHONY: test clean
