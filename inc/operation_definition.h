#ifndef _OPERATION_DEFINITION_H_
#define _OPERATION_DEFINITION_H_

/* This file should contains the definitions of some procedures which
 * would be used in code_stream.
 */

/* Should define procedure as a structure of a class might be well */

#include<cstddef>
#include<cstring>
#include<concepts>


namespace otm {

  /* port from a repository <cryptor.git> which owns to @Takanashi-Rikka-O */

  struct one_two_map {

#define Q_TEXT "XJwd!fGZib<l,)Opq|(tuvCayhSnERkm"
#define R_TEXT "xbedPfvhSzW+-1m}?@#tQ&c(%Hsq;a<>"

    const char *_const_q_text;
    const char *_const_r_text;
    const unsigned int _scale;
    char *_q_text;
    char *_r_text;
    size_t _text_length;
    size_t _division;
    unsigned short _resort;
    unsigned short _if_inited;
    
    one_two_map() : _const_q_text(Q_TEXT), _const_r_text(R_TEXT) {
    }
    one_two_map(unsigned short sort_distance) : _const_q_text(Q_TEXT), _const_r_text(R_TEXT) {

    }

#undef Q_TEXT
#undef R_TEXT

    ~one_two_map() {
      if (_q_text)
	delete[] _q_text, _q_text = nullptr;
      if (_r_text)
	delete[] _r_text, _r_text = nullptr;
    }

    void initOTM(void);
    void sortHashList(void);
    void setResortKey(unsigned short x);
    ssize_t otmEncode(const char *plaintext, size_t plaintext_length,
		      char *ciphertext, size_t ciphertext_size);
    ssize_t otmDecode(const char *ciphertext, size_t ciphertext_length,
		      char *plaintext, size_t plaintext_size);

    template<typename T>
    requires (T &arg, unsigned short p) {
      arg * p;
      p * arg;
    }
    constexpr T scaleElement(T x) {
      return (x * _scale);
    }
  
  };

  using otm_object = struct one_two_map;

}

namespace base64 {

  /* port from a repository <cryptor.git> which owns to @Takanashi-Rikka-O */

  struct base64_coding {

#define BASE64_MAPPING "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"

    const char *_base64_mapping;

    base64_coding() : _base64_mapping(BASE64_MAPPING) { }

#undef BASE64_MAPPING

    /* parts */
    unsigned short getIndexForC(char c);
    char *nextAddress(char *p, short displacement);
    char first_of(char *segment);
    char second_of(char *segment);
    char third_of(char *segment);
    char fourth_of(char *segment);
    
    ssize_t base64Encode(const char *plaintext, size_t plaintext_length,
			 char *ciphertext, size_t ciphertext_size);
    ssize_t base64Decode(const char *ciphertext, size_t plaintext_length,
			 char *plaintext, size_t plaintext_size);
  };

  using base64_object = struct base64_coding;

}


#endif
