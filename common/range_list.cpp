#include "range_list.h"
#include <algorithm>
#include <iostream>

RangeList::RangeList()
{
	this->minimum = 0;
	this->maximum = 0;
	this->headptr = nullptr;
}

RangeList::RangeList(unsigned long min, unsigned long max)
{
	this->init(min, max);
}

RangeList& RangeList::operator=(const RangeList& copy)
{
	this->minimum = copy.minimum;
	this->maximum = copy.maximum;
	this->single_list = nullptr;
	this->range_list = nullptr;

	dual_link* curr_cp = copy.headptr;
	if(curr_cp == nullptr)
	{
		this->headptr = nullptr;
		return *this;
	}

	this->headptr = new dual_link;
	dual_link* curr = this->headptr;
	curr->start = curr_cp->start;
	curr->end = curr_cp->end;
	curr->next = nullptr;

	curr_cp = curr_cp->next;

	while(curr_cp != nullptr)
	{
		curr->next = new dual_link;
		curr = curr->next;

		curr->start = curr_cp->start;
		curr->end = curr_cp->end;
		curr->next = nullptr;

		curr_cp = curr_cp->next;
	}
	return *this;
}

RangeList::~RangeList()
{
	dual_link* curr = this->headptr;
	dual_link* temp = nullptr;

	while(curr != nullptr)
	{
		temp = curr;
		curr = curr->next;
		delete temp;
	}

	if(this->single_list != nullptr)
		delete[] this->single_list;

	if(this->range_list != nullptr)
		delete[] this->range_list;
}

void RangeList::init(unsigned long min, unsigned long max)
{
	this->minimum = min;
	this->maximum = max;
	this->headptr = new dual_link;
	this->headptr->next = nullptr;
	this->headptr->start = min;
	this->headptr->end = max;
	this->single_list = nullptr;
	this->range_list = nullptr;
}

unsigned long* RangeList::get_single_list(unsigned long& size)
{
	dual_link* curr = this->headptr;
	size = 0;

	while(curr != nullptr)
	{
		if(curr->start == curr->end)
			size++;

		curr = curr->next;
	}

	if(this->single_list != nullptr)
		delete[] this->single_list;

	this->single_list = new unsigned long[size];

	curr = this->headptr;
	size = 0;

	while(curr != nullptr)
	{
		if(curr->start == curr->end)
		{
			this->single_list[size++] = curr->start;
		}
		curr = curr->next;
	}

	return this->single_list;
}

unsigned long* RangeList::get_range_list(unsigned long& size)
{
	dual_link* curr = this->headptr;
	size = 0;

	while(curr != nullptr)
	{
		if(curr->start != curr->end)
			size += 2;
		curr = curr->next;
	}

	if(this->range_list != nullptr)
		delete[] this->range_list;

	this->range_list = new unsigned long[size];

	curr = this->headptr;
	size = 0;

	while(curr != nullptr)
	{
		if(curr->start != curr->end)
		{
			this->range_list[size++] = curr->start;
			this->range_list[size++] = curr->end;
		}
		curr = curr->next;
	}
	return this->range_list;
}

void RangeList::remove(unsigned long num)
{
	if(!this->is_in_list(num))
		return;

	dual_link* current_range = this->headptr;
	dual_link* last_range = nullptr;
	bool removed = false;

	while(!removed)
	{
		if(num == current_range->start)
		{
			if(current_range->start == current_range->end)
			{
				if(current_range == this->headptr)
					this->headptr = current_range->next;
				else
					last_range->next = current_range->next;

				delete current_range;
			}
			else
				current_range->start++;

			removed = true;
		}
		else if(num > current_range->start && num < current_range->end)
		{
			dual_link* temp = current_range->next;
			current_range->next = new dual_link;
			current_range->next->end = current_range->end;
			current_range->next->start = num + 1;
			current_range->next->next = temp;
			current_range->end = num - 1;

			removed = true;
		}
		else if(num == current_range->end)
		{
			current_range->end--;

			removed = true;
		}

		if(!removed)
		{
			last_range = current_range;
			current_range = current_range->next;
		}
	}
}

void RangeList::add(unsigned long num)
{
	if(this->is_in_list(num))
		return;

	bool added = false;

	dual_link* current_range = this->headptr;

	while(!added)
	{
		if(num == current_range->end + 1)
		{
			if(num == current_range->next->start - 1)
			{
				current_range->end = current_range->next->end;
				dual_link* temp = current_range->next;
				current_range->next = current_range->next->next;
				delete temp;
			}
			else
				current_range->end++;

			added = true;
		}
		else if(num == current_range->start - 1)
		{
			current_range->start--;

			added = true;
		}
		else if(num > current_range->end && num < current_range->next->start)
		{
			dual_link* temp = current_range->next;
			current_range->next = new dual_link;
			current_range->next->start = num;
			current_range->next->end = num;
			current_range->next->next = temp;

			added = true;
		}

		if(!added)
			current_range = current_range->next;
	}
}

void RangeList::impress(std::vector<unsigned long> singles, std::vector<unsigned long> ranges)
{
	dual_link* curr = this->headptr;
	dual_link* temp = nullptr;

	while(curr != nullptr)
	{
		temp = curr;
		curr = curr->next;
		delete temp;
	}
	this->headptr = nullptr;

	std::sort(singles.begin(), singles.end());
	std::sort(ranges.begin(), ranges.end());

	unsigned long i = 0;
	unsigned long j = 0;
	while(i < singles.size() && j < ranges.size())
	{
		if(j >= ranges.size() || (i < singles.size() && singles.at(i) < ranges.at(j)))
		{
			if(this->headptr == nullptr)
			{
				this->headptr = new dual_link;
				curr = this->headptr;
			}
			else
			{
				curr->next = new dual_link;
				curr = curr->next;
			}
			curr->start = singles.at(i);
			curr->end = singles.at(i);
			curr->next = nullptr;

			i++;
		}
		else
		{
			if(this->headptr == nullptr)
			{
				this->headptr = new dual_link;
				curr = this->headptr;
			}
			else
			{
				curr->next = new dual_link;
				curr = curr->next;
			}
			curr->start = ranges.at(j);
			curr->end = ranges.at(j+1);
			curr->next = nullptr;

			j += 2;
		}
	}
}

bool RangeList::is_in_list(unsigned long num)
{
	if(num < this->minimum || num > this->maximum)
		return false;
	std::cout << "Is " << num << " in list?\n";
	dual_link* current_range = this->headptr;

	while(current_range != nullptr)
	{std::cout << "start " << current_range->start << " end " << current_range->end << std::endl;
		if(num >= current_range->start && num <= current_range->end)
			return true;
		else if(num < current_range->start)
			return false;

		current_range = current_range->next;
	}

	return false;
}


void RangeList::set_min(unsigned long num)
{
	this->minimum = num;
}

void RangeList::set_max(unsigned long num)
{
	this->maximum = num;
}
