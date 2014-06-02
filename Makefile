CXX = gcc
CCFLAGS = -Wall -Werror -ansi -pedantic -fPIC

LIBS = -lbibtexManager
LIBSDIR = -L/usr/lib/ -L.
INCLUDEDIR = -I. -I/usr/include/

LIBNAME = libbibtexManager.so
LIBFILES = bibtexManager
SOURCELIBFILES = $(LIBFILES:=.c)
OBJECTLIBFILES = $(LIBFILES:=.o)

TARGET = bibtexManager.exe
TARGETFILES = bibtexMain
SOURCETARGETFILE = $(TARGETFILES:=.c)
OBJECTTARGETFILE = $(TARGETFILES:=.o)

all: $(TARGET)

lib: $(LIBNAME)

$(TARGET): $(OBJECTTARGETFILE) $(LIBNAME)
	@echo "\nGeneration of " $(TARGET)
	$(CXX) $(CCFLAGS) $(LIBSDIR) -o $(TARGET) $(OBJECTTARGETFILE) $(LIBS)

.c.o:
	@echo "\nGeneration of " $@ " from " $<
	$(CXX) $(INCLUDEDIR) $(CCFLAGS) -c -o $@ $<

$(LIBNAME): $(OBJECTLIBFILES)
	@echo "\nLibrary Generation named " $(LIBNAME)
	$(CXX) $(CCFLAGS) -shared -o $(LIBNAME) $(OBJECTLIBFILES)

clean:
	@echo "\n Cleaning current directory"
	rm -f *.o *~ 
