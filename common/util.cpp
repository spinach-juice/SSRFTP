#include "util.h"

void ascii2hex(char const * const ascii, unsigned char* hex, unsigned int ascii_length)
{
	if(hex == nullptr || ascii == nullptr)
		return;

	unsigned int i = 0;
	for(; i < ascii_length; i+=2)
	{
		unsigned char hexdigit = 0x00;
		if(ascii[i] >= '0' && ascii[i] <= '9')
			hexdigit |= (unsigned char)(ascii[i] - '0') << 4;
		else if(ascii[i] >= 'A' && ascii[i] <= 'F')
			hexdigit |= (unsigned char)(ascii[i] - 'A' + 0x0a) << 4;
		else if(ascii[i] >= 'a' && ascii[i] <= 'f')
			hexdigit |= (unsigned char)(ascii[i] - 'a' + 0x0a) << 4;

		if(ascii[i+1] >= '0' && ascii[i+1] <= '9')
			hexdigit |= (unsigned char)(ascii[i+1] - '0');
		else if(ascii[i+1] >= 'A' && ascii[i+1] <= 'F')
			hexdigit |= (unsigned char)(ascii[i+1] - 'A' + 0x0a);
		else if(ascii[i+1] >= 'a' && ascii[i+1] <= 'f')
			hexdigit |= (unsigned char)(ascii[i+1] - 'a' + 0x0a);

		hex[i / 2] = hexdigit;
	}
}

bool uchar_array_equal(unsigned char const * const a, unsigned char const * const b, unsigned int size)
{
	unsigned int i = 0;
	for(; i < size; i++)
		if(a[i] != b[i])
			return false;
	return true;
}
