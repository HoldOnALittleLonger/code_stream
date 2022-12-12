#ifndef _CODING_CLASS_H_
#define _CODING_CLASS_H_

#include<cstddef>
#include<cstdbool>
#include<cstring>
#include<memory>
#include<iterator>
#include<assert>
#include<concepts>

namespace coding {

  template<class T, size_t TN>
  requires std::unsigned_integral<T> || std::floating<T> || std::same_as<unsigned char>
  class basic_coding {
  public:
    basic_coding();
    virtual ~basic_coding() { };
    basic_coding(T &t);
    basic_coding(T &&t) noexcept;
    basic_coding &operator=(T &&t) noexcept;

    virtual bool encoding(const T *bInput, size_t nInput)override =0;
    virtual bool decoding(const T *bInput, size_t nInput)override =0;
    bool getOutput(T *bOutput, size_t nOutput);
    bool fillInput(const T *bInput, size_t nInput);

  protected:
    std::unique_ptr<T> _input;
    std::unique_ptr<T> _output;
    
    size_t _buff_size;
    size_t _input_length;
    size_t _output_length;

    basic_coding::iterator _input_begein() { return _input.get(); }
    basic_coding::iterator _output_begin() { return _output.get(); }

    typedef std::forward_iterator<T> iterator;
  };

  template<class T, size_t TN>
  basic_coding::basic_coding() : _input(std::make_unique<T>(TN)), _output(std::make_unique<T>(TN))
  {
    assert(!_input);
    assert(!_output);

    _buff_size = TN;
    _input_length = 0;
    _output_length = 0;
  }

  template<class T, size_t TN>
  basic_coding::basic_coding(T &t)
  {
    size_t toalloc(TN < t._buff_size ? t._buff_size : TN);
    std::unique_ptr<T> b1(std::make_unique<T>(toalloc));
    std::unique_ptr<T> b2(std::make_unique<T>(toalloc));

    assert(!b1);
    assert(!b2);

    memcpy(b1.get(), t._input.get(), toalloc);
    memcpy(b2.get(), t._output.get(), toalloc);

    _input = std::move(b1);
    _output = std::move(b2);
    _buff_size = toalloc;
  }

  template<class T, size_t TN>
  basic_coding::basic_coding(T &&t) noexcept
  {
    _input = std::move(t._input);
    _output = std::move(t._output);
    _buff_size = t._buff_size;
    _input_length = t._input_length;
    _output_length = t._output_length;
  }

  template<class T, size_t TN>
  basic_coding &basic_coding::operator=(T &&t) noexcept
  {
    this->basic_coding(t);
    return *this;
  }

  template<class T, size_t TN>
  bool basic_coding::getOutput(T *bOutput, size_t nOutput)
  {
    if (nOutput * sizeof(T) < _output_length * sizeof(T))
      return false;
    
    memcpy(bOutput, this->_output_begin(), nOutput);
    return true;
  }

  template<class T, size_t TN>
  bool basic_coding::fillInput(const T *bInput, size_t nInput)
  {
    if (nInput * sizeof(T) > _buff_size * sizeof(T))
      return false;

    memcpy(this->_input_begin(), bInput, nInput);
    return true;
  }

  class otm : public virtual basic_coding<unsigned char, BUFF_SIZE> {

  };

  class base64 : public virtual basic_coding<unsigned char, BUFF_SIZE + 4> {

  };

  class coding_class : public otm, public base64 {

  };

}


#endif
