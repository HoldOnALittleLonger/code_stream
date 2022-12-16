#include<memory>
#include<exception>

#include"ops_wrapper.h"
#include"operation_definition.h"

namespace ops {
  static std::unique_ptr<otm::otm_object> otm_bewrapped(nullptr);
  static std::unique_ptr<base64::base64_object> base64_bewrapped(nullptr);

  //  otm_init - set key for otm object which was wrapped.
  //    @x : the key.
  //    #  if that object had not been inited,it would do nothing.
  static void otm_init(unsigned short x)
  {
    if (!otm_bewrapped)
      return;
    otm_bewrapped->setResortKey(x);
  }

}  //  ops END


//  owa - ops_wrapper_aid namespace.
namespace owa {

  using ops_wrapper::general_coding_struct;
  using ops_wrapper::gcstruct;

  //  ow_gcs_all_wt - work together between procedures.
  //    @gcs : the pointer points to general coding structure object.
  //    #  these procedures must be such procedure
  //    #  that use all members of gcstruct.
  void ow_gcs_all_wt(gcstruct *gcs)
  {
    char *tmpp(nullptr);
    size_t tmpv(0);

    if (!gcs)
      return;
    
    tmpp = gcs->buff1;
    gcs->buff1 = gcs->buff2;
    gcs->buff2 = tmpp;

    tmpv = gcs->size_of_buff1;
    gcs->size_of_buff1 = gcs->size_of_buff2;
    gcs->size_of_buff2 = tmpv;

    tmpv = gcs->length_of_buff1;
    gcs->length_of_buff1 = gcs->length_of_buff2;
    gcs->length_of_buff2 = tmpv;
  }



}  //  owa END 


namespace ops_wrapper {

  //  let ops_wrapper_init() just be invoked at onece.
  static unsigned short ops_wrapper_init_record = 0;

  //  cwer - code wrapper execute record.
  //    records which ops object was worked in last time.
  static unsigned short cwer = 0;
#define CWER_OTM 0
#define CWER_BASE64 1

  static union gc_keys_ul gckul;

  //  construct_ops_wrappers - constructure ops_wrappers.
  static void construct_ops_wrappers(void)
  {
    otm::otm_object *otmp = new otm::otm_object;
    base64::base64_object *base64p = new base64::base64_object;
    ops::otm_bewrapped.reset(otmp);
    ops::base64_bewrapped.reset(base64p);

    //  ... more

  }

  //  if_wrappers_exist - check if ops_wrappers existed realy.
  //    # if there has any object isnt existed,then throw exception.
  static void if_wrappers_exist(void) noexcept(false)
  {
    if (!ops::otm_bewrapped)
      throw OTM_INIT_EXCEPT;
    
    if (!ops::base64_bewrapped)
      throw BASE64_INIT_EXCEPT;
  }

  //  generateOPSWrapper - template for generate a ops_wrapper
  //    @s       : gcstruct buffer 
  //    @__cref  : a reference type of class type _CType
  //    @__cfunc : a pointer point to a class method
  //    return - a lambda function {
  //               captures : @s, &@__cref, &@__cfunc
  //               return - @s
  //             }
  //   #  the difference bewteen encode and decode just at object and 
  //      mehtod is not same.then make an abstract at there to reduce
  //      complexity,but size of code is same.
  template<class _Tp_CType, typename _Tp_CFunc>
  static auto generateOPSWrapper(gcstruct *s, _Tp_CType &__cref, _Tp_CFunc __cfunc)
  {
    return [&](void) -> void *
      {
	ssize_t result(0);
	if (!s)
	  return nullptr;
	result = (__cref.*__cfunc)(s->buff1, s->length_of_buff1,
				  s->buff2, s->size_of_buff2);
	s->length_of_buff2 = (result > 0) ? result : 0;
	return s;
      };
  }
  //  !!  I tried to put @s at the enf of arg-list,but received SIGSEGV,and @this of otmEncode
  //      points to another otm_object.otm_object::_q_text points to an address out of bound.
  //      but in gdb interaction debugging,__cref == the static otm_object.
  //      STRANGE
  //      put @s at end of arg-list,@this of otm_object::otmEncode points to an object in stack
  //      put @s at first,@this of otm_object::otmEncode points to an object in static segement

  //  ops_wrapper_otm_encode - wrapper for otm encode.
  //    @gcs : data struct pointer point to operand.
  //    return - return @gcs if working fine,otherwise,return nullptr.
  void *ops_wrapper_otm_encode(void *gcs)
  {
    auto f = generateOPSWrapper(static_cast<gcstruct *>(gcs),
				*ops::otm_bewrapped, &otm::otm_object::otmEncode);
    if (!f())
      return nullptr;
    cwer = CWER_OTM;
    return gcs;
  }

  //  ops_wrapper_otm_decode - wrapper for otm decode.
  //    @gcs : data struct pointer point to operand.
  //    return - return @gcs if working fine,otherwise,return nullptr.
  void *ops_wrapper_otm_decode(void *gcs)
  {
    auto f = generateOPSWrapper(static_cast<gcstruct *>(gcs),
				*ops::otm_bewrapped, &otm::otm_object::otmDecode);
    if (!f())
      return nullptr;
    cwer = CWER_OTM;
    return gcs;
  }

  //  ops_wrapper_base_encode - wrapper for base64 encode.
  //    @gcs : data struct pointer point to operand.
  //    return - return @gcs if working fine,other wise,return nullptr.
  void *ops_wrapper_base64_encode(void *gcs)
  {
    auto f = generateOPSWrapper(static_cast<gcstruct *>(gcs),
				*ops::base64_bewrapped,	&base64::base64_object::base64Encode);
    if (!f())
      return nullptr;
    cwer = CWER_BASE64;
    return gcs;
  }

  //  ops_wrapper_base_decode - wrapper for base64 decode.
  //    @gcs : data struct pointer point to operand.
  //    return - return @gcs if working fine,other wise,return nullptr.
  void *ops_wrapper_base64_decode(void *gcs)
  {
    auto f = generateOPSWrapper(static_cast<gcstruct *>(gcs),
				*ops::base64_bewrapped,	&base64::base64_object::base64Decode);
    if (!f())
      return nullptr;
    cwer = CWER_BASE64;
    return gcs;
  }

  //  ops_wrapper_gcwt - general code procedure work together 
  //    @gcs : @gcs should be a pointer returned by any code procedure.
  //    return - return @gcs after do something for work-together.
  void *ops_wrapper_gcwt(void *gcs)
  {
    gcstruct *s = static_cast<gcstruct *>(gcs);
    if (!s)
      return nullptr;

    switch (cwer) {
    case CWER_OTM:
      owa::ow_gcs_all_wt(s);
      break;

    case CWER_BASE64:
      owa::ow_gcs_all_wt(s);
      break;

    default:  //  nodefined,do nothing.
      ;
    }

    return s;
  }

  //  ops_wrapper_init - initializer for all ops object.
  //    #  should invoke other procedure to set key value for
  //    #  the object before initializer was called.
  //    #  this procedure can only be called by once.
  //    #  some procedure may be throws some exceptions,but 
  //    #  initializer does not catch them,all exceptions should
  //    #  be caugh by external procedure which using ops_wrapper.
  void ops_wrapper_init(void) noexcept(false)
  {
    //  dont init wrappers for twice or more.
    if (ops_wrapper_init_record)
      return;

    construct_ops_wrappers();
    if_wrappers_exist();

    //  -- init procedures chain

    ops::otm_init(gckul.otm_key);    

    //  ... more

    //  init procedures chain --

    ops_wrapper_init_record = 1;
  }

  //  ops_wrapper_otm_key - set key for otm.
  //    @x : the key.
  void ops_wrapper_otm_key(unsigned short x)
  {
    gckul.otm_key = x;  //  just set the union member 
  }

}  //  namespace end
