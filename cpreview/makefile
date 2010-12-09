TARGET = linux
#TARGET = win

CC = icc
CXX = icpc
MAKEINFO = makeinfo
PACKAGE = cgpsmapper

# CPPFLAGSLINUX = -DLINUX
# CPPFLAGSWINDOWS = -DMINGW
bin_PROGRAMS = cpreview
cpreview_SOURCES = mdr_creator.cpp filexor.cpp utils.cpp preview.cpp tdb.cpp compare.cpp cpreview.cpp vcgl_gpc.cpp error.cpp ftrcreate.cpp \
./../sqlite-source/sqlite3.c ./../mapRead/img_internal.cpp ./../mapRead/mapReadDLL.cpp ./../mapRead/img.cpp ./../mapRead/hcode.cpp ./../mapRead/ITOA.C \
./../gusb/com_gps.cpp ./../gusb/gps.cpp ./../gusb/g_usb_gps.cpp ./../gusb/progress.cpp \
./../gusb/sim_gps.cpp ./../gusb/single_map.cpp ./../gusb/uploader.cpp ./../gusb/gui_sendmap.cpp \
./../gusb/sendmapDLL.cpp ./../gusb/img_manager.cpp


cpreview_OBJECTS =  mdr_creator.o filexor.o utils.o preview.o tdb.o compare.o cpreview.o vcgl_gpc.o error.o ftrcreate.o \
./../sqlite-source/sqlite3.o ./../mapRead/img_internal.o ./../mapRead/mapReadDLL.o ./../mapRead/img.o ./../mapRead/hcode.o ./../mapRead/ITOA.o \
./../gusb/com_gps.o ./../gusb/gps.o ./../gusb/g_usb_gps.o ./../gusb/progress.o \
./../gusb/sim_gps.o ./../gusb/single_map.o ./../gusb/uploader.o ./../gusb/gui_sendmap.o \
./../gusb/sendmapDLL.o ./../gusb/img_manager.o

#
#
#

#CFLAGSWINDOWS = -DMINGW -mno-cygwin -O1 -DPACKAGE=\"cgpsmapper\" -I./ -I./shp/ -I./importlib/ -I./importlib/vcgl/

CFLAGSLINUX = -D_MAPREAD_STATIC -DLINUX -O2 -DPACKAGE=\"cpreview\" -I./ -I./../mapRead/ -I./../sqlite-source/ -static-intel -static-libgcc -gcc-version=430 -pthread
#CFLAGSLINUX = -D_MAPREAD_STATIC -DLINUX -g -DPACKAGE=\"cpreview\" -I./ -I./../mapRead/ -I./../sqlite-source/ -static-intel -static-libgcc -gcc-version=430 -pthread

ifeq ($(TARGET),windows)
CWX = $(CXX) $(CFLAGSWINDOWS)
CWN = $(CC) $(CFLAGSWINDOWS)
else
CWX = $(CXX) $(CFLAGSLINUX)
CWN = $(CC) $(CFLAGSLINUX)
endif

CXXLD = $(CXX)
CXXLINK = $(CXXLD) -o $@

all: cpreview-static

.cpp.o:
	$(CWX) -c -o $@ $<

.c.o:
	$(CWN) -c -o $@ $<


cpreview: $(gpsmapper_OBJECTS) $(gpsmapper_SOURCES)
	$(CWX) $(gpsmapper_OBJECTS) -o cpreview
#
#
#

cpreview-static: $(cpreview_OBJECTS) $(cpreview_SOURCES)
	$(CWX) $(cpreview_OBJECTS) -static -o cpreview-static

clean:
	-rm -f *.o core *.core  ./../mapRead/*.o ./../sqlite-source/*.o ./../gusb/*.o

