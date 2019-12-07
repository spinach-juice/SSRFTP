#ifndef __UTIL__H__
#define __UTIL__H__

#include <vector>
#include <fstream>

unsigned long long getFileSize(std::ifstream& file);
void getFileContents(std::ifstream& file,unsigned long long fileSize,char* buffer);
void MD5(char const * const file_path, char* file_checksum);
void ascii2hex(char const * const ascii, unsigned char* hex, unsigned int ascii_length);
void hex2ascii(unsigned char const * const hex, char* ascii, unsigned int hex_length);
bool uchar_array_equal(unsigned char const * const a, unsigned char const * const b, unsigned int size);
bool schar_array_equal(char const * const a, char const * const b, unsigned int size);
bool ulong_array_equal(unsigned long const * const a, unsigned long const * const b, unsigned int size);
void ulong_array_sort(unsigned long * array, unsigned long const length);
unsigned long ulong_array_max(unsigned long const * const array, unsigned long const length);
unsigned long ulong_array_min(unsigned long const * const array, unsigned long const length);
std::vector<unsigned long> ulong_array_singles(unsigned long const * const array, unsigned long const length);
std::vector<unsigned long> ulong_array_ranges(unsigned long const * const array, unsigned long const length);

#endif
