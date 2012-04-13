CC = gcc
CFLAGS = -std=c99 -Wall -g -pedantic -O -DTEST
INCLUDES = .
LIBS = ./lib
RM = rm
TARGET = tests

TESTOBJS = main.o TimeTest.o NetworkTest.o mockDs1820.o mockEnc28j60.o mockLcd.o mockRelay.o
SRC = ../src/time.o ../src/net/network.o ../src/net/eth.o ../src/net/arp.o ../src/net/ip.o  ../src/net/icmp.o  ../src/net/tcp.o ../src/net/http.o
SRCOBJS = time.o network.o eth.o arp.o ip.o icmp.o tcp.o http.o


all: $(TARGET)

$(TARGET): $(TESTOBJS) $(SRC)
	$(CC) -o $@ $(TESTOBJS) $(SRCOBJS) -L$(LIBS) -lembUnit

.c.o:
	$(CC) $(CFLAGS) -I$(INCLUDES) -c $<

clean:
	-$(RM) $(TARGET) $(TESTOBJS) $(SRCOBJS)

.PHONY: clean all
