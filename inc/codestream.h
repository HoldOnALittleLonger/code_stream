#ifndef _CODESTREAM_H_
#define _CODESTREAM_H_

#include"ops_wrapper.h"
#include"codestream_abstract.h"
#include<cstdlib>
#include<fstream>
#include<iostream>
#include<concepts>

//  OPTION_STRING
//    -k - key value
//    -e - encode mode
//    -d - decode mode
//    -f - from file
//    -h - print help message
#define OPTION_STRING "-k:-e:-d:-fh"

//  ERROR CODE
//    EINVALIDKEY  - invalid key
//    EOPTION      - unknown option or use option incorrectly
//    ENILP        - nullptr
//    ECODING      - error occured while coding.
//    EFPERMISSION - deny permission to access file
//    EINIT        - init program failed.
enum {
  EINVALIDKEY = 0,	
  EOPTION,
  ENILP,
  ECODING,
  EFPERMISSION,
  EINIT,
  ENOE
};

#define FTOINSTALL_NUM 3

extern std::istream *data_src;
extern codestream::Codestream codestream_main;
extern unsigned char if_f_got;
extern unsigned short main_error_code;
extern void *(*toinstall[FTOINSTALL_NUM])(void *);

//  main_optionf_<option> - main program option function for <option>.

//  main_optionf_k - set key value for ops_wrapper
template<class T>
requires std::same_as<T, const char *> || std::integral<T> || std::floating_point<T>
int main_optionf_k(T key);

//  main_optionf_eandd - does init works for encoding and decoding.
int main_optionf_eandd(const char *target);

//  main_optionf_f - indicate data from file.
int main_optionf_f(void);

//  main_coding - start encoding ot decoding.
//    @gcs : the general coding structure object pointer.
int main_coding(ops_wrapper::gcstruct *gcs);

//  main_optionf_h - just output help.
void main_optionf_h(void)
{
  using std::cout;
  using std::endl;
  cout<<"HELP MESSAGE : "<<endl
      <<"  usage : <program> <options>"<<endl
      <<"    options : "<<endl
      <<"      -k <key-value> : set key value for coding."<<endl
      <<"      -e <target>    : encode <target>."<<endl
      <<"      -d <target>    : decode <target>."<<endl
      <<"      -f             : read data from file."<<endl
      <<"      -h             : print these messages."<<endl
      <<"    # dont use -e and -d at same time."<<endl
      <<"    # target could be file-name or string."<<endl
      <<"    # if provide file-name,must assign -f option."<<endl
      <<"    # symbol - means stdin."<<endl;
}

template<class T>
int main_optionf_k(T key)
{
  ops_wrapper::ops_wrapper_otm_key(key);
  return 0;
}

template<>
int main_optionf_k(const char *key)
{
  unsigned long ulkey(0);
  if (!key)
    return -1;

  ulkey = strtoul(key, nullptr, 10ul);
  ops_wrapper::ops_wrapper_otm_key(ulkey);
  return 0;
}

void main_output_error_msg(decltype(main_error_code) e)
{
  using std::cerr;
  using std::endl;

  #define CMESTR "codestream_main : error : "

  switch (e) {
  case EINVALIDKEY:
    cerr<<CMESTR "key value is not a valid type."<<endl;
    break;

  case EOPTION:
    cerr<<CMESTR "incorrectly used option."<<endl;
    break;
   
  case ENILP:
    cerr<<CMESTR "code error,nullptr."<<endl;
    break;

  case ECODING:
    cerr<<CMESTR "error occured while coding,maybe data reading fault."<<endl;
    break;

  case EFPERMISSION:
    cerr<<CMESTR "open file was failed,maybe deny permission."<<endl;
    break;

  case EINIT:
    cerr<<CMESTR "init program failed."<<endl;
    break;

  case ENOE:
    cerr<<CMESTR "code error,not an error."<<endl;
    break;

  default:
    cerr<<CMESTR "code error,unknown error."<<endl;
  }

  #undef CMESTR
}



#endif // HEAD END
