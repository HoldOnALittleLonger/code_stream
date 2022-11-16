#ifndef _CODESTREAM_H_
#define _CODESTREAM_H_

#include"ops_wrapper.h"
#include"codestream_abstract.h"
#include<cstdlib>
#include<fstream>
#include<iostream>
#include<concepts>

//  DATA SOURCE
//    FFILE - from file
//    FSTDIN - from stdin
//    FCMD - from command line
#define FFILE  0
#define FSTDIN 1
#define FCMD   2

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
//    EENCODE      - occur error when encoding
//    EDECODE      - occur error when decoding
//    EFPERMISSION - deny permission to access file
//    EINIT        - init program failed.
#define EINVALIDKEY	0
#define EOPTION 	1
#define EENCODE 	2
#define EDECODE 	3
#define EFPERMISSION 	4
#define EINIT 		5

extern std::istream *data_src;
extern codestream::Codestream codestream_main;
extern ops_wrapper::gcstruct gcs;
extern unsigned short src_from;
extern unsigned short main_error_code;

//  main_optionf_<option> - main program option function for <option>.

//  main_optionf_k - set key value for ops_wrapper
template<class T>
requires std::same_as<T, const char *> || std::integral<T> || std::floating_point<T>
int main_optionf_k(T key);

//  main_optionf_e - does init works for encoding.
int main_optionf_e(const char *target);

//  main_optionf_d - does decode works for decoding.
int main_optionf_d(const char *target);

//  main_optionf_f - indicate data from file.
int main_optionf_f(void);

//  main_create_stram - create stream for prepare to read data.
//  This function should be STATIC in *.cc file.
//int main_create_stream(unsigned short which_case);

//  main_coding - start encoding ot decoding.
//    @gcs : the general coding structure object pointer.
int main_coding(ops_wrapper::gstruct *gcs);

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



#endif // HEAD END
