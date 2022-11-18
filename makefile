all: codestream

CC := g++
CCFLAGS := -g -Wall -std=gnu++2a
CLIBS := pthread
LCLIBS := 
LIBS :=
LLIBS :=
CCFILES :=
OBJS := 
DIRS := inc bin src


CCFILES += $(wildcard $(foreach cf, $(DIRS), $(cf)/*.cc))
LCLIBS := $(patsubst %, -l%, $(CLIBS))

OBJS += operation_definition.o
OBJS += ops_wrapper.o
OBJS += codestream_abstract.o
OBJS += codestream.o

LIBS += libops.so
LIBS += libopswrapper.so
LIBS += libcodestream.so
LLIBS := $(patsubst lib%.so, -l%, $(LIBS))

vpath %.cc ./src:./lib
vpath %.h ./inc

%.o:%.cc
	$(CC) $(CCFLAGS) -o $@ -c $< -Iinc

codestream: codestream_main.o $(OBJS)
	$(CC) $(CCFLAGS) -o $@ $^ $(LCLIBS)

libops.so: operation_definition.cc
libopswrapper.so: ops_wrapper.cc
libcodestream.so: codestream_abstract.cc
$(LIBS):%.so:%.cc
	$(CC) $(CCFLAGS) -fPIC -shared -o $@ $< -Iinc


.PHONY: catm clean ops_test ops_wrapper_test make_libs
catm: codestream_abstract_test.o codestream_abstract.o
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBS)

ops_test: ops_test.o operation_definition.o
	$(CC) $(CCFLAGS) -o $@ $^

ops_wrapper_test: ops_wrapper_test.o ops_wrapper.o operation_definition.o
	$(CC) $(CCFLAGS) -o $@ $^

make_libs: $(LIBS)
	@mv $(LIBS) lib/

clean:
	@rm -f *.o lib/*.so
	@rm -f $(foreach d, $(DIRS), $(d)/*~)
