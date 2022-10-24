all: codestream

CC := g++
CCFLAGS := -g -Wall -std=gnu++2a
LIBS := pthread
OBJS := 

vpath %.cc ./src
vpath %.h ./inc

%.o:%.cc
	$(CC) $(CCFLAGS) -o $@ -c $< -Iinc

OBJS += codestream_abstract_test.o
OBJS += codestream_abstract.o
OBJS += ops_def.o

LIBS := $(patsubst %,-l%,$(LIBS))

codestream:
	@echo "uncompleted"
	echo $(LIBS)

codestream_abstract.o: codestream_abstract.cc

ops_def.o: operation_definition.cc

.PHONY: catm clean
catm: codestream_abstract_test.o codestream_abstract.o
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBS)

clean:
	@rm -f *.o
