

#include"codestream_abstract.h"

namespace codestream {

#define LOCAL_ULOCKER __luniquelocker
#define local_unique_locker(__locker) std::unique_lock<std::mutex> LOCAL_ULOCKER(__locker)
#define local_unique_unlock() LOCAL_ULOCKER.unlock()
#define local_unique_lock() LOCAL_ULOCKER.lock()

  Codestream::Codestream() {
    /* default constructor */
    
    _codestream_flag.reset();
    _codestream_flag.set(OP_ORDER);        // default is left to right.
    _cp_end = 0;
    _ip = 0;
    _ip_start = 0;
    _ip_end = 0;
    _last_op_ret = nullptr;
    _state = INVAILD;

    /*    for (auto i : _code_procedures)
      i = nullptr;
    */

  }

  Codestream::~Codestream() {
    lock_ip();
    _code_procedures.clear();
    unlock_ip();
  }

  // cant invoke get<Function> after called stopCode().
  // cant invoke get<Function> before restartCode() was called.

  opip Codestream::getProgress(void) {
    opip curr(0);
    lock_ip();
    curr = _ip;
    unlock_ip();
    return curr;
  }

  void *Codestream::getLastResult(void) {
    void *ret(nullptr);
    lock_ip();
    ret = _last_op_ret;
    unlock_ip();
    return ret;
  }

  opci Codestream::getOpChainSize(void) {
    opci i(0);
    lock_ip();
    i = _cp_end;
    unlock_ip();
    return i;
  }

  void Codestream::installProcedure(std::function<void *(void *)> &&f) {
    local_unique_locker(_state_mutex);
    local_unique_unlock();

    lock_ip();
    _code_procedures.emplace_back(f);

    // If nothing was installed into procedures, INIT flag should be false.
    if (!_codestream_flag[INIT])
      _codestream_flag.set(INIT);
    _ip_end = _cp_end = _code_procedures.size();        // update pos records.

    local_unique_lock();
    _state = NOERROR;
    unlock_ip();
  }

  void Codestream::installProcedure(std::function<void *(void *)> &&f, aindex pos) {
    local_unique_locker(_state_mutex);
    local_unique_unlock();
    lock_ip();
    auto vector_it(_code_procedures.begin());

    vector_it += pos;
    _code_procedures.emplace(vector_it, f);

    if (!_codestream_flag[INIT])
      _codestream_flag.set(INIT);
    _ip_end = _cp_end = _code_procedures.size();

    local_unique_lock();
    _state = NOERROR;
    unlock_ip();
  }

  codestream_process_state Codestream::uninstallProcedure(aindex which) {
    local_unique_locker(_state_mutex);
    _state = NOERROR;
    local_unique_unlock();
   
    lock_ip();
    if (which < _cp_end)
      if (_code_procedures.at(which) == nullptr) {
	local_unique_lock();
	_state = UNINSTALL_FAILED;
	local_unique_unlock();
      } else {
	auto f = _code_procedures[which];
	f = nullptr;
	_code_procedures[which] = f;	// use nullptr means the element is in useless state.
      }
    else {
      local_unique_lock();
      _state = UNINSTALL_EXCEED;	// want to uninstall element which is not in an effective scope.
      local_unique_unlock();
    }

    unlock_ip();
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
      lock_ip();
      local_unique_lock();
      _state = IN_PROGRESSING;
      local_unique_unlock();

      if ((_codestream_flag[OP_ORDER] && (_ip < _ip_end))
	  || (!_codestream_flag[OP_ORDER] && (_ip >= _ip_start))) {
	if (_code_procedures.at(_ip) != nullptr) {
	  f = _code_procedures.at(_ip);
	  unlock_ip();	// dont lock ip while process f
	  _last_op_ret = f(_last_op_ret);
	  lock_ip();
	}
       
	_ip = (_codestream_flag[OP_ORDER]) ? _ip + 1 : _ip - 1;        // set direction.
	unlock_ip();
      } else {
	local_unique_lock();
	_state = SHUTDOWN;
	local_unique_unlock();
	unlock_ip();
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

    lock_ip();	// ip mutex
    local_unique_lock();
    if (_ip >= _ip_end) {        // if _ip > _ip_end, there would no residue procedure have to exec.
      _state = SHUTDOWN;
      unlock_ip();
      return _state;
    }

    local_unique_unlock();
    if (this->is_processing()) {
      _codestream_flag.set(STOPPED);
      local_unique_lock();
      _state = SUSPEND;
      return _state;
    }

    local_unique_lock();
    _state = SUSPEND_FAILED;
    unlock_ip();
    return _state;
  }

  // should use stopCode() and restartCode() as a commands sequence
  // a1 a2 <stopCode()> a3 a4 <restartCode()> ...
  // and cant invoke get<Function> between them.

  codestream_process_state Codestream::restartCode(void) {
    local_unique_locker(_state_mutex);
    local_unique_unlock();

    if (!_codestream_flag[INIT]) {
      local_unique_lock();
      _state = NOINIT;
      return _state;
    }

    if (this->is_suspend()) {
      local_unique_lock();
      if (_codestream_flag[STOPPED]) {
	_codestream_flag.reset(STOPPED);
	_state = IN_PROGRESSING;
	unlock_ip();
      }
    }
    else {
      local_unique_lock();
      _state = NOT_SUSPEND;
    }

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

    case SUSPEND_FAILED:
      msg.clear();
      msg = "codestream: may be procedure is not in progressing.";
      break;

    case SHUTDOWN:
      msg.clear();
      msg = "codestream: working shutdown.";
      break;

    case UNINSTALL_FAILED:
      msg.clear();
      msg = "codestream: uninstall procedure was failed,may be such procedure was not installed.";
      break;

    case UNINSTALL_EXCEED:
      msg.clear();
      msg = "codestream: cant uninstall procedure which is not in op-chain.";

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
