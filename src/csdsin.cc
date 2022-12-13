#include<iostream>

#include"csdsin.h"

namespace csds {


  Csdsin::Csdsin() : _csdsin(nullptr), _csdsinb("nil"), _df(DFSTDIN), _init(0)
  {
    _csdsinb.clear();
  }

  Csdsin::~Csdsin()
  {
    _init &= 0;
    _df = DFSTDIN;
    if (_df == DFFILE)
      delete _csdsin;
    _csdsin = nullptr;
  }

  //  initCsdsin - initialize Csdsin.
  //    @sK       : indicates which situation.
  //    @data_str : it is effective only sK = S1 or sK = S3
  void Csdsin::initCsdsin(dfrom sK, const char *data_str = nullptr)
  {
    auto lf_fillString = [this, data_str](void) -> void
      {
	if (data_str) {
	  closeCsdsin();
	  _csdsinb = data_str;
	}
      };
    _df = sK;

    //  just init once
    if (_init)
      return;

    switch (_df) {
    case DFFILE:
      {
	std::ifstream *readFile = new std::ifstream;
	if (!readFile) {
	  _init &= 0;
	  break;
	}
	readFile->open(data_str, std::ios_base::in);
	_init = readFile->is_open() ? 1 : 0;
	_csdsin = dynamic_cast<std::istream *>(readFile);
      }
      break;

    case DFSTDIN:
      _csdsin = &std::cin;
      _init ^= 1;
      break;

    case DFCMD:
      lf_fillString();
      _init = (data_str) ? 1 : 0;
      break;

    default:  //  undefined situation
      _init &= 0;
    }

  }

  //  closeCsdsin - close stream.
  //    #  after closed,the stream will be ineffeciency.
  void Csdsin::closeCsdsin(void)
  {
    if (!_init)  //  refuse close uninit object
      return;

    switch (_df) {
    case DFFILE:
      dynamic_cast<std::ifstream *>(_csdsin)->close();
      break;
    case DFSTDIN:
      _csdsin->setstate(std::ios_base::eofbit);
      break;
    case DFCMD:
      _csdsinb.clear();
    }

    _df = DFSTDIN;
    _init &= 0;
  }

  //  readCsdsin - read stream.
  //    @dest : destination for data to write.
  //    @n    : size to read.
  //    return - length of record which had been read.
  std::size_t Csdsin::readCsdsin(char *dest, std::size_t n)
  {
    if (!_init)  //  shouldnt read uninit stream
      return 0;

    if (_df == DFCMD) {
      std::size_t count(0);
      if (_csdsinb.empty())  //  do nothing if there is null
	return count;

      count = _csdsinb.copy(dest, n, 0);
      if (!count) {
	_csdsinb.clear();
      } else
	_csdsinb = _csdsinb.substr(count, _csdsinb.length() - count);
      //  delete the parts which just readed
      
      return count;
    } else if (_df == DFFILE) {
      dynamic_cast<std::ifstream *>(_csdsin)->read(dest, n);
      return dynamic_cast<std::ifstream *>(_csdsin)->gcount();
    } else {
      _csdsin->read(dest, n);
      return _csdsin->gcount();
    }
  }

}
