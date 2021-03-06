// insertionSort.cpp : Defines the entry point for the console application.
//

#include <gtest\gtest.h>
#include <algorithm>
#include <array>
#include <functional>
#include <iostream>
#include <vector>
#include <random>
#include <cstdint>
#include <cmath>
#include <numeric>

#include <chrono>
using std::chrono::high_resolution_clock;
using std::chrono::duration;
using std::chrono::duration_cast;



template <class RandomIt, class Compare = std::less<RandomIt::value_type>>
void insertion_sort(RandomIt begin, RandomIt end, Compare comp = Compare());


typedef std::tuple<uint32_t, uint32_t> UintPair;

std::ostream& operator<<(std::ostream& os, const UintPair& tup)
{
	return os << "(" << std::get<0>(tup) << ", " << std::get<1>(tup) << ")";
}

// generates sorted array = {0, 1, 2, 3, .. size - 1}
std::vector<uint32_t> sorted_array_of_size(size_t size)
{
	std::vector<size_t> arr;
	arr.reserve(size);
	for (uint32_t i = 0; i < size; ++i)
		arr.push_back(i);

	return arr;
}

// generates reversed array = {size - 1, size - 2, ... 3, 2, 1, 0}
std::vector<uint32_t> reversed_array_of_size(size_t size)
{
	std::vector<size_t> arr;
	arr.reserve(size);
	for (uint32_t i = size; i > 0; --i)
		arr.push_back(i - 1);

	return arr;
}

// generates random array with unique elements 
std::vector<uint32_t> random_unique_array_of_size(size_t size)
{
	std::vector<uint32_t> arr;
	arr.reserve(size);
	for (uint32_t i = 0; i < size; ++i)
		arr.push_back(i);

	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(std::begin(arr), std::end(arr), g);

	return arr;
}

// generates random array with not unique elements from range = [0, max_value]
std::vector<uint32_t> random_array_with_duplicates_of_size(size_t size, uint32_t max_value)
{
	std::random_device rd;
	std::mt19937 g(rd());
	std::uniform_int_distribution<uint32_t> dis(0, max_value - 1);

	std::vector<uint32_t> arr;
	arr.reserve(size);
	for (size_t i = 0; i < size; ++i)
		arr.push_back(dis(g));

	return arr;
}

// generates random array with not unique elements from range = [0, max_value]
std::vector<UintPair> random_array_of_pairs(size_t size, uint32_t max_value)
{
	std::random_device rd;
	std::mt19937 g(rd());
	std::uniform_int_distribution<uint32_t> dis(0, max_value - 1);

	std::vector<UintPair> arr;
	arr.reserve(size);
	for (size_t i = 0; i < size; ++i)
		arr.push_back({ dis(g), i });

	return arr;
}


template <class Array> 
std::string print_array(const Array& arr)
{
	std::stringstream stream;
	
	stream << "[";

	bool firstItem = true;
	for (const auto& item : arr)
	{
		if (!firstItem)
			stream << ", ";
		else
			firstItem = false;
			
		stream << item;
	}

	stream << "]";

	return stream.str();
}

class SortingTest : public ::testing::TestWithParam<size_t>
{
};


TEST(SortingTest, sorted_arrays)
{
	for (size_t size = 1; size < 100; ++size)
	{
		auto arr = sorted_array_of_size(size);
		auto original = arr;
		insertion_sort(std::begin(arr), std::end(arr));
		ASSERT_TRUE(std::is_sorted(std::begin(arr), std::end(arr))) << "bad case = " << print_array(original);
	}
}

TEST(SortingTest, reversed_arrays)
{
	for (size_t size = 1; size < 100; ++size)
	{
		auto arr = reversed_array_of_size(size);
		auto original = arr;
		insertion_sort(std::begin(arr), std::end(arr));
		ASSERT_TRUE(std::is_sorted(std::begin(arr), std::end(arr))) << "bad case = " << print_array(original);
	}
}


TEST(SortingTest, random_arrays)
{
	for (size_t size = 1; size < 100; ++size)
	{
		auto arr = random_unique_array_of_size(size);
		auto original = arr;
		insertion_sort(std::begin(arr), std::end(arr));
		ASSERT_TRUE(std::is_sorted(std::begin(arr), std::end(arr))) << "bad case = " << print_array(original);
	}
}

TEST(SortingTest, random_arrays_with_duplicates)
{
	for (size_t size = 1; size < 100; ++size)
	{
		for (size_t max_val = 1; max_val < 50; ++max_val)
		{
			auto arr = random_array_with_duplicates_of_size(size, max_val);
			auto original = arr;
			insertion_sort(std::begin(arr), std::end(arr));
			ASSERT_TRUE(std::is_sorted(std::begin(arr), std::end(arr))) << "bad case = " << print_array(original);
		}
	}
}

TEST(SortingTest, stability)
{
	for (size_t size = 1; size < 100; ++size)
	{
		for (size_t max_val = 1; max_val < 50; ++max_val)
		{
			auto arr = random_array_of_pairs(size, max_val);
			auto original = arr;

			// sort only by first element as a key, so we hide second component for sort algorithm
			insertion_sort(std::begin(arr), std::end(arr), [=](const UintPair& lhs, const UintPair& rhs){ return std::get<0>(lhs) < std::get<0>(rhs); });
			
			// but here we use lexicographical comparison to check stability
			ASSERT_TRUE(std::is_sorted(std::begin(arr), std::end(arr))) << "bad case = " << print_array(original);
		}
	}
}

TEST_P(SortingTest, complexity_power_test)
{
	// check a hypothesis T(n) = C*n^p, where:
	// T - working time,
	// N - array size,
	// C - some constant
	// p - some exponent
	const size_t SampleSize = 100;

#ifdef _DEBUG
	const size_t ArrayBaseSize = 100;
#else
	const size_t ArrayBaseSize = 10000;
#endif // DEBUG
	
	std::vector<double> duration_data1;
	std::vector<double> duration_data2;

	const double Epsilon = 0.1;
	const size_t power_to_check = GetParam();

	for (size_t i = 0; i < SampleSize; ++i)
	{
		auto arr = random_unique_array_of_size(ArrayBaseSize);
		auto original = arr;

		high_resolution_clock::time_point start = high_resolution_clock::now();
		insertion_sort(std::begin(arr), std::end(arr));
		high_resolution_clock::time_point end = high_resolution_clock::now();

		// here we save time in seconds
		duration_data1.push_back(duration_cast<duration<double>>(end - start).count());
		ASSERT_TRUE(std::is_sorted(std::begin(arr), std::end(arr))) << "bad case = " << print_array(original);
	}

	for (size_t i = 0; i < SampleSize; ++i)
	{
		auto arr = random_unique_array_of_size(2 * ArrayBaseSize);
		auto original = arr;

		high_resolution_clock::time_point start = high_resolution_clock::now();
		insertion_sort(std::begin(arr), std::end(arr));
		high_resolution_clock::time_point end = high_resolution_clock::now();

		// here we save time in seconds
		duration_data2.push_back(duration_cast<duration<double>>(end - start).count());
		ASSERT_TRUE(std::is_sorted(std::begin(arr), std::end(arr))) << "bad case = " << print_array(original);
	}

	const double average_time1 = std::accumulate(std::begin(duration_data1), std::end(duration_data1), 0.0) / duration_data1.size();
	const double average_time2 = std::accumulate(std::begin(duration_data2), std::end(duration_data2), 0.0) / duration_data2.size();

	const double power = std::log2(average_time2 / average_time1);
	ASSERT_TRUE(std::abs(power_to_check - power) <= Epsilon) << "power = " << power;
}

INSTANTIATE_TEST_CASE_P(CheckPowerOfTwoComplexity, SortingTest, ::testing::Values(2.0));

int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	int res = RUN_ALL_TESTS();
	return res;
}

