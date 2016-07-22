OBJS=datahandler.o SockHandler.o SensorInfoRecvPacket.o webapiv1.o Utils.o ../shared/rconfig.o ../shared/strutils.o
LDFLAGS+=-lzmq -lmsgpackc -lcurl -ljson-c
INCLUDES+=-I../shared

CXXFLAGS+=-std=c++11
BIN=datahandler.bin

include ../Makefile.include
