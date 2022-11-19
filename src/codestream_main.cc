#include"codestream.h"
#include"recycle_atexit.h"
#include<unistd.h>
#include<cstring>

//  toinstall - function array for install.
void *(*toinstall[FTOINSTALL_NUM])(void *);

int main(int argc, char *argv[])
{
  //  -e or -d just one effective.
  unsigned char prevent_ef(0);
  char option('\0');
  char *cmdarg(nullptr);
  ops_wrapper::general_coding_struct gcs;
  ratexit::recycle_atexit<char, 2> recycle_gcs;

  //  number for options should be given is 3 at latest.
  if (argc < 3) {
    main_optionf_h();
    return -EOPTION;
  }

  memset(toinstall, 0, sizeof(void *(*)(void *)) * FTOINSTALL_NUM);
  memset(&gcs, 0, sizeof(gcs));

  gcs.buff1 = new char[GCS_BUFF_SIZE];
  gcs.buff2 = new char[GCS_BUFF_SIZE];
  if (!gcs.buff1 || !gcs.buff2) {
    std::cerr<<"codestream_main : error : allocate memory for buffer fault."
	     <<std::endl;
    return -ENILP;
  }
  gcs.length_of_buff1 = gcs.length_of_buff2 = 0;
  gcs.size_of_buff1 = gcs.size_of_buff2 = GCS_BUFF_SIZE;

  recycle_gcs.addObjToRecycle(gcs.buff1);
  recycle_gcs.addObjToRecycle(gcs.buff2);

  //  analyse options.
  while ((option = getopt(argc, argv, OPTION_STRING)) != -1) {
    switch (option) {
    case 'k':
      main_error_code = ENOE;
      if (main_optionf_k(optarg) < 0) {
	main_output_error_msg(main_error_code);
	return main_error_code * -1;
      }
      break;

    case 'e':
      if (!prevent_ef) {
	prevent_ef = 1;
	toinstall[0] = ops_wrapper::ops_wrapper_otm_encode;
	toinstall[1] = ops_wrapper::ops_wrapper_gcwt;
	toinstall[2] = ops_wrapper::ops_wrapper_base64_encode;
	cmdarg = optarg;
      }
      break;

    case 'd':
      if (!prevent_ef) {
	prevent_ef = 1;
	//  have to install decode function in recursive order to
	//  encode.
	toinstall[0] = ops_wrapper::ops_wrapper_base64_decode;
	toinstall[1] = ops_wrapper::ops_wrapper_gcwt;
	toinstall[2] = ops_wrapper::ops_wrapper_otm_decode;
	cmdarg = optarg;
      }
      break;

    case 'f':
      main_optionf_f();
      break;

    case 'h':
      main_optionf_h();
      return 0;

    default:
      std::cerr<<"codestream_main : error : unknown option - "<<option
	       <<std::endl;
      return -EOPTION;
    }
  }


  //  if prevent_ef == 0,that means 
  //  neither -e nor -d was given.
  if (prevent_ef) {
  try_wrapper_init:
    unsigned char init_counter(3);  //  hard coding counter.
    try {
      ops_wrapper::ops_wrapper_init();  //  init wrapper
    } catch (int e) {
      //  report exception.
      switch (e) {
      case OTM_INIT_EXCEPT:
	std::cerr<<"codestream_main : exception : otm init failed."
		 <<std::endl;
	break;

      case BASE64_INIT_EXCEPT:
	std::cerr<<"codestream_main : exception : base64 init failed."
		 <<std::endl;
	break;

      default:
	std::cerr<<"codestream_main : exception : unknown exception."
		 <<std::endl;
      }

      if (init_counter-- > 0)
	goto try_wrapper_init;
      else
	return -EINIT;
    }

    main_error_code = ENOE;
    if (main_optionf_eandd(cmdarg) < 0) {
      main_output_error_msg(main_error_code);
      return main_error_code * -1;
    }
  }
  else
    return -EOPTION;
  
  //  already to coding.
  main_error_code = ENOE;
  if (main_coding(&gcs, gcs.size_of_buff1 / 4) < 0) {
    main_output_error_msg(main_error_code);
    return main_error_code * -1;
  }

  return 0;
}


