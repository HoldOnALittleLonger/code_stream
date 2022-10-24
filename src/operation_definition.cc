#include"operation_definition.h"

namespace otm {

  void one_two_map::sortHashList(void) {
    unsigned short int index(0);
    char i = '\0',j = '\0';

    // This means dont need to sort them.
    if (!_resort)
      return;

    // p2c
    for (index = 0; index < _text_length; ++index) {
      if (index + _resort < _text_length) {
	i = _q_text[index];
	j = _q_text[index + _resort];

	if (j != '\0') {
	  _q_text[index] = j;
	  _q_text[index + _resort] = i;
	}
      }
      else
	break;
    }
    // k2c
    for (index = 0; index < _text_length; ++index) {
      if (index + _resort < _text_length) {
	i = _r_text[index];
	j = _r_text[index + _resort];

	if (j != '\0') {
	  _r_text[index] = _r_text[index + _resort];
	  _r_text[index + _resort] = i;
	}
      }
      else
	break;
    }

  }

  void one_two_map::setResortKey(unsigned short x) {
    decltype(x) i = x * 61;
    _resort = i - (i / _text_length) * _text_length;
    this->sortHashList();
  }

  ssize_t one_two_map::otmEncode(const char *plaintext, size_t plaintext_length,
				 char *ciphertext, size_t ciphertext_size) {
    long int after_scale(0);
    unsigned quotient_value(0), remainder_value(0);
    ssize_t coded(0);

    if (!plaintext || !ciphertext)    // no data was found
      return -1;

    if (ciphertext_size < (plaintext_length * 2))    // would exceed space.
      return -1;

    for (unsigned short plaintext_index(0), q(0), r(1); plaintext_index < plaintext_length; ++plaintext_index) {
      after_scale = this->scaleElement(static_cast<long int>(plaintext[plaintext_index]));
      quotient_value = after_scale / _division;
      remainder_value = after_scale - (after_scale / _division) * _division;
      ciphertext[q] = _q_text[quotient_value];
      ciphertext[r] = _r_text[remainder_value];
      coded = r + 1;
      q += 2;
      r += 2;
    }

    return coded;
  }

  ssize_t one_two_map::otmDecode(const char *ciphertext, size_t ciphertext_length,
				 char *plaintext, size_t plaintext_size)
  {
    unsigned short quotient_value(0), remainder_value(0), plaintext_index(0);
    ssize_t decoded(0);

    auto position = [](char c, const char *list) -> unsigned short {
      unsigned short pos(0);
      for (; *list != '\0'; ++list) {
	if (*list == c)
	  break;
	++pos;
      }
      return pos;
    };
    
    if (!ciphertext || !plaintext)    // no data was found
      return -1;

    if (plaintext_size < (ciphertext_length / 2))    // would exceed space.
      return -1;
    
    for (unsigned short q(0), r(1); r < ciphertext_length; q += 2, r += 2) {
      // quotient
      quotient_value = position(ciphertext[q], this->_q_text);
      // remainder
      remainder_value = position(ciphertext[r], this->_r_text);
      plaintext[plaintext_index++] = (quotient_value * _division + remainder_value) / _scale;
    }

    decoded = plaintext_index;
    return decoded;
  }

}


namespace base64 {

  unsigned short base64_coding::getIndexForC(char c) {
    unsigned short i(0);
    for (const char *iterator(_base64_mapping); *iterator != '\0'; ++iterator) {
      if (*iterator == c)
	break;
      ++i;
    }
    return i;
  }
  
  inline
  char base64_coding::first_of(const char *segment) {
    return *segment;
  }

  inline
  char base64_coding::second_of(const char *segment) {
    return *(segment + 1);
  }

  inline
  char base64_coding::third_of(const char *segment) {
    return *(segment + 2);
  }

  inline 
  char base64_coding::fourth_of(const char *segment) {
    return *(segment + 3);
  }

  ssize_t base64_coding::base64Encode(const char *plaintext, size_t plaintext_length,
				      char *ciphertext, size_t ciphertext_size)
  {
    unsigned char first('\0'), second('\0'), third('\0');
    ssize_t ciphertext_index(0);
    unsigned short quotient_value(0), remainder_value(0);
    char *temp_buffer(nullptr);
    const char *entry(nullptr);

    if (!plaintext || !ciphertext)
      return -1;

    temp_buffer = new char[plaintext_length + 16];
    if (!temp_buffer)
      return -1;
    else {
      // size checking for place where save output
      if (ciphertext_size < ((plaintext_length + 16) * 4)) {
	delete[] temp_buffer;
	temp_buffer = nullptr;
	return -1;
      }
      else
	strncpy(temp_buffer, plaintext, plaintext_length);
    }

    quotient_value = plaintext_length / 3;
    remainder_value = plaintext_length - (plaintext_length / 3) * 3;

    // replenish zero at end.
    if (remainder_value) {
      for (unsigned short count(0); count < 3 - remainder_value; ++count)
	temp_buffer[plaintext_length++] = '\0';
      quotient_value = plaintext_length / 3;
    }
    
    entry = temp_buffer;
    ciphertext_index = 0;
    // main algorithm
    for (unsigned short iterator(0), map_index(0); iterator < quotient_value; ++iterator) {
      first = first_of(entry);
      second = second_of(entry);
      third = third_of(entry);

      map_index = (unsigned char)(first >> 2);
      ciphertext[ciphertext_index++] = _base64_mapping[map_index];

      map_index = (unsigned char)((unsigned char)(first << 6) >> 2) | (unsigned char)(second >> 4);
      ciphertext[ciphertext_index++] = _base64_mapping[map_index];

      map_index = (unsigned char)((unsigned char)(second << 4) >> 2) | (unsigned char)(third >> 6);
      ciphertext[ciphertext_index++] = _base64_mapping[map_index];

      map_index = (unsigned char)(third << 2) >> 2;
      ciphertext[ciphertext_index++] = _base64_mapping[map_index];      

      entry = nextAddress(entry, 3);
    }
    // hint that how many zero were appended.
    for (unsigned short count(0), index(ciphertext_index - 1);
	 !remainder_value && count < 3 - remainder_value;
	 ++count)
      ciphertext[index--] = '=';
    
    delete[] temp_buffer;
    temp_buffer = nullptr;
    return ciphertext_index;
  }

  ssize_t base64_coding::base64Decode(const char *ciphertext, size_t ciphertext_length,
				      char *plaintext, size_t plaintext_size)
  {
    const char *entry(nullptr);
    char *temp_buffer(nullptr);
    unsigned char first('\0'), second('\0'), third('\0'), fourth('\0');
    unsigned short map_index1(0), map_index2(0), map_index3(0), map_index4(0);
    unsigned short quotient_value(0), remainder_value(0);
    ssize_t plaintext_index(0);
    
    if (!ciphertext || !plaintext)
      return -1;

    quotient_value = ciphertext_length / 4;
    remainder_value = ciphertext_length - (ciphertext_length / 4) * 4;

    if (plaintext_size < (quotient_value * 3))
      return -1;

    if (!remainder_value)    // ciphertext may be not a properly base64 data string
      return -1;

    temp_buffer = new char[ciphertext_length];
    if (!temp_buffer)
      return -1;
    else
      strncpy(temp_buffer, ciphertext, ciphertext_length);

    // delete =
    for (unsigned short index(ciphertext_length - 1);
	 temp_buffer[index] == '=' && index > 0; --index)
      temp_buffer[index] = *_base64_mapping;

    entry = temp_buffer;
    plaintext_index = 0;
    // main algorithm
    for (unsigned short count(0); count < quotient_value; ++count) {
      first = first_of(entry);
      second = second_of(entry);
      third = third_of(entry);
      fourth = fourth_of(entry);

      map_index1 = getIndexForC(first);
      map_index2 = getIndexForC(second);
      map_index3 = getIndexForC(third);
      map_index4 = getIndexForC(fourth);      

      if (map_index1 >= 64 || map_index2 >= 64
	  ||
	  map_index3 >= 64 || map_index4 >= 64) {
	delete[] temp_buffer;
	temp_buffer = nullptr;
	return -1;
      }

      plaintext[plaintext_index++] = (unsigned char)(map_index1 << 2) | (unsigned char)(map_index2 >> 4);
      plaintext[plaintext_index++] = (unsigned char)(map_index2 << 4) | (unsigned char)(map_index3 >> 2);
      plaintext[plaintext_index++] = (unsigned char)(map_index3 << 6) | map_index4;

      entry = nextAddress(entry, 4);
    }

    delete[] temp_buffer;
    temp_buffer = nullptr;
    return plaintext_index;
  }


}
