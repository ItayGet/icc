CC = gcc

INCDIR = ./include
SRCDIR = ./src
OBJDIR = ./obj

CFLAGS = -I$(INCDIR) -fdiagnostics-color=always
LIBS = -lm

OBJ = lex.o stringbuilder.o literal.o parser.o tokenstream.o icc.o symboltable.o type.o symbol.o ir.o parser-type.o

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
