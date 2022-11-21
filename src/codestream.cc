#include"codestream.h"
#include<memory>
#include<cstring>

//  DATA SOURCE
//    dinputs::DFFILE  - data from file
//    dinputs::DFSTDIN - data from stdin
//    dinputs::DFCMD   - data from command line

//  codestream_main - codestream object
codestream::Codestream codestream_main;

//  main_error_code - explain what error was occured.
//    #  caller should set it to ENOE before invokes any function which
//    #  would changes this variable.
unsigned short main_error_code(ENOE);

//  if_f_got - just a flag to distingulish DATA SOURCE.
static unsigned char if_f_got(0);

//  dinputs - data input stream for main.
//    i dint constructure a new class,just
//    use some variables and functions to 
//    abstract it.
//    dinputs_XX functions would as abstract
//    interfaces.
namespace dinputs {

  unsigned short dinputs_init(unsigned short data_from, const char *t);
  size_t dinputs_read(char *dest, size_t n);
  void dinputs_close(void);
  unsigned short dinputs_iseof(void);
  unsigned short dinputs_isbad(void);
  unsigned short dinputs_isavailable(void);

  namespace __dis {
    enum {
      DFFILE,
      DFSTDIN,
      DFCMD
    };

    static char data_src(-1);

    static std::unique_ptr<std::ifstream> dis_f(nullptr);
    static std::istream *dis_stdin(nullptr);

    enum { CMD_MAX_SIZE = 256 };
    static std::unique_ptr<char> dis_cmd(nullptr);
    static size_t count_dis_cmd(0);
    static size_t length_dis_cmd(0);

    unsigned short init_dis(unsigned short data_from, const char *t);
    size_t read_dis(char *dest, size_t n);
    void close_dis(void);
    unsigned short iseof_dis(void);
    unsigned short isbad_dis(void);
    unsigned short isalive_dis(void);

  }  //  end of __dis



  using __dis::DFFILE;
  using __dis::DFSTDIN;
  using __dis::DFCMD;
}  //  end of dinputs

//  main_optionf_eandd - do some prepare works for encode or decode.
//    @target : const char pointer,it should be the same char pointer
//              from cmd.
//    return - -1 or 0, @main_error_code would be setted while -1 was
//             returned.
int main_optionf_eandd(const char *target)
{
  unsigned short src_from(dinputs::DFCMD);
  if (!target) {
    main_error_code = EOPTION;
    return -1;
  }

  //  determine where data from.
  if (!strcmp(target, "-") && if_f_got)
    src_from = dinputs::DFSTDIN;
  else if (if_f_got)
    src_from = dinputs::DFFILE;

  //  try to create stream.
  (void)dinputs::dinputs_init(src_from, target);
  if (!dinputs::dinputs_isavailable()) {
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
    codestream_main.installProcedure(toinstall[i]);
    if (!codestream_main.is_execsuccess()) {
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
    //  read data from stream.
    record_length = dinputs::dinputs_read(gcs->buff1, once_read);
    if (record_length == 0)
      if (dinputs::dinputs_iseof()) {
	break;
      } else if (dinputs::dinputs_isbad()) {
	main_error_code = ECODING;
	break;
      }

    //  ready to coding.
    gcs->length_of_buff1 = record_length;
    codestream_main.coding(static_cast<void *>(gcs));
    codestream_main.waitForStateMove();
    if (codestream_main.is_shutdown())
      goto output_content;
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


namespace dinputs {
  unsigned short dinputs_init(unsigned short data_from, const char *t)
  {
    return __dis::init_dis(data_from, t);
  }

  size_t dinputs_read(char *dest, size_t n)
  {
    return __dis::read_dis(dest, n);
  }

  void dinputs_close(void)
  {
    __dis::close_dis();
  }

  unsigned short dinputs_iseof(void)
  {
    return __dis::iseof_dis();
  }

  unsigned short dinputs_isbad(void)
  {
    return __dis::isbad_dis();
  }

  unsigned short dinputs_isavailable(void)
  {
    return __dis::isalive_dis();
  }

  namespace __dis {
    //  init_dis - do initialization to dinputs.
    //    @data_from : where data from -
    //                   DFFILE | DFSTDIN | DFCMD
    //    @t : target.
    //  return - 1 in sucessed,0 in failed.
    inline
    unsigned short init_dis(unsigned short data_from, const char *t) 
    {
      std::ifstream *f(nullptr);

      switch (data_from) {
      case DFFILE:
	f = new std::ifstream;
	if (!f)
	  return 0;
	dis_f.reset(f);
	dis_f->open(t, std::ios_base::in);
	if (!dis_f->is_open()) {
	  dis_f.reset(nullptr);
	  return 0;
	}
	break;

      case DFSTDIN:
	dis_stdin = &std::cin;
	break;

      case DFCMD:
	{
	  //  because no any method can retrun a
	  //  non-const pointer from unique_str,
	  //  thus declare @b in initialization stage.
	  char *b(new char[CMD_MAX_SIZE]);
	  if (!b)
	    return 0;
	  length_dis_cmd = strlen(t);
	  if (length_dis_cmd > CMD_MAX_SIZE)  //  there has a length limiter.
	    return 0;
	  memcpy(b, t, length_dis_cmd);
	  dis_cmd.reset(b);
	}
	break;

      default:
	return 0;
      }

      data_src = data_from;
      return 1;
    }

    //  read_dis - read from dinputs.
    //    @dest : where the data was readed to save.
    //    @n : size to read.
    //    return - readed size.
    //    #  it works should alike cin.read().
    inline
    size_t read_dis(char *dest, size_t n)
    {
      if (!dest)
	return 0;

      //  DFFILE and DFSTDIN almost same case,
      //  but input stream is different.
      switch (data_src) {
      case DFFILE:
	dis_f->read(dest, n);
	return dis_f->gcount();

      case DFSTDIN:
	dis_stdin->read(dest, n);
	return dis_stdin->gcount();

      case DFCMD:
	if (iseof_dis())
	  return 0;
	n = (length_dis_cmd < n) ? length_dis_cmd : n;
	memcpy(dest, dis_cmd.get() + count_dis_cmd, n);
	count_dis_cmd += n;
	length_dis_cmd -= n;
	return n;
      default:;
      }

      return 0;
    }

    //  close_dis - close dinputs.
    //    #  closing,dont recycle anything.
    inline
    void close_dis(void)
    {
      if (dis_f)
	dis_f->close();
      else if (dis_stdin)
	dis_stdin->setstate(std::ios_base::eofbit);
      else
	length_dis_cmd = 0, count_dis_cmd = CMD_MAX_SIZE;
    }

    //  iseof_dis - if no more.
    inline
    unsigned short iseof_dis(void)
    {
      if (dis_f)
	return dis_f->eof();
      else if (dis_stdin)
	return dis_stdin->eof();
      else
	return !length_dis_cmd;
    }

    //  isbad_dis - if bad stream.
    inline
    unsigned short isbad_dis(void)
    {
      if (dis_f)
	return dis_f->bad() || dis_f->fail();
      else if (dis_stdin)
	return dis_stdin->bad() || dis_stdin->fail();
      else
	return 0;
    }

    //  isalive_dis - if dinputs is available.
    inline
    unsigned short isalive_dis(void)
    {
      return data_src != -1;
    }

  }
}
