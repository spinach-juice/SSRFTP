#ifndef __RANGE__LIST__H__
#define __RANGE__LIST__H__

#include <vector>

struct dual_link
{
	unsigned long start;
	unsigned long end;
	dual_link* next;
};

class RangeList
{
public:
	RangeList();
	RangeList(unsigned long min, unsigned long max);
	RangeList& operator=(const RangeList&);
	~RangeList();

	unsigned long* get_single_list(unsigned long& size);
	unsigned long* get_range_list(unsigned long& size);

	void init(unsigned long min, unsigned long max);

	void remove(unsigned long num);
	void add(unsigned long num);

	void impress(std::vector<unsigned long> singles, std::vector<unsigned long> ranges);

	bool is_in_list(unsigned long num);

	void set_min(unsigned long num);
	void set_max(unsigned long num);

private:
	unsigned long minimum;
	unsigned long maximum;

	unsigned long* single_list;
	unsigned long* range_list;

	dual_link* headptr;
};



#endif
