CC = gcc

INCDIR = ./include
SRCDIR = ./src
OBJDIR = ./obj

CFLAGS = -I$(INCDIR)
LIBS = -lm

OBJ = lex.o stringbuilder.o literal.o parser.o token-stream.o icc.o

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(OBJDIR) icc

icc: $(OBJ:%=$(OBJDIR)/%)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

$(OBJDIR): 
	mkdir $(OBJDIR)

.PHONY: debug clean

debug: CFLAGS += -g
debug: all

clean:
	rm -f *~
	rm -rf $(OBJDIR)
