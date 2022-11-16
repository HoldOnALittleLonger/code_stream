all: codestream

CC := g++
CCFLAGS := -g -Wall -std=gnu++2a
CLIBS := pthread
LIBS :=
CCFILES :=
OBJS := 
DIRS := inc bin src

vpath %.cc ./src
vpath %.h ./inc

%.o:%.cc
	$(CC) $(CCFLAGS) -o $@ -c $< -Iinc

%.so:%.cc
	$(CC) -fPIC -shared -o $@ $< -Iinc

CCFILES += $(wildcard $(foreach cf, $(DIRS), $(cf)/*.cc))
OBJS := $(patsubst %.cc, %.o, $(CCFILES))
LCLIBS := $(patsubst %, -l%, $(CLIBS))

LIBS += libops.so
LIBS += libopswrapper.so
LIBS += libcodestream.so
LLIBS := $(patsubst lib%.so, -l%, $(LIBS))

codestream: $(LIBS) ops_test.o
	@echo "uncompleted"

libops.so: operation_definition.cc
libopswrapper.so: ops_wrapper.cc
libcodestream.so: codestream_abstract.cc

.PHONY: catm clean ops_test ops_wrapper_test make_libs
catm: codestream_abstract_test.o codestream_abstract.o
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBS)

ops_test: ops_test.o operation_definition.o
	$(CC) $(CCFLAGS) -o $@ $^

ops_wrapper_test: ops_wrapper_test.o ops_wrapper.o operation_definition.o
	$(CC) $(CCFLAGS) -o $@ $^

clean:
	@rm -f *.o
	@rm -f $(foreach d, $(DIRS), $(d)/*~)
