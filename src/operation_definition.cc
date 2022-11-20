#include"operation_definition.h"

namespace otm {

  //  sortHashList - sort order for hash listes
  void one_two_map::sortHashList(void)
  {
    unsigned short int index(0);
    unsigned char i = '\0',j = '\0';

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

  //  setResortKey - set resort key.
  //    # method would automaticaly resort order for hash listes.
  void one_two_map::setResortKey(unsigned short x)
  {
    unsigned short i = x * 61;
    _resort = i - (i / _text_length) * _text_length;
    this->sortHashList();
  }

  //  otmEncode - main encode method of otm.
  //    @plaintext : plaintext.
  //    @plaintext_length : how long for @plaintext.
  //    @ciphertext : ciphertext buffer.
  //    @ciphertext_size : size of memory space for @ciphertext.
  //
  //    return - number of characters were coded.

  ssize_t one_two_map::otmEncode(const void *plaintext, size_t plaintext_length,
				 void *ciphertext, size_t ciphertext_size)
  {
    unsigned int after_scale(0);
    unsigned int quotient_value(0), remainder_value(0);
    ssize_t coded(0);
    const unsigned char *p(static_cast<const unsigned char *>(plaintext));
    unsigned char *c(static_cast<unsigned char *>(ciphertext));

    if (!p || !c)    // no data was found
      return -1;

    if (ciphertext_size < (plaintext_length * 2))    // would exceed space.
      return -1;

    for (unsigned short plaintext_index(0), q(0), r(1); 
	 plaintext_index < plaintext_length;
	 ++plaintext_index) {
      after_scale = this->scaleElement(p[plaintext_index]);
      quotient_value = after_scale / _division;
      remainder_value = after_scale - (after_scale / _division) * _division;
      c[q] = _q_text[quotient_value];
      c[r] = _r_text[remainder_value];
      coded = r + 1;
      q += 2;
      r += 2;
    }
    return coded;
  }

  //  otmDecode - main decode method of otm.
  //    @ciphertext : ciphertext.
  //    @ciphertext_length : how long for @ciphertext.
  //    @plaintext: plaintext buffer.
  //    @plaintext_size : size of memory space for @plaintext.
  //
  //    return - number of characters were decoded.
  ssize_t one_two_map::otmDecode(const void *ciphertext, size_t ciphertext_length,
				 void *plaintext, size_t plaintext_size)
  {
    unsigned int quotient_value(0), remainder_value(0), plaintext_index(0);
    const unsigned char *c(static_cast<const unsigned char *>(ciphertext));
    unsigned char *p(static_cast<unsigned char *>(plaintext));
    ssize_t decoded(0);

    auto position = [](unsigned char c, const unsigned char *list) -> unsigned int {
      unsigned int pos(0);
      for (; *list != '\0'; ++list) {
	if (*list == c)
	  break;
	++pos;
      }
      return pos;
    };
    
    if (!c || !p)    // no data was found
      return -1;

    if (plaintext_size < (ciphertext_length / 2))    // would exceed space.
      return -1;

    for (unsigned short q(0), r(1); r < ciphertext_length; q += 2, r += 2) {
      // quotient
      quotient_value = position(c[q], this->_q_text);
      // remainder
      remainder_value = position(c[r], this->_r_text);
      p[plaintext_index++] = (quotient_value * _division + remainder_value) / _scale;
    }

    decoded = plaintext_index;
    return decoded;
  }

}


namespace base64 {

  //  getIndexForC - find position of @c in base64 string.
  //    @c : the character.
  //
  //    return - index of c.
  unsigned short base64_coding::getIndexForC(unsigned char c)
  {
    unsigned short i(0);
    for (auto iterator(_base64_mapping); *iterator != '\0'; ++iterator) {
      if (*iterator == c)
	break;
      ++i;
    }
    return i;
  }
  
  //  first_of - first element in segment.
  //    @segment : a pointer point to segement.
  //
  //    return - the first element.
  inline
  unsigned char base64_coding::first_of(const unsigned char *segment)
  {
    return *segment;
  }

  //  second_of - second element in segment.
  //    @segment : a pointer point to segment.
  //
  //    return - the second element.
  inline
  unsigned char base64_coding::second_of(const unsigned char *segment) {
    return *(segment + 1);
  }

  //  third_of - third element in segment.
  //    @segment : a pointer point to segment.
  //
  //    return - the third element.
  inline
  unsigned char base64_coding::third_of(const unsigned char *segment)
  {
    return *(segment + 2);
  }

  //  fourth_of - fourth element in segment.
  //    @segment : a pointer point to segment.
  //
  //    return - the fourth element.
  inline 
  unsigned char base64_coding::fourth_of(const unsigned char *segment)
  {
    return *(segment + 3);
  }

  //  base64Encode - main method do base64 coding.
  //    @plaintext : plaintext.
  //    @plaintext_length : how long for @plaintext.
  //    @ciphertext : ciphertext buffer.
  //    @ciphertext_size : size of memory space for @ciphertext.
  //
  //    return - number of characters were encoded.
  ssize_t base64_coding::base64Encode(const void *plaintext, size_t plaintext_length,
				      void *ciphertext, size_t ciphertext_size)
  {
    unsigned char first('\0'), second('\0'), third('\0');
    ssize_t ciphertext_index(0);
    unsigned int quotient_value(0), remainder_value(0);
    unsigned char *temp_buffer(nullptr);
    const unsigned char *entry(nullptr), *p(static_cast<const unsigned char *>(plaintext));
    unsigned char *c(static_cast<unsigned char *>(ciphertext));

    if (!p || !c)
      return -1;

    temp_buffer = new unsigned char[plaintext_length];
    if (!temp_buffer)
      return -1;
    else {
      // size checking for place where save output
      if (ciphertext_size < (plaintext_length * 4 / 3)) {
	delete[] temp_buffer;
	return -1;
      }
      else
	memcpy(temp_buffer, p, plaintext_length);
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
      c[ciphertext_index++] = _base64_mapping[map_index];

      map_index = (unsigned char)((unsigned char)(first << 6) >> 2) | (unsigned char)(second >> 4);
      c[ciphertext_index++] = _base64_mapping[map_index];

      map_index = (unsigned char)((unsigned char)(second << 4) >> 2) | (unsigned char)(third >> 6);
      c[ciphertext_index++] = _base64_mapping[map_index];

      map_index = (unsigned char)(third << 2) >> 2;
      c[ciphertext_index++] = _base64_mapping[map_index];

      entry = nextAddress(entry, 3);
    }
    // prompt that how many zero were appended.
    for (unsigned short count(0), index(ciphertext_index - 1);
	 remainder_value && count < 3 - remainder_value;
	 ++count)
      c[index--] = '=';
    
    delete[] temp_buffer;
    return ciphertext_index;
  }

  //  base64Decode - main method do base64 decoding.
  //    @ciphertext : ciphertext buffer.
  //    @ciphertext_length : how long for @ciphertext.
  //    @plaintext : plaintext.
  //    @plaintext_size : size of memory space for @plaintext.
  //
  //    return - number of characters were decoded.
  ssize_t base64_coding::base64Decode(const void *ciphertext, size_t ciphertext_length,
				      void *plaintext, size_t plaintext_size)
  {
    const unsigned char *entry(nullptr);
    unsigned char *temp_buffer(nullptr);  //  need a changiable pointer.
    unsigned char first('\0'), second('\0'), third('\0'), fourth('\0');
    unsigned short map_index1(0), map_index2(0), map_index3(0), map_index4(0);
    unsigned short quotient_value(0), remainder_value(0);
    ssize_t plaintext_index(0);
    unsigned char *p(static_cast<unsigned char *>(plaintext));

    unsigned char count_for_addition(0);  //  dont out put additions.
    
    if (!ciphertext || !p)
      return -1;

    quotient_value = ciphertext_length / 4;
    remainder_value = ciphertext_length - (ciphertext_length / 4) * 4;

    if (plaintext_size < (quotient_value * 3))
      return -1;

    if (remainder_value)    // ciphertext may be not a properly base64 data string
      return -1;

    temp_buffer = new unsigned char[ciphertext_length];
    if (!temp_buffer)
      return -1;
    else
      memcpy(temp_buffer, ciphertext, ciphertext_length);

    // delete =
    for (auto index(ciphertext_length - 1);
	 temp_buffer[index] == '=' && index > 0; --index) {
      temp_buffer[index] = *_base64_mapping;
      ++count_for_addition;
    }

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
	return -1;
      }

      p[plaintext_index++] = (unsigned char)(map_index1 << 2) | (unsigned char)(map_index2 >> 4);
      p[plaintext_index++] = (unsigned char)(map_index2 << 4) | (unsigned char)(map_index3 >> 2);
      p[plaintext_index++] = (unsigned char)(map_index3 << 6) | map_index4;

      entry = nextAddress(entry, 4);
    }

    delete[] temp_buffer;
    plaintext_index -= count_for_addition;
    return plaintext_index;
  }


}
