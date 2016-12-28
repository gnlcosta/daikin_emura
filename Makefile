

# root directory
ROOT_DIR = $(shell pwd)

# src file
SRC = decode.c cmd_compare.c daikin_emura.c

# compilation
CFLAGS=-g -Wall -I./include
LDFLAGS=-lwiringPi


all: decode compare decode_all daikin_emura

decode: decode.o
	$(CC) $(CFLAGS) $(LDFLAGS) $+ -o $@

compare: cmd_compare.o
	$(CC) $(CFLAGS) $(LDFLAGS) $+ -o $@
	
decode_all:
	./decode_all.py

daikin_emura: daikin_emura.o
	$(CC) $(CFLAGS) $(LDFLAGS) $+ -o $@

clean:
	$(RM) -f *~ */*~ *.o decode compare daikin_emura cmds/* .depend

	
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

.depend: $(SRC)
	$(CC) -M $(CFLAGS) $(SRC) > $@


sinclude .depend
