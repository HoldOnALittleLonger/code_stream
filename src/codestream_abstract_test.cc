#include"codestream_abstract_test.h"
#include<thread>

#include<unistd.h>


codestream::Codestream cds;

void thread_func(void) {
  cds.startCode(nullptr);
}

int main(void) {
  std::function<void *(void *)> a(func_wrapper), b(func_wrapper);

  codestream::codestream_process_state cps(codestream::INVAILD);

  cds.installProcedure(std::move(a));
  cds.installProcedure(std::move(b));

  std::thread t1(thread_func);

  for (auto i(0); i < 5; ++i) {
  std::cerr<<"entry stop\n"<<std::endl;
  cds.stopCode();
  std::cerr<<"exit stop\n"<<std::endl;
  if (cds.is_suspend()) {
    std::cout<<"code suspend!"<<std::endl;
    break; 
  }
  else
    std::cerr<<"suspend unactive"<<std::endl;
  }
  cds.uninstallProcedure(2 - 1);
  
  sleep(3);
  cds.restartCode();
  t1.join();

  if (cds.is_shutdown())
    std::cout<<cps<<std::endl;
  else
    std::cerr<<"Exec failed"<<std::endl;


  return 0;
}
