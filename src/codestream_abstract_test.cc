#include"codestream_abstract_test.h"
#include<unistd.h>

using namespace codestream;

int main(void) {
  int value(32);
  int *a(nullptr);
  int r(0);
  std::string cdsret("fff");

  Codestream cds;
  std::function<void *(void *)> f1, f2, f3, f4;
  f1 = nullptr;
  f2 = func1;
  f3 = func2;
  f4 = func3;

  cds.installProcedure(std::move(f1));
  cds.installProcedure(std::move(f2));
  cds.installProcedure(std::move(f3));
  cds.installProcedure(std::move(f4));

  cds.coding(&value);
  cds.stopCode();

  if (cds.is_suspend()) {
    std::cerr<<"program suspend\n";
    a = (int *)cds.getLastResult();
    r = (a) ? *a : r;
    std::cout<<"value is "<<r<<std::endl;
  } else {
    std::cerr<<"suspend error,try to recover\n";
    try {
      cds.programErrorRecover();
    } catch (std::string errs) {
      std::cerr<<errs<<std::endl;
      return 1;
    }
    if (cds.is_processing()) {
      std::cerr<<"program recovered from error,now processing\n";
      goto codestream_processing;
    } else {
      std::cerr<<"program recovered failed\n";
      return 2;
    }

  }

  sleep(3);
  cds.restartCode();
 codestream_processing:
  while (1) {
    cds.waitForStateMove();
    if (cds.is_shutdown()) {
      std::cout<<"wait task done"<<std::endl;
      break;
    }
    sleep(1);
  }
  cdsret = cds.processStateExplain();
  std::cout<<cdsret<<std::endl;
  a = (int *)cds.getLastResult();
  r = (a) ? *a : r;
  std::cout<<"value is "<<r<<std::endl;
  return 0;
}
