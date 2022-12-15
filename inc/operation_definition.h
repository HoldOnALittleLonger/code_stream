#ifndef _OPERATION_DEFINITION_H_
#define _OPERATION_DEFINITION_H_

/* This file should contains the definitions of some procedures which
 * would be used in code_stream.
 */

/* Should define procedure as a structure of a class might be well */

#include<cstddef>
#include<cstring>
#include<concepts>
#include<string>
#include<exception>


namespace otm {

  /* port from a repository <cryptor.git> which owns to @Takanashi-Rikka-O */

  struct one_two_map {

    static constexpr char _R_TEXT[]{"XJwd!fGZib<l,)Opq|(tuvCayhSnERkm"};
    static constexpr char _Q_TEXT[]{"xbedPfvhSzW+-1m}?@#tQ&c(%Hsq;a<>"};

    const unsigned int _scale;
    unsigned char *_q_text;
    unsigned char *_r_text;
    unsigned int _text_length;
    unsigned int _division;
    unsigned short _resort;
    
    one_two_map() noexcept(false) : _scale(7)
    {
      size_t text_length(0);

      // copy keyword string.
      text_length = strlen(_Q_TEXT);
      _q_text = new unsigned char[text_length];
      if (!_q_text)
	throw std::bad_exception();
      memcpy(_q_text, _Q_TEXT, text_length);

      text_length = strlen(_R_TEXT);
      _r_text = new unsigned char[text_length];
      if (!_r_text)
	throw std::bad_exception();
      memcpy(_r_text, _R_TEXT, text_length);

      // set env
      _division = _text_length = text_length;
      _resort = 0;
    }

    one_two_map(const struct one_two_map &otm) = delete;
    one_two_map(struct one_two_map &&otm) = delete;
    one_two_map &operator=(const one_two_map &) = delete;
    one_two_map &operator=(one_two_map &&) = delete;

    ~one_two_map()
    {
      if (_q_text)
	delete[] _q_text, _q_text = nullptr;
      if (_r_text)
	delete[] _r_text, _r_text = nullptr;
    }

    void sortHashList(void);
    void setResortKey(unsigned short x);
    ssize_t otmEncode(const void *plaintext, size_t plaintext_length,
		      void *ciphertext, size_t ciphertext_size);
    ssize_t otmDecode(const void *ciphertext, size_t ciphertext_length,
		      void *plaintext, size_t plaintext_size);

    //  scaleElement - scale @x by multiple _scale
    //    @x : value to scale
    constexpr unsigned int scaleElement(unsigned char x) 
    {
      return (x * _scale);
    }
  
  };

  using otm_object = struct one_two_map;

}

namespace base64 {

  /* port from a repository <cryptor.git> which owns to @Takanashi-Rikka-O */

  struct base64_coding {

    static constexpr char BASE64_MAPPING[]{"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"};
    const char *_base64_mapping;

    base64_coding() : _base64_mapping(BASE64_MAPPING) { }

#undef BASE64_MAPPING

    /* parts */
    unsigned short getIndexForC(unsigned char c);

    //  nextAddress - return next pointer 
    //    @p : the pointer for current position
    //    @displacement: displacement for next address
    template<typename T>
    requires requires(T *arg, int x) {
      arg + x;
      x + arg;
    }
    T *nextAddress(T *p, short displacement)
    {
      return p + displacement;
    }

    unsigned char first_of(const unsigned char *segment);
    unsigned char second_of(const unsigned char *segment);
    unsigned char third_of(const unsigned char *segment);
    unsigned char fourth_of(const unsigned char *segment);
    
    ssize_t base64Encode(const void *plaintext, size_t plaintext_length,
			 void *ciphertext, size_t ciphertext_size);
    ssize_t base64Decode(const void *ciphertext, size_t plaintext_length,
			 void *plaintext, size_t plaintext_size);

    base64_coding(const base64_coding &) = delete;
    base64_coding &operator=(const base64_coding &) = delete;
    base64_coding(base64_coding &&) = delete;
    base64_coding &operator=(base64_coding &&) = delete;
  };

  using base64_object = struct base64_coding;

}


#endif
