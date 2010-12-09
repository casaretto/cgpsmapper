CC = gcc
CXX = c++ 

#CC = /opt/intel/cc/9.0/bin/icc
#CXX = /opt/intel/cc/9.0/bin/icc
MAKEINFO = makeinfo
PACKAGE = gpsmapper

bin_PROGRAMS = sendmap20
sendmap_SOURCES = sendmap.cpp com_gps.cpp gps.cpp g_usb_gps.cpp progress.cpp sim_gps.cpp single_map.cpp uploader.cpp gui_sendmap.cpp

#CXXFLAGS = -O2 -mno-cygwin -I./ 
CXXFLAGS = -static -O2 -DLINUX  -I./
#CXXFLAGS = -Kc++ -cxxlib-icc -static -g -DLINUX -D_DEBUG -I./

sendmap_OBJECTS = sendmap.o com_gps.o gps.o g_usb_gps.o progress.o sim_gps.o single_map.o uploader.o gui_sendmap.o

CXXCOMPILE = $(CXX) $(CXXFLAGS)
CXXLD = $(CXX) $(CXXFLAGS)
CXXLINK = $(CXXLD) $(CXXFLAGS) -o $@

all: sendmap20

.cpp.p:
	$(CXXCOMPILE) -c -o $@ $<

.c.o:
	$(CXXCOMPILE) -c -o $@ $<
	

sendmap20: $(sendmap_OBJECTS) $(sendmap_SOURCES)
	$(CXXCOMPILE) $(sendmap_OBJECTS) -o sendmap20

release: $(sendmap_OBJECTS) $(sendmap_SOURCES)
	$(CXXCOMPILE) $(sendmap_OBJECTS) -o sendmap20

clean:
	-rm -f *.o core *.core

