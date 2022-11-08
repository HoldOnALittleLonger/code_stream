all: codestream

CC := g++
CCFLAGS := -g -Wall -std=gnu++2a
LIBS := pthread
CCFILES :=
OBJS := 
DIRS := inc bin src

vpath %.cc ./src
vpath %.h ./inc

%.o:%.cc
	$(CC) $(CCFLAGS) -o $@ -c $< -Iinc

CCFILES += $(wildcard $(foreach cf, $(DIRS), $(cf)/*.cc))
OBJS := $(patsubst %.cc, %.o, $(CCFILES))
LIBS := $(patsubst %, -l%, $(LIBS))

codestream:
	@echo "uncompleted"
	echo $(LIBS)

codestream_abstract.o: codestream_abstract.cc

ops_def.o: operation_definition.cc
ops_test.o: ops_test.cc
ops_wrapper.o: ops_wrapper.cc operation_definition.cc
ops_wrapper_test.o: ops_wrapper_test.cc

.PHONY: catm clean ops_test
catm: codestream_abstract_test.o codestream_abstract.o
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBS)

ops_test: ops_test.o operation_definition.o
	$(CC) $(CCFLAGS) -o $@ $^

clean:
	@rm -f *.o
	@rm -f $(foreach d, $(DIRS), $(d)/*~)
