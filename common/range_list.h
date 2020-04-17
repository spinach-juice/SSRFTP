#ifndef __RANGE__LIST__H__
#define __RANGE__LIST__H__

struct dual_link
{
	unsigned long start;
	unsigned long end;
	dual_link* next;
};

class RangeList
{
public:
	RangeList(unsigned long min, unsigned long max);
	~RangeList();

	std::vector<unsigned long> get_single_list();
	std::vector<unsigned long> get_range_list();

	void remove(unsigned long num);
	void add(unsigned long num);

	void impress(std::vector<unsigned long> singles, std::vector<unsigned long> ranges);

	bool is_in_list(num);

private:
	unsigned long minimum;
	unsigned long maximum;

	dual_link* headptr;
};



#endif
