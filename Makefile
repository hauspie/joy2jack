CFLAGS=-Wall -W
LDFLAGS=-ljack
CC=clang
LD=$(CC)

EXE=joy2jack

SRCS=$(wildcard src/*.c)
OBJS=$(SRCS:.c=.o)

$(EXE): $(OBJS)
	$(LD) -o $@ $^ $(LDFLAGS)

.PHONY: clean

clean:
	$(RM) $(OBJS)
