CFLAGS=-Wall -std=c11 -g -static
SRCS=main.c token.c vector.c
OBJS=main.o token.o vector.o
#SRCS=$(wildcard *.c)
#OBJS=$(SRCS:.c=.o)

38sh: $(OBJS)
	$(CC) -g -o 38sh $(OBJS) $(LDFLAGS)

$(OBJS): 38sh.h $(SRCS)

clean:
	rm -f 38sh *.s *.o *~ tmp/* .*.c

.PHONY: test clean
