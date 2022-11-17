#include"codestream.h"
#include<cstring>

//  DATA SOURCE
//    DFFILE  - data from file
//    DFSTDIN - data from stdin
//    DFCMD   - data from command line
#define DFFILE    0
#define DFSTDIN   1
#define DFCMD     2

std::istream *data_src(nullptr);
codestream::Codestream codestream_main;

//  main_error_code - explain what error was occured.
//    #  caller should set it to ENOE before invokes any function which
//    #  would changes this variable.
unsigned short main_error_code(ENOE);

//  if_f_got - just a flag to distingulish DATA SOURCE.
unsigned char if_f_got(0);

//  toinstall - a function array saves the function pointer all will be
//              installed into codestream.
//void *(toinstall *)(void *)[FTOINSTALL_NUM];
//  #  defined in codestream_main.cc

//  previous declare.
static int main_create_stream(unsigned short, const char *);

//  main_optionf_eandd - do some prepare works for encode or decode.
//    @target : const char pointer,it should be the same char pointer
//              from cmd.
//    return - -1 or 0, @main_error_code would be setted while -1 was
//             returned.
int main_optionf_eandd(const char *target)
{
  unsigned short src_from(DFCMD);
  if (!target) {
    main_error_code = EOPTION;
    return -1;
  }

  //  determine where data from.
  if (!strcmp(target, "-") && if_f_got)
    src_from = DFSTDIN;
  else if (if_f_got)
    src_from = DFFILE;

  main_error_code = ENOE;
  //  try to create stream.
  if (main_create_stream(src_from, target) < 0)
    return -1;

  //  install procedures from @toinstall[].
  for (unsigned short i(0); i < FTOINSTALL_NUM; ++i) {
    codestream_main.installProcedure(toinstall[i]);
    if (!codestream_main.is_execsuccess()) {
      main_error_code = EINIT;
      break;
    }
  }

  return (main_error_code == ENOE) ? 0 : -1;
}

//  main_optionf_f - open flag if_f_got.
int main_optionf_f(void)
{
  if_f_got = 1;
  return 0;
}

//  main_coding - procedure to coding.
//    @gcs   : the ops_wrapper::general coding structure pointer.
//    return - 0 or -1,main_error_code would be setted while -1
//             was returned.
int main_coding(ops_wrapper::gcstruct *gcs)
{
  ssize_t once_read(0);
  ssize_t record_length(0);

  if (!gcs || !gcs->buff1 || !gcs->buff2) {
    main_error_code = ENILP;
    return -1;
  }

  //  this it's size limit on coding algorithm.
  once_read = gcs->size_of_buff1 / 4;
  
  do {
    //  read data from stream.
    data_src->read(gcs->buff1, once_read);
    record_length = data_src->gcount();
    if (record_length == 0)
      if (data_src->eof()) {
	break;
      } else if (data_src->bad() || data_src->fail()) {
	main_error_code = ECODING;
	break;
      }
    
    //  ready to coding.
    gcs->length_of_buff1 = record_length;
    do {
      codestream_main.coding(static_cast<void *>(gcs));
      codestream_main.waitForStateMove();
      if (codestream_main.is_shutdown())
	break;
      else if (!codestream_main.is_execsuccess()) {
	std::cerr<<codestream_main.processErrorExplain()<<std::endl;
	try {
	  codestream_main.programErrorRecover();
	} catch (std::string &s) {
	  std::cerr<<s<<std::endl;
	  main_error_code = ECODING;
	  goto main_coding_exit;
	}
      } else {
	std::cerr<<codestream_main.processStateExplain()<<std::endl;
      }
    } while (1);

    //  output
    std::cout.write(gcs->buff2, gcs->length_of_buff2);
    std::cout.flush();

  } while (1);

 main_coding_exit:
  return (main_error_code == ENOE) ? 0 : -1;
}

static std::ifstream *for_recycle(nullptr);
static void recycle_mcs(void)
{
  delete for_recycle;
}

//  main_create_stream - create stream for reading data.
//    @which_case : indicates which case is it.
//                  DFFILE | DFSTDIN | DFCMD
//    @t          : the target string.it maybe data string or file name.
//    return - return 0 if no error occurs,otherwise return -1.
//    #  this function would sets data_src pointer,it is a 
//    #  std::istream pointer.
static int main_create_stream(unsigned short which_case, const char *t)
{
  switch (which_case) {
  case DFFILE:
    {
      std::ifstream *f = new std::ifstream;
      for_recycle = f;
      atexit(recycle_mcs);
      data_src = dynamic_cast<std::istream *>(f);
      if (!data_src) {
	main_error_code = EINIT;
	break;
      }

      //  try to open file.
      //  because system would close file automatically after exit,
      //  so dont registe close function at exit.
      f->open(t, std::ios_base::in);
      if (!f->is_open())
	main_error_code = EFPERMISSION;
    }
    break;
    
  case DFSTDIN:
    data_src = dynamic_cast<std::istream *>(&std::cin);
    if (!data_src)
      main_error_code = EINIT;
    break;
      
  case DFCMD:
    data_src = dynamic_cast<std::istream *>(&std::cin);
    if (!data_src) {
      main_error_code = EINIT;
      break;
    }
    //  put characters in @t back to std::cin.
    for ( ; *t != '\0'; ++t)
      data_src->putback(*t);

    break;

  default:
    main_error_code = EOPTION;
  }

  return (main_error_code == ENOE) ? 0 : -1;
}
