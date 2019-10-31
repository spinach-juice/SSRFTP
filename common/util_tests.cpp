#include "catch.hpp"
#include "util.h"
/*
void ulong_array_sort(unsigned long * array, unsigned long const length);
unsigned long ulong_array_max(unsigned long const * const array, unsigned long const length);
unsigned long ulong_array_min(unsigned long const * const array, unsigned long const length);
std::vector<unsigned long> ulong_array_singles(unsigned long const * const array, unsigned long const length);
std::vector<unsigned long> ulong_array_ranges(unsigned long const * const array, unsigned long const length);
*/

TEST_CASE("Utility Functions: uchar_array_equal")
{
	unsigned char array_1[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
	unsigned char array_2[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
	unsigned char array_3[] = {'0', '2', '3', '4', '5', '6', '7', '8', '9'};
	unsigned char array_4[] = {'1', '2', '3', '4', '5', '6', '7', '8', '0'};

	CHECK(uchar_array_equal(array_1, array_1, 9));
	CHECK(uchar_array_equal(array_1, array_2, 9));
	CHECK(uchar_array_equal(array_2, array_1, 9));
	CHECK(!uchar_array_equal(array_1, array_3, 9));
	CHECK(!uchar_array_equal(array_3, array_1, 9));
	CHECK(uchar_array_equal(array_1, array_4, 8));
	CHECK(!uchar_array_equal(array_1, array_4, 9));
}

TEST_CASE("Utility Functions: schar_array_equal")
{
	char array_1[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
	char array_2[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
	char array_3[] = {'0', '2', '3', '4', '5', '6', '7', '8', '9'};
	char array_4[] = {'1', '2', '3', '4', '5', '6', '7', '8', '0'};

	CHECK(schar_array_equal(array_1, array_1, 9));
	CHECK(schar_array_equal(array_1, array_2, 9));
	CHECK(schar_array_equal(array_2, array_1, 9));
	CHECK(!schar_array_equal(array_1, array_3, 9));
	CHECK(!schar_array_equal(array_3, array_1, 9));
	CHECK(schar_array_equal(array_1, array_4, 8));
	CHECK(!schar_array_equal(array_1, array_4, 9));
}

TEST_CASE("Utility Functions: ulong_array_equal")
{
	unsigned long array_1[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
	unsigned long array_2[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
	unsigned long array_3[] = {'0', '2', '3', '4', '5', '6', '7', '8', '9'};
	unsigned long array_4[] = {'1', '2', '3', '4', '5', '6', '7', '8', '0'};

	CHECK(ulong_array_equal(array_1, array_1, 9));
	CHECK(ulong_array_equal(array_1, array_2, 9));
	CHECK(ulong_array_equal(array_2, array_1, 9));
	CHECK(!ulong_array_equal(array_1, array_3, 9));
	CHECK(!ulong_array_equal(array_3, array_1, 9));
	CHECK(ulong_array_equal(array_1, array_4, 8));
	CHECK(!ulong_array_equal(array_1, array_4, 9));
}

TEST_CASE("Utility Functions: ascii2hex")
{
	char ascii[] = "af4389761cc798aa";
	unsigned char hex[8];

	ascii2hex(ascii, hex, 16);

	unsigned char valid_output[] = {0xaf, 0x43, 0x89, 0x76, 0x1c, 0xc7, 0x98, 0xaa};

	CHECK(uchar_array_equal(hex, valid_output, 8));
}

TEST_CASE("Utility Functions: hex2ascii")
{
	unsigned char hex[] = {0xaf, 0x43, 0x89, 0x76, 0x1c, 0xc7, 0x98, 0xaa};
	char ascii[16];

	hex2ascii(hex, ascii, 8);

	char valid_output[] = "af4389761cc798aa";

	CHECK(schar_array_equal(ascii, valid_output, 16));
}
