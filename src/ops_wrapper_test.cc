#include<cstring>
#include<iostream>

#include"ops_wrapper.h"

#define BUFF_SIZE 512

using std::cout;
using std::cerr;
using std::endl;

const char *plaintext("kksk");

int main(void)
{
  using namespace ops_wrapper;
  gcstruct gcs, base64gcs;
  gcstruct *gcsp(&gcs);
  gcstruct *retp(nullptr);
  memset(&gcs, 0, sizeof(gcs));

  gcs.buff1 = new char[BUFF_SIZE];
  gcs.buff2 = new char[BUFF_SIZE];
  gcs.size_of_buff1 = gcs.size_of_buff2 = BUFF_SIZE;

  if (!gcs.buff1 || !gcs.buff2) {
    cerr<<"memory allocate failed"<<endl;
    return -1;
  }

  ops_wrapper_otm_key(0);
  try {
    ops_wrapper_init();
  } catch (int e) {
    if (e == OTM_INIT_EXCEPT) {
      cerr<<"init otm failed"<<endl;
      return -2;
    }
  }

  strncpy(gcsp->buff1, plaintext, strlen(plaintext));
  gcsp->length_of_buff1 = strlen(gcsp->buff1);
  base64gcs = gcs;

  retp = static_cast<gcstruct *>(ops_wrapper_otm_encode(static_cast<void *>(gcsp)));
  if (!retp) {
    cerr<<"ops_wrapper_otm_encode() failed"<<endl;
    return -3;
  }
  (retp->buff2)[retp->length_of_buff2] = '\0';

  cout<<retp->buff1<<endl;
  cout<<retp->buff2<<endl;

  gcsp = &base64gcs;
  retp = static_cast<gcstruct *>(ops_wrapper_base64_encode(static_cast<void *>(gcsp)));
  if (!retp) {
    cerr<<"ops_wrapper_base64_encode() failed"<<endl;
    return -3;
  }
  (retp->buff2)[retp->length_of_buff2] = '\0';

  cout<<retp->buff1<<endl;
  cout<<retp->buff2<<endl;

  delete[] gcsp->buff1;
  delete[] gcsp->buff2;

  return 0;
}
