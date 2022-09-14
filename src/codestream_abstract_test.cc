#include"codestream_abstract_test.h"

int main(void) {
  std::function<void *(void *)> a(func_wrapper), b(func_wrapper);
  codestream::Codestream cds;
  codestream::codestream_process_state cps(codestream::INVAILD);
  cds.installProcedure(std::move(a));
  if (!cds.is_execsuccess()) {
    std::cerr<<"Install failed"<<std::endl;
    return 1;
  }

  cds.installProcedure(std::move(b));
  if (!cds.is_execsuccess()) {
    std::cerr<<"Install failed"<<std::endl;
    return 1;
  }

  cps = cds.startCode(nullptr);
  if (cds.is_shutdown())
    std::cout<<cps<<std::endl;
  else
    std::cerr<<"Exec failed"<<std::endl;


  return 0;
}
