#ifndef _CODESTREAM_H_
#define _CODESTREAM_H_

#include<cstdlib>
#include<fstream>
#include<iostream>
#include<concepts>

#include"codestream_abstract.h"
#include"ops_wrapper.h"

//  Options string
extern const char *const OPTION_STRING;

//  ERROR CODE
//    EINVALIDKEY  - invalid key
//    EOPTION      - unknown option or use option incorrectly
//    ENILP        - nullptr
//    ECODING      - error occured while coding.
//    EFPERMISSION - deny permission to access file
//    EINIT        - init program failed.
enum {
  EINVALIDKEY,
  EOPTION,
  ENILP,
  ECODING,
  EFPERMISSION,
  EINIT,
  ENOE
};

//  Buffer size
constexpr unsigned int GCS_BUFF_SIZE(4096);
//  Number of functions to be installed
constexpr unsigned short FTOINSTALL_NUM(3);

extern unsigned short main_error_code;
extern void *(*toinstall[FTOINSTALL_NUM])(void *);

//  main_optionf_<option> - main program option function for <option>.

//  main_optionf_k - set key value for ops_wrapper
template<class T>
requires std::integral<T> || std::floating_point<T>
int main_optionf_k(T key)
{
  ops_wrapper::ops_wrapper_otm_key(key);
  return 0;
}

//  template function overlord.
template<class T>
requires std::same_as<T, char *>
int main_optionf_k(const T key)
{
  unsigned long ulkey(0);
  if (!key) {
    main_error_code = EINVALIDKEY;
    return -1;
  }

  ulkey = strtoul(key, nullptr, 10ul);
  ops_wrapper::ops_wrapper_otm_key(ulkey);
  return 0;
}



//  main_optionf_eandd - does init works for encoding and decoding.
int main_optionf_eandd(const char *target);

//  main_optionf_f - indicate data from file.
void main_optionf_f(void);

//  main_coding - start encoding ot decoding.
//    @gcs       : the general coding structure object pointer.
//    @once_read : how many data should read at once cycle.
int main_coding(ops_wrapper::gcstruct *gcs, ssize_t once_read);

//  main_optionf_h - just output help.
void main_optionf_h(void);

//  main_output_error_msg - output error messages.
void main_output_error_msg(decltype(main_error_code) e);

#endif // HEAD END
