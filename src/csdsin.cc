#include"csdsin.h"
#include<iostream>


namespace csds {


  Csdsin::Csdsin() : _csdsins(&std::cin), _csdsinb("nil"), _init(0), _df(DFSTDIN)
  {
  }

  Csdsin::~Csdsin()
  {
    _init = 0;
  }

  Csdsin &operator>>(Csdsin &csdsin, char &c)
  {
    switch (csdsin._df) {
    case DFFILE:
      *(csdsin._csdsinf)>>c;
      break;

    case DFSTDIN:
      *(csdsin._csdsins)>>c;
      break;

    case DFCMD:
      if (csdsin._csdsinb.empty())
	break;
      c = csdsin._csdsinb.at(0);
      if (csdsin._csdsinb.length() - 1 > 0)
	csdsin._csdsinb = csdsin._csdsinb.substr(1, csdsin._csdsinb.length() - 1);
      else
	csdsin._csdsinb.clear();
      break;
    }
    
    return csdsin;
  }

  void Csdsin::initCsdsin(dfrom sK, const char *data_str = nullptr)
  {
    auto lf_fillString = [this, data_str](void) -> void
      {
	std::size_t sl(strlen(data_str));
	closeCsdsin();
	if (sl) 
	  for (decltype(sl) e(0); e < sl; ++e)
	    _csdsinb+=data_str[e];
      };

    //  just init once
    if (_init)
      return;

    switch (sK) {
    case DFFILE:
      _csdsinf = new std::ifstream;
      if (!_csdsinf) {
	_init = 0;
	break;
      }
      _csdsinf->open(data_str, std::ios_base::in);
      _init = _csdsinf->is_open() ? 1 : 0;
      break;

    case DFSTDIN:
      _csdsins = &std::cin;
      _init = 1;
      break;

    case DFCMD:
      lf_fillString();
      _init = (data_str) ? 1 : 0;
      break;

    default:  //  undefined situation
      _init = 0;
    }

    _df = sK;
  }

  void Csdsin::closeCsdsin(void)
  {
    switch (_df) {
    case DFFILE:
      _csdsinf->close();
      break;
    case DFSTDIN:
      break;
    case DFCMD:
      _csdsinb.clear();
    }
  }

  std::size_t Csdsin::read(char *dest, std::size_t n)
  {
    if (_df == DFCMD) {
      if (_csdsinb.empty())
	return 0;
      ssize_t count(0);
      do {
	(*this)>>dest[count++];
	--n;
      } while (!_csdsinb.empty() && n);
      return count;
    } else if (_df == DFFILE) {
      _csdsinf->read(dest, n);
      return _csdsinf->gcount();
    } else {
      _csdsins->read(dest, n);
      return _csdsins->gcount();
    }
  }

}
