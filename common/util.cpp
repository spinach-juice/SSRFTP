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

void ulong_array_sort(unsigned long * array, unsigned long const length)
{
	unsigned long min = ulong_array_min(array, length);
	unsigned long max = ulong_array_max(array, length);

	unsigned long span = max - min + 1;

	unsigned long * bucket = new unsigned long[span];

	unsigned long i = 0;
	for(; i < span; i++)
		bucket[i] = 0;

	for(i = 0; i < length; i++)
		bucket[array[i] - min]++;

	unsigned long j = 0;
	for(i = 0; j < span; j++)
	{
		while(bucket[j] > 0)
		{
			array[i] = j + min;
			i++;
			bucket[j]--;
		}
	}

	delete bucket;
}

unsigned long ulong_array_max(unsigned long const * const array, unsigned long const length)
{
	unsigned long i = 0;
	unsigned long max = 0;
	for(; i < length; i++)
		if(array[i] > max)
			max = array[i];

	return max;
}

unsigned long ulong_array_min(unsigned long const * const array, unsigned long const length)
{
	unsigned long i = 0;
	unsigned long min = 0xffffffffffffffff;
	for(; i < length; i++)
		if(array[i] < min)
			min = array[i];

	return min;
}

std::vector<unsigned long> ulong_array_singles(unsigned long const * const array, unsigned long const length)
{
	std::vector<unsigned long> singles;
	bool consec_with_last = false;
	bool eq_to_last = false;
	unsigned long i = 0;
	for(; i < length; i++)
	{
		if(!consec_with_last && !eq_to_last && (i == length - 1 || array[i + 1] != array[i] + 1))
			singles.push_back(array[i]);

		if(i != length - 1)
		{
			consec_with_last = array[i + 1] == array[i] + 1;
			eq_to_last = array[i + 1] == array[i];
		}
	}

	return singles;
}

std::vector<unsigned long> ulong_array_ranges(unsigned long const * const array, unsigned long const length)
{
	std::vector<unsigned long> ranges;
	bool consec_with_last = false;
	unsigned long i = 0;
	for(; i < length; i++)
	{
		if(consec_with_last && i == length - 1)
			ranges.push_back(array[i]);
		else if(consec_with_last && array[i + 1] != array[i] + 1)
		{
			consec_with_last = false;
			ranges.push_back(array[i]);
		}
		else if(!consec_with_last && i != length - 1 && array[i + 1] == array[i] + 1)
		{
			consec_with_last = true;
			ranges.push_back(array[i]);
		}
	}
	return ranges;
}
