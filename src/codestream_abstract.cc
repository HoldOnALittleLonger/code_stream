

#include"codestream_abstract.h"

namespace codestream {

  Codestream::Codestream() : _code_procedures(VECTOR_SIZE), _ip_mutex_unique(_ip_mutex) {
    _init = 0;
    _procedure_order.reset();
    _cp_end = 0;
    _ip = 0;
    _ip_end = 0;
    _last_op_ret = nullptr;
    _last_install = 0;

    for (auto i : _code_procedures)
      i = nullptr;

    _ip_mutex_unique.unlock();
  }

  Codestream::~Codestream() {
    _ip_mutex_unique.lock();
    for (auto i : _code_procedures)
      i = nullptr;
    _ip_mutex_unique.unlock();
  }

  opip Codestream::getProgress(void) {
    opip curr(0);
    _ip_mutex_unique.lock();
    curr = _ip;
    _ip_mutex_unique.unlock();
    return curr;
  }

  void *Codestream::getLastResult(void) {
    void *ret(nullptr);
    _ip_mutex_unique.lock();
    ret = _last_op_ret;
    _ip_mutex_unique.unlock();
    return ret;
  }

  opci Codestream::getOpChainSize(void) {
    return _cp_end;
  }

  void Codestream::installProcedure(std::function<void *(void *)> &&f) {
    aindex install_number(_last_install + 1);
    _ip_mutex_unique.lock();
    _code_procedures[install_number] = f;
    if (!_codestream_flag[INIT])
      _codestream_flag.set(INIT);
    _last_install = install_number;
    _ip_end = _cp_end = install_number;
    _ip_mutex_unique.unlock();
  }

  short Codestream::uninstallProcedure(aindex which) {
    short ret(0);
    _ip_mutex_unique.lock();
    if (_code_procedures[which] == nullptr) {
      ret = -1;
    } else
      _code_procedures[which] = nullptr;

    _ip_mutex_unique.unlock();
    return ret;
  }


  int Codestream::startCode(void *vec) {
    if (!_codestream_flag[INIT])
      return -NOINIT;

    do {
      _ip_mutex_unique.lock();
      if (_ip <= _ip_end) {
	if (_code_procedures[_ip] != nullptr)
	  _last_op_ret = _code_procedures[_ip](_last_op_ret);
	++_ip;
	_ip_mutex_unique.unlock();
      } else {
	_ip_mutex_unique.unlock();
	break;
      }

    } while (1);

    return 0;
  }

  int Codestream::stopCode(void) {
    if (!_codestream_flag[INIT])
      return -NOINIT;

    _ip_mutex_unique.lock();
    if (_ip > _ip_end) {
      _ip_mutex_unique.unlock();
      return -FINISHED;
    }

    _codestream_flag.set(STOPPED);
    
    return 0;
  }

  int Codestream::restartCode(void) {
    if (!_codestream_flag[INIT])
      return -NOINIT;
    
    if (_codestream_flag[STOPPED])
      _ip_mutex_unique.unlock();
    else
      return -UNSTOPPED;

  }

  





}
