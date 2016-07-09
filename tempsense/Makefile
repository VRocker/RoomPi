OBJS=dht11.o ../shared/gpiohandler.o main.o ../shared/clientsock/ClientSock.o TempSensorPacket.o
LDFLAGS+=-lzmq -lmsgpackc
INCLUDES+=-I../shared

CXXFLAGS+=-std=c++11
BIN=tempsense.bin

include ../Makefile.include
