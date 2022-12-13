#include<iostream>

#include"codestream_abstract.h"



void *func1(void *arg) {
  int *a((int *)arg);
  for (auto i(0); i < 100000; ++i);
  *a += 1;
  return arg;
}

void *func2(void *arg) {
  int *a((int *)arg);
  for (auto i(0); i < 200000; ++i);
  *a *= 2;
  return arg;
}

void *func3(void *arg) {
  int *a((int *)arg);
  for (auto i(0); i < 200000; ++i);
  *a *= 3;
  return arg;
}
