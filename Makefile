CFLAGS=-Wall -W -g -O
CFLAGS+=$(shell pkg-config jack --cflags)
LDFLAGS=
LDFLAGS+=$(shell pkg-config jack --libs)
CC=clang
LD=$(CC)

EXE=joy2jack

SRCS=$(wildcard src/*.c)
OBJS=$(SRCS:.c=.o)

$(EXE): $(OBJS)
	$(LD) -o $@ $^ $(LDFLAGS)

.PHONY: clean makefile.dep

makefile.dep:
	$(CC) -MM $(SRCS) | sed 's@^\([^:]*\):@src/\1:@g' > $@

clean:
	$(RM) $(OBJS)

include makefile.dep
