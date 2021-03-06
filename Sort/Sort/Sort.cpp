// Sort.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


template <typename RandomIt, typename Comparator = std::less<RandomIt::value_type>>
void insertion_sort(RandomIt begin, RandomIt end, Comparator comp = Comparator());

template <typename RandomIt, typename Comparator = std::less<RandomIt::value_type>>
void insertion_sort(RandomIt begin, RandomIt end, Comparator comp = Comparator())
{
	// TODO
	//if (std::distance(begin, end) == 2)
	//	return comp(begin, end);
	const auto TryPermutation = [](RandomIt it1, RandomIt it2)
	{
		if (comp(*it1, *it2))
			*(it2 + 1) = *it2;
	};

	for (RandomIt it1 = begin + 1; it1 != end; ++it1)
	{
		//RandomIt curr = it;
		for (RandomIt it2 = it1 - 1; it2 != begin; --it2)
		{
			if (comp(*it1, *it2))
				*(it2 + 1) = *it2;
			else
				break;
		}

		*(it2 + 1) = *it;
	}
}

//
int main()
{
	system("pause");
    return 0;
}

