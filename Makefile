SUBDIRS = tempsense doorsense

export BUILDROOTDIR = $(CURDIR)/buildroot
export SKELDIR = 	$(CURDIR)/skel
export OUTPUTDIR =	$(SKELDIR)/pi
export OUTBINDIR =	$(OUTPUTDIR)/bin
export OUTLIBDIR =	$(OUTPUTDIR)/lib
export ROOTDIR = 	$(BUILDROOTDIR)/output/target
export IMAGEDIR =	$(BUILDROOTDIR)/output/images

export BUILDCC =        $(BUILDROOTDIR)/output/host/usr/bin/arm-linux-gcc
export BUILDCXX =       $(BUILDROOTDIR)/output/host/usr/bin/arm-linux-g++
export BUILDAR =        $(BUILDROOTDIR)/output/host/usr/bin/arm-linux-ar

export CFLAGS =		-mfloat-abi=hard -mfpu=vfp -mtune=arm1176jzf-s -march=armv6zk
export CXXFLAGS =	-mfloat-abi=hard -mfpu=vfp -mtune=arm1176jzf-s -march=armv6zk

default: all

all:
	rm -rf $(OUTPUTDIR)/bin/*
	mkdir -p $(OUTBINDIR)
	mkdir -p $(OUTLIBDIR)
	mkdir -p $(OUTPUTDIR)/etc
	for subdir in $(SUBDIRS); do \
		(cd $$subdir && $(MAKE) clean && $(MAKE)) \
	done;
	(cd $(BUILDROOTDIR);make)
	cp buildroot/output/images/zImage .

setup:
	./checkconfig.sh
	(cd $(BUILDROOTDIR);make)

clean:
	for subdir in $(SUBDIRS); do \
		(cd $$subdir && $(MAKE) clean) \
	done;
	rm -rf $(OUTBINDIR)
	rm -rf $(OUTLIBDIR)
	rm -f zImage
