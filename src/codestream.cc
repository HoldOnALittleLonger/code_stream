#include<memory>
#include<cstring>

#include"codestream.h"
#include"csdsin.h"

//  DATA SOURCE
//    csds::dfrom::DFFILE  - data from file
//    csds::dfrom::DFSTDIN - data from stdin
//    csds::dfrom::DFCMD   - data from command line

//  main_error_code - explain what error was occured.
//    #  caller should set it to ENOE before invokes any function which
//    #  would changes this variable.
unsigned short main_error_code(ENOE);

//  if_f_got - just a flag to distingulish DATA SOURCE.
static unsigned char if_f_got(0);

#define g_Csdsin csds::getGlobalCsdsinRef()


//  main_optionf_eandd - do some prepare works for encode or decode.
//    @target : const char pointer,it should be the same char pointer
//              from cmd.
//    return - -1 or 0, @main_error_code would be setted while -1 was
//             returned.
int main_optionf_eandd(const char *target)
{
  csds::dfrom src_from(csds::DFCMD);
  if (!target) {
    main_error_code = EOPTION;
    return -1;
  }

  //  determine where data from.
  if (!strcmp(target, "-") && if_f_got)
    src_from = csds::DFSTDIN;
  else if (if_f_got)
    src_from = csds::DFFILE;

  //  try to create stream.
  g_Csdsin.initCsdsin(src_from, target);
  if (!g_Csdsin.isinit()) {
    //  maybe target is too longer in cmd-line mode,
    //  but dont use error code to distingulish 
    //  file-permission or no enough space.
    //  in face,cmd-line mode is possible has
    //  EFPERMISSION error.
    main_error_code = EFPERMISSION;
    return -1;
  }

  main_error_code = ENOE;
  //  install procedures from @toinstall[].
  for (unsigned short i(0); i < FTOINSTALL_NUM; ++i) {
    g_Codestream.installProcedure(toinstall[i]);
    if (!g_Codestream.is_execsuccess()) {
      main_error_code = EINIT;
      break;
    }
  }

  return (main_error_code == ENOE) ? 0 : -1;
}

//  main_optionf_f - open flag if_f_got.
void main_optionf_f(void)
{
  if_f_got = 1;
}

//  main_coding - procedure to coding.
//    @gcs       : the ops_wrapper::general coding structure pointer.
//    @once_read : how many data should read at once cycle.
//                 #  This argument is size limit on algorithm.
//    return - 0 or -1,main_error_code would be setted while -1
//             was returned.
int main_coding(ops_wrapper::gcstruct *gcs, ssize_t once_read)
{
  ssize_t record_length(0);

  if (!gcs || !gcs->buff1 || !gcs->buff2) {
    main_error_code = ENILP;
    goto main_coding_exit;
  }

  std::cout.flush();  //  flush stream before work.
  do {
    //    std::cerr<<"Debug: ready to reading"<<std::endl;
    //  read data from stream.
    record_length = g_Csdsin.readCsdsin(gcs->buff1, once_read);
    //    std::cerr<<"Debug: rl = "<<record_length<<std::endl;
    if (record_length == 0)
      if (g_Csdsin.iseof()) {
	break;
      } else if (g_Csdsin.isbad()) {
	main_error_code = ECODING;
	break;
      }

    //  ready to coding.
    gcs->length_of_buff1 = record_length;
    g_Codestream.coding(static_cast<void *>(gcs));
    g_Codestream.waitForStateMove();
    if (g_Codestream.is_shutdown())
      goto output_content;
    else if (!g_Codestream.is_execsuccess()) {
      std::cerr<<g_Codestream.processErrorExplain()<<std::endl;
      try {
	g_Codestream.programErrorRecover();
      } catch (std::string &s) {
	std::cerr<<s<<std::endl;
	main_error_code = ECODING;
	goto main_coding_exit;
      }
    } else {
      std::cerr<<g_Codestream.processStateExplain()<<std::endl;
    }

  output_content:
    //  output
    std::cout.write(gcs->buff2, gcs->length_of_buff2);
    std::cout.flush();

  } while (1);

 main_coding_exit:
  return (main_error_code == ENOE) ? 0 : -1;
}

//  main_optionf_h - print help messages.
void main_optionf_h(void)
{
  using std::cerr;
  using std::endl;
  cerr<<"HELP MESSAGE : "<<endl
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

//  main_output_error_msg - print error messages.
//    @e : error code.
void main_output_error_msg(decltype(main_error_code) e)
{
  using std::cerr;
  using std::endl;

  #define CMESTR "codestream_main : error : "

  cerr<<"e is "<<e<<endl;

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
