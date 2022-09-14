

#include"codestream_abstract.h"

namespace codestream {

#define LOCAL_ULOCKER __luniquelocker
#define local_unique_locker(__locker) std::unique_lock<std::mutex> LOCAL_ULOCKER(__locker)
#define local_unique_unlock() LOCAL_ULOCKER.unlock()
#define local_unique_lock() LOCAL_ULOCKER.lock()

  Codestream::Codestream() : _ip_mutex_unique(_ip_mutex) {
    /* default constructor */

    local_unique_locker(_state_mutex);        // would unlock after exit function

    _codestream_flag.reset();
    _codestream_flag.set(OP_ORDER);
    _cp_end = 0;
    _ip = 0;
    _ip_start = 0;
    _ip_end = 0;
    _last_op_ret = nullptr;
    _state = INVAILD;

    /*    for (auto i : _code_procedures)
      i = nullptr;
    */

    _ip_mutex_unique.unlock();
  }

  Codestream::~Codestream() {
    _ip_mutex_unique.lock();
    _code_procedures.clear();
    _ip_mutex_unique.unlock();
  }

  // cant invoke get<Function> after called stopCode().
  // cant invoke get<Function> before restartCode() was called.

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
    opci i(0);
    _ip_mutex_unique.lock();
    i = _cp_end;
    _ip_mutex_unique.unlock();
    return i;
  }

  void Codestream::installProcedure(std::function<void *(void *)> &&f) {
    local_unique_locker(_state_mutex);
    local_unique_unlock();

    _ip_mutex_unique.lock();
    _code_procedures.emplace_back(f);

    // If nothing was installed into procedures, INIT flag should be false.
    if (!_codestream_flag[INIT])
      _codestream_flag.set(INIT);
    _ip_end = _cp_end = _code_procedures.size();        // update pos records.

    local_unique_lock();
    _state = NOERROR;
    _ip_mutex_unique.unlock();
  }

  void Codestream::installProcedure(std::function<void *(void *)> &&f, aindex pos) {
    local_unique_locker(_state_mutex);
    local_unique_unlock();
    _ip_mutex_unique.lock();
    auto vector_it(_code_procedures.begin());

    vector_it += pos;
    _code_procedures.emplace(vector_it, f);

    if (!_codestream_flag[INIT])
      _codestream_flag.set(INIT);
    _ip_end = _cp_end = _code_procedures.size();

    local_unique_lock();
    _state = NOERROR;
    _ip_mutex_unique.unlock();
  }

  codestream_process_state Codestream::uninstallProcedure(aindex which) {
    local_unique_locker(_state_mutex);
    _state = NOERROR;
    local_unique_unlock();
    
    auto it(_code_procedures.begin());

    _ip_mutex_unique.lock();
    if (_code_procedures.at(which) == nullptr) {
      local_unique_lock();
      _state = UNINSTALL_FAILED;
      local_unique_unlock();
    } else {
      it += which;
      _code_procedures.erase(it);
    }

    _ip_mutex_unique.unlock();
    return _state;
  }


  codestream_process_state Codestream::startCode(void *vec) {
    local_unique_locker(_state_mutex);
    std::function<void *(void *)> f(nullptr);

    if (!_codestream_flag[INIT]) {
      _state = NOINIT;
      return _state;
    }

    local_unique_unlock();

    // like a signal system

    do {
      _ip_mutex_unique.lock();
      local_unique_lock();
      _state = IN_PROGRESSING;
      local_unique_unlock();

      if ((_codestream_flag[OP_ORDER] && (_ip < _ip_end))
	  || (!_codestream_flag[OP_ORDER] && (_ip >= _ip_start))) {
	if (_code_procedures.at(_ip) != nullptr) {
	  f = _code_procedures.at(_ip);
	  _last_op_ret = f(_last_op_ret);
	}
       
	_ip = (_codestream_flag[OP_ORDER]) ? _ip + 1 : _ip - 1;        // set direction.

	_ip_mutex_unique.unlock();
      } else {
	local_unique_lock();
	_state = SHUTDOWN;
	local_unique_unlock();
	_ip_mutex_unique.unlock();
	break;
      }

    } while (1);

    local_unique_lock();
    return _state;
  }


  // dont call stop and restart span thread bounder

  codestream_process_state Codestream::stopCode(void) {
    local_unique_locker(_state_mutex);
    local_unique_unlock();

    if (!_codestream_flag[INIT]) {
      local_unique_lock();
      _state = NOINIT;
      return _state;
    }

    _ip_mutex_unique.lock();	// ip mutex
    local_unique_lock();
    if (_ip > _ip_end) {        // if _ip > _ip_end, there would no residue procedure have to exec.
      _state = SHUTDOWN;
      _ip_mutex_unique.unlock();
      return _state;
    }

    _codestream_flag.set(STOPPED);
    _state = SUSPEND;
    
    return _state;
  }

  codestream_process_state Codestream::restartCode(void) {
    local_unique_locker(_state_mutex);
    local_unique_unlock();

    if (!_codestream_flag[INIT]) {
      local_unique_lock();
      _state = NOINIT;
      return _state;
    }

    local_unique_lock();    
    if (_codestream_flag[STOPPED]) {
      _codestream_flag.reset(STOPPED);
      _state = IN_PROGRESSING;
      _ip_mutex_unique.unlock();
    }
    else
      _state = NOT_SUSPEND;

    return _state;
  }

  bool Codestream::is_processing(void) {
    local_unique_locker(_state_mutex);
    return _state == IN_PROGRESSING;
  }

  bool Codestream::is_suspend(void) {
    local_unique_locker(_state_mutex);
    return _state == SUSPEND;
  }

  bool Codestream::is_shutdown(void) {
    local_unique_locker(_state_mutex);
    return _state == SHUTDOWN;
  }

  bool Codestream::is_execsuccess(void) {
    local_unique_locker(_state_mutex);
    return _state == NOERROR;
  }

  std::string Codestream::processStateExplain(void) {
    std::string msg("0000");
    local_unique_locker(_state_mutex);
    switch (_state) {
    case INVAILD:
      msg.clear();
      msg = "codestream: program is unavaiable.";
      break;

    case NOERROR:
      msg.clear();
      msg = "codestream: executing no error happend.";
      break;

    case NOINIT:
      msg.clear();
      msg = "codestream: program had not initialized,may be none of any procedure was installed.";
      break;

    case IN_PROGRESSING:
      msg.clear();
      msg = "codestream: working...";
      break;

    case SHUTDOWN:
      msg.clear();
      msg = "codestream: working shutdown.";
      break;

    case UNINSTALL_FAILED:
      msg.clear();
      msg = "codestream: uninstall procedure was failed,may be such procedure was not installed.";
      break;

    case SUSPEND:
      msg.clear();
      msg = "codestream: suspended.";
      break;

    case NOT_SUSPEND:
      msg.clear();
      msg = "codestream: cant recover program from suspend while it had not be suspended.";
      break;

    default:
      msg.clear();
      msg = "codestream: no defined state.";
      break;

    }

    return msg;
  }

}
