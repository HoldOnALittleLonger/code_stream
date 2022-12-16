#ifndef _CSDSIN_H_
#define _CSDSIN_H_

#include<cstddef>
#include<cstring>
#include<string>
#include<memory>
#include<fstream>


namespace csds {

//  DATA SOURCE
//    csds::dfrom::DFFILE  - data from file
//    csds::dfrom::DFSTDIN - data from stdin
//    csds::dfrom::DFCMD   - data from command line
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
      return genericISFunc(ISF_EOF);
    }
    bool isinit(void)
    {
      return _init;
    }
    bool isfail(void)
    {
      return genericISFunc(ISF_FAIL);
    }
    bool isbad(void)
    {
      return genericISFunc(ISF_BAD);
    }

  private:
    std::istream *_csdsin;  //  for S1 S2
    std::string _csdsinb;   //  for S3
    dfrom _df;    
    unsigned char _init:1;

    //  idenfier for which is-function should be call
    enum ISFUNCspecifier : unsigned char {
      ISF_EOF = 0,
	ISF_FAIL = 1,
	ISF_BAD = 2
    };

    //  genric is-func,use @i to distinguish which case it is
    bool genericISFunc(ISFUNCspecifier i);
  };

  inline
  Csdsin &getGlobalCsdsinRef(void)
  {
    static Csdsin csdsin;
    return csdsin;
  }

}

#define g_Csdsin csds::getGlobalCsdsinRef()

#endif  //  end of head
