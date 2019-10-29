#ifndef __UTIL__H__
#define __UTIL__H__

void ascii2hex(char const * const ascii, unsigned char* hex, unsigned int ascii_length);
bool uchar_array_equal(unsigned char const * const a, unsigned char const * const b, unsigned int size);

#endif
