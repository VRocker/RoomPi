OBJS=../shared/gpiohandler.o main.o DoorStatePacket.o

CXXFLAGS+=-std=c++11
BIN=doorsense.bin

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
