all: codestream

CC := g++
CCFLAGS := -g -Wall -std=gnu++2a

vpath %.cc ./src
vpath %.h ./inc

%.o:%.cc
	$(CC) $(CCFLAGS) -o $@ -c $< -Iinc

codestream:
	@echo "uncompleted"

.PHONY: catm clean
catm: codestream_abstract_test.o codestream_abstract.o
	$(CC) $(CCFLAGS) -o $@ $^

clean:
	@rm -f *.o
