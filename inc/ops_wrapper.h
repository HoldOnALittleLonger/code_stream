#ifndef _OSP_WRAPPER_H_
#define _OPS_WRAPPER_H_

#include<cstddef>

//  OTM_INIT_EXCEPT - occur exception while init otm 
#define OTM_INIT_EXCEPT 1

//  BASE64_INIT_EXCEPT - occur exception while init base64
#define BASE64_INIT_EXCEPT 2

namespace ops_wrapper {

  //  struct general_coding_struct - data structure for coding.
  //    @buffx : buffer.
  //    @size_of_xxx : records size of @buffx.
  //    @length_of_xxx : records the total is how many bytes for data in @buffx.
  struct general_coding_struct {
    char *buff1;
    size_t size_of_buff1;
    size_t length_of_buff1;

    char *buff2;
    size_t size_of_buff2;
    size_t length_of_buff2;
  };  //  should be 48B
  using gcstruct = struct general_coding_struct;

  //  wrappers for otm
  void *ops_wrapper_otm_encode(void *gcs);
  void *ops_wrapper_otm_decode(void *gcs);

  //  wrappers for base64
  void *ops_wrapper_base64_encode(void *gcs);
  void *ops_wrapper_base64_decode(void *gcs);

  //  general coding work together procedure.
  //  all code procedure should has an aid procedure
  //  to help anothers use gcs correctly.
  //  E.g.
  //    otm-encode -> ops_wrapper_gcwt -> base64-encode -> END
  //    otm-encode -> END
  //    #  decode as same.
  void *ops_wrapper_gcwt(void *gcs);

  //  wrappers initialization

  //  gc_keys_ul - unsigned long key-values for general coding.
  union gc_keys_ul {
    unsigned long otm_key;
    //  unsigned long base64_key;
    
    //  ... more
  };

  //  must call it before using any ops-wrappers.
  void ops_wrapper_init(void) noexcept(false);

  //  set key for otm.
  void ops_wrapper_otm_key(unsigned short);

  // set key for base64.
  //  void ops_wrapper_base64_key(unsigned short);


}  //  namespace end

#endif  // head end
