all: codestream

CC := g++
CCFLAGS := -g -Wall -std=gnu++2a
CLIBS := pthread
LCLIBS := 
LIBS :=
LLIBS :=
CCFILES :=
OBJS := 
STATIC_OBJS :=
DIRS := inc bin src


CCFILES += $(wildcard $(foreach cf, $(DIRS), $(cf)/*.cc))
LCLIBS := $(patsubst %, -l%, $(CLIBS))

OBJS += operation_definition.o
OBJS += ops_wrapper.o
OBJS += codestream_abstract.o

STATIC_OBJS += codestream_main.o
STATIC_OBJS += codestream.o

LIBS += libops.so
LIBS += libopswrapper.so
LIBS += libcodestream.so
LLIBS := $(patsubst lib%.so, -l%, $(LIBS))

vpath %.cc ./src:./lib
vpath %.h ./inc

%.o:%.cc
	$(CC) $(CCFLAGS) -o $@ -c $< -Iinc

codestream: $(STATIC_OBJS) $(OBJS)
	$(CC) $(CCFLAGS) -o $@ $^ $(LCLIBS)
	@mv $@ bin/

codestream_libv: $(STATIC_OBJS) $(LIBS)
	$(CC) $(CCFLAGS) -o $@ $(STATIC_OBJS) -L. $(LLIBS) $(LCLIBS)
	@echo "!!! Have to move lib files $(LIBS) into /lib,and then run ldconfig."
	@mv $@ bin/
	@echo "moved codestream into bin/"

help:
	@echo "Usage : make codestream | make codestream_libv"

libops.so: operation_definition.cc
libopswrapper.so: ops_wrapper.cc
libcodestream.so: codestream_abstract.cc
$(LIBS):%.so:%.cc
	$(CC) $(CCFLAGS) -fPIC -shared -o $@ $< -Iinc


.PHONY: clean
clean:
	@rm -f *.o *.so
	@rm -f $(foreach d, $(DIRS), $(d)/*~)
