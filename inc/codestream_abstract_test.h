#include"codestream_abstract.h"
#include<iostream>

void func(void) {
  std::cout<<"func calling\n";
  for (auto i(0); i < 100000; ++i)
    i = i + 1;
  std::cout<<"func end\n";
}

void *func_wrapper(void *) {
  func();
  return nullptr;
}
