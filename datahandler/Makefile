OBJS=datahandler.o SockHandler.o SensorInfoRecvPacket.o 
LDFLAGS+=-lzmq -lmsgpackc

CXXFLAGS+=-std=c++11
BIN=datahandler.bin

all: $(BIN) $(LIB)

%.o: %.c
	@rm -f $@
	$(BUILDCC) $(CFLAGS) $(INCLUDES) -c $< -o $@ -Wno-deprecated-declarations

%.o: %.cpp
	@rm -f $@
	$(BUILDCXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@ -Wno-deprecated-declarations

%.bin: $(OBJS)
	$(BUILDCXX) -o $@ -Wl,--whole-archive $(OBJS) $(LDFLAGS) -Wl,--no-whole-archive -rdynamic
	cp $@ $(OUTPUTDIR)/bin/

%.a: $(OBJS)
	$(BUILDAR) r $@ $^

clean:
	for i in $(OBJS); do (if test -e "$$i"; then ( rm $$i ); fi ); done
	@rm -f $(BIN) $(LIB)
