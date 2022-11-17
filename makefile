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
OBJS := $(patsubst %.cc, %.o, $(CCFILES))
LCLIBS := $(patsubst %, -l%, $(CLIBS))
LIBS += libops.so
LIBS += libopswrapper.so
LIBS += libcodestream.so
LLIBS := $(patsubst lib%.so, -l%, $(LIBS))

vpath %.cc ./src:./lib
vpath %.h ./inc

%.o:%.cc
	$(CC) $(CCFLAGS) -o $@ -c $< -Iinc

codestream: codestream.o $(LIBS)
	@echo "uncompleted"

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
