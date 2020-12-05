CC = gcc
CFLAGS = -I.

LIBS = -lm

DEPS = 

OBJ = *.c

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
	
icc: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

debug: CFLAGS += -g
debug: icc

.PHONY: clean

clean:
	rm -f *.o *~ core *~
