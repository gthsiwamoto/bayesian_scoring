CC=gcc

CFLAGS=-std=gnu11 -O2 -Wall
LDFLAGS=-Wall -lm

SRCDIR=src
OBJDIR=obj
BINDIR=bin

SOURCES=$(wildcard $(SRCDIR)/*.c)
OBJECTS=$(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
INCLUDES=-Iinclude

TARGET=bayesian_scoring

all: $(SOURCES) $(BINDIR)/$(TARGET)

$(BINDIR)/$(TARGET): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@

.PHONY: clean
clean: 
	rm $(OBJECTS) $(BINDIR)/$(TARGET)
