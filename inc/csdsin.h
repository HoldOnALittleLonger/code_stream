#ifndef _CSDSIN_H_
#define _CSDSIN_H_

#include<cstddef>
#include<cstring>
#include<string>
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
    std::size_t readCsdsin(char *dest, std::size_t n);

    bool iseof(void)
    {
      switch (_df) {
      case DFFILE:
	return dynamic_cast<std::ifstream *>(_csdsin)->eof();
      case DFSTDIN:
	return _csdsin->eof();
      case DFCMD:
	return _csdsinb.empty();
      default:
	return false;
      }
    }
    bool isinit(void)
    {
      return _init;
    }
    bool isfail(void)
    {
      switch (_df) {
      case DFFILE:
	return dynamic_cast<std::ifstream *>(_csdsin)->fail();
      case DFSTDIN:
	return _csdsin->fail();
      case DFCMD:;
      }
      return false;
    }
    bool isbad(void)
    {
      switch (_df) {
      case DFFILE:
	return dynamic_cast<std::ifstream *>(_csdsin)->bad();
      case DFSTDIN:
	return _csdsin->bad();
      case DFCMD:
	return _csdsinb.empty();
      }
      return false;
    }

  private:
    std::istream *_csdsin;  //  for S1 S2
    std::string _csdsinb;   //  for S3
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
