#ifndef __RANGE__LIST__H__
#define __RANGE__LIST__H__

#include <vector>

// The basis for a linked list structure.
// Payload is two 32 bit integers.
struct dual_link
{
	unsigned long start;
	unsigned long end;
	dual_link* next;
};

// This class keeps track of membership in a subset within a contiguous set of integers.
// The modifiers can either grant or revoke membership.
// The accessors return lists of members.
class RangeList
{
public:
	RangeList();
	RangeList(unsigned long min, unsigned long max);
	RangeList& operator=(const RangeList&);
	~RangeList();

	// The Single List contains members which have no contiguous neighbors which are also members.
	unsigned long* get_single_list(unsigned long& size);
	// The Range List contains pairs which specify a range of members starting with the first of the pair and ending with the second of the pair.
	unsigned long* get_range_list(unsigned long& size);

	void init(unsigned long min, unsigned long max);

	void remove(unsigned long num);
	void add(unsigned long num);

	// This takes in a list of Singles and Ranges and modifies the class data to match them.
	void impress(std::vector<unsigned long> singles, std::vector<unsigned long> ranges);

	bool is_in_list(unsigned long num);

	void set_min(unsigned long num);
	void set_max(unsigned long num);

private:
	unsigned long minimum;
	unsigned long maximum;

	// These contain temporary data to be used by calling functions.
	unsigned long* single_list;
	unsigned long* range_list;

	dual_link* headptr;
};



#endif
