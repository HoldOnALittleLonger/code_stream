#include"codestream_abstract.h"
#include<iostream>

void func(void) {
  std::cout<<"func calling\n";
}

void *func_wrapper(void *) {
  func();
  return nullptr;
}
