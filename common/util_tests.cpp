#include "catch.hpp"
#include "util.h"

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

TEST_CASE("Utility Functions: ulong_array_max")
{
	unsigned long array[] = {0x6789, 0xaaaa, 0x1234, 0xdddd, 0x4321, 0xbbbb, 0x9876, 0xcccc};

	unsigned long max = ulong_array_max(array, 8);

	unsigned long valid_output = 0xdddd;

	CHECK(max == valid_output);
}

TEST_CASE("Utility Functions: ulong_array_min")
{
	unsigned long array[] = {0x6789, 0xaaaa, 0x1234, 0xdddd, 0x4321, 0xbbbb, 0x9876, 0xcccc};

	unsigned long min = ulong_array_min(array, 8);

	unsigned long valid_output = 0x1234;

	CHECK(min == valid_output);
}

TEST_CASE("Utility Functions: ulong_array_sort")
{
	unsigned long array[] = {0x6789, 0xaaaa, 0x1234, 0xdddd, 0x4321, 0xbbbb, 0x9876, 0xcccc};

	ulong_array_sort(array, 8);

	unsigned long valid_output[] = {0x1234, 0x4321, 0x6789, 0x9876, 0xaaaa, 0xbbbb, 0xcccc, 0xdddd};

	CHECK(ulong_array_equal(array, valid_output, 8));
}

TEST_CASE("Utility Functions: ulong_array_singles")
{
	unsigned long missing_nums[100] = {0x0000, 0x0003, 0x4158, 0x1122, 0x5673, 0xaaaa, 0xf437, 0x0005, 0x0010, 0x0020};

	int i = 10;
	for(; i < 100; i++)
		missing_nums[i] = missing_nums[i - 1] + 1;

	std::vector<unsigned long> valid_output = {0x0000, 0x0003, 0x0005, 0x0010, 0x1122, 0x4158, 0x5673, 0xaaaa, 0xf437};

	ulong_array_sort(missing_nums, 100);

	std::vector<unsigned long> output = ulong_array_singles(missing_nums, 100);

	CHECK(output == valid_output);
}

TEST_CASE("Utility Functions: ulong_array_ranges")
{
	unsigned long missing_nums[100] = {0x0000, 0x0003, 0x4158, 0x1122, 0x5673, 0xaaaa, 0xf437, 0x0005, 0x0010, 0x0020};

	int i = 10;
	for(; i < 100; i++)
		missing_nums[i] = missing_nums[i - 1] + 1;

	std::vector<unsigned long> valid_output = {0x0020, 0x007a};

	ulong_array_sort(missing_nums, 100);

	std::vector<unsigned long> output = ulong_array_ranges(missing_nums, 100);

	CHECK(output == valid_output);
}
