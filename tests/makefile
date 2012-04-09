CC = gcc
CFLAGS = -std=c99 -Wall -g -pedantic -O -DTEST
INCLUDES = .
LIBS = ./lib
RM = rm
TARGET = tests

TESTOBJS = main.o TimeTest.o
SRC = ../src/time.o
SRCOBJS = time.o


all: $(TARGET)

$(TARGET): $(TESTOBJS) $(SRC)
	$(CC) -o $@ $(TESTOBJS) $(SRCOBJS) -L$(LIBS) -lembUnit

.c.o:
	$(CC) $(CFLAGS) -I$(INCLUDES) -c $<

clean:
	-$(RM) $(TARGET) $(TESTOBJS) $(SRCOBJS)

.PHONY: clean all
