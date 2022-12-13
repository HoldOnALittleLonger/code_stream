#include<iostream>

#include"operation_definition.h"

using std::cout;
using std::cerr;
using std::endl;

const char *cptext("kksk");

int main(void)
{
  using namespace otm;
  using namespace base64;

  size_t cptext_len(strlen(cptext));
  ssize_t returnv(0);
  char *buff_en(new char[512]);
  char *buff_de(new char[512]);
  otm_object otm1;
  base64_object b641;


  if (!buff_en || !buff_de) {
    cerr<<"allocate memory failed!"<<endl;
    delete[] buff_en;
    delete[] buff_de;
    return 1;
  }
 
  cout<<"raw text : "<<cptext<<endl;

  memset(buff_en, '\0', 512);
  memset(buff_de, '\0', 512);
  returnv = otm1.otmEncode(cptext, cptext_len, buff_de, 512);
  cout<<"encode text - otm : "<<buff_de<<endl;
  returnv = otm1.otmDecode(buff_de, returnv, buff_en, 512);
  cout<<"decode text - otm : "<<buff_en<<endl;

  memset(buff_en, '\0', 512);
  memset(buff_de, '\0', 512);
  returnv = b641.base64Encode(cptext, cptext_len, buff_de, 512);
  cout<<"encode text - base64 : "<<buff_de<<endl;
  returnv = b641.base64Decode(buff_de, returnv, buff_en, 512);
  cout<<"decode text - base 64 : "<<buff_en<<endl;

  delete[] buff_en;
  delete[] buff_de;

  return 0;
}
