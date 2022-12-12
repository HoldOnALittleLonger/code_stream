#ifndef _CSDSIN_H_
#define _CSDSIN_H_

#include<cstddef>
#include<string>
#include<cstring>
#include<memory>
#include<fstream>

namespace csds {

  enum dfrom : unsigned char {
    DFFILE = 0,
      DFSTDIN,
      DFCMD,
      };

  class Csdsin final {
  public:
    Csdsin();
    ~Csdsin();

    Csdsin(const Csdsin &) = delete;
    Csdsin &operator=(const Csdsin &) = delete;
    Csdsin(Csdsin &&) = delete;
    Csdsin &operator=(Csdsin &&) = delete;

    void initCsdsin(dfrom sK, const char *data_str);
    void closeCsdsin(void);
    std::size_t read(char *dest, std::size_t n);

    friend Csdsin &operator>>(Csdsin &, char &);

    unsigned short iseof(void)
    {
      switch (_df) {
      case DFFILE:
	return _csdsinf->eof();
      case DFCMD:
	return _csdsinb.empty();
      default:
	return _csdsins->eof();
      }
    }
    unsigned short isinit(void)
    {
      return _init;
    }
    unsigned short isfail(void)
    {
      if (_df == DFFILE)
	return _csdsinf->fail();
      return 0;
    }
    unsigned short isbad(void)
    {
      switch (_df) {
      case DFFILE:
	return _csdsinf->bad();
      case DFCMD:
	return _csdsinb.empty();
      }
      return 0;
    }

  private:
    std::ifstream *_csdsinf;  //  for S1
    std::istream *_csdsins;   //  for S2          
    std::string _csdsinb;     //  for S3
    dfrom _df;    
    unsigned char _init:1;
  };

  inline
  Csdsin &getGlobalCsdsinRef(void)
  {
    static Csdsin csdsin;
    return csdsin;
  }

}




#endif  //  end of head
