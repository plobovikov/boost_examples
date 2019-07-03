#include "stdafx.h"

#include <iostream>
#include <string>

#include <boost/algorithm/searching/boyer_moore.hpp>
#include <boost/algorithm/searching/boyer_moore_horspool.hpp>
#include <boost/algorithm/searching/knuth_morris_pratt.hpp>

#include <boost/algorithm/cxx11/all_of.hpp>
#include <boost/algorithm/cxx11/any_of.hpp>
#include <boost/algorithm/cxx11/none_of.hpp>
#include <boost/algorithm/cxx11/one_of.hpp>

#include <boost/algorithm/cxx14/equal.hpp>

#include <boost/algorithm/clamp.hpp>
#include <boost/algorithm/gather.hpp>
#include <boost/algorithm/hex.hpp>

template<class StringType, template<class,class> class Searcher>
void search_pattern(const StringType& sPattern, const StringType& sInputString)
{
	Searcher<typename StringType::const_iterator, boost::algorithm::detail::BM_traits<typename StringType::const_iterator> > searcher(sPattern.begin(), sPattern.end());
	auto it = searcher(sInputString.begin(), sInputString.end()); 

	std::cout << "Start position: " << std::distance(sInputString.begin(), it.first)
		<< ". End position: " << std::distance(sInputString.begin(), it.second) << std::endl;
}

template<class StringType, template<class> class Searcher>
void search_pattern(const StringType& sPattern, const StringType& sInputString)
{
	Searcher<typename StringType::const_iterator> searcher(sPattern.begin(), sPattern.end());
	auto it = searcher(sInputString.begin(), sInputString.end());

	std::cout << "Start position: " << std::distance(sInputString.begin(), it.first)
		<< ". End position: " << std::distance(sInputString.begin(), it.second) << std::endl;
}

int main()
{
	// Searching algorithms
	{
		search_pattern<std::string, boost::algorithm::boyer_moore>("World", "Hello, World!"); // --> start == 7, end == 12
		search_pattern<std::string, boost::algorithm::boyer_moore_horspool>("Bob", "Hello, Bob!"); // --> start == 7, end == 10
		search_pattern<std::string, boost::algorithm::knuth_morris_pratt>("Alice", "    Hello, Alice!    "); // --> start == 11, end == 16

		std::cout << std::endl;
	}
	
	// Some of C++11 algorithms

	{
		// all_of
		{
			std::vector<int> vElements{ 1, 2, 3, -5 , 5 };
			std::cout << std::boolalpha << boost::algorithm::all_of(vElements, [](int nElement) -> bool { return nElement > 0; }) << std::endl; // --> false
			std::cout << std::boolalpha << boost::algorithm::all_of(vElements.begin(), vElements.begin() + 3, [](int nElement) -> bool { return nElement > 0; }) << std::endl; // --> true

			std::cout << std::endl;
		}
		
		// any_of

		{
			std::vector<std::string> vElements{ "Hello", "world" , "!", "" };
			std::cout << std::boolalpha << boost::algorithm::any_of(vElements, [](const std::string& sString) -> bool { return sString.empty(); }) << std::endl; // --> true
			std::cout << std::boolalpha << boost::algorithm::any_of(vElements.begin(), vElements.begin() + 3, [](const std::string& sString) -> bool { return sString.empty(); }) << std::endl; // --> false
		
			std::cout << std::endl;
		}

		// none_of

		{
			std::vector<char> vElements{ 'a','b','c','d' };
			std::cout << std::boolalpha << boost::algorithm::none_of(vElements, [](char c) -> bool { return ::isdigit(c); }) << std::endl; // --> true
			std::cout << std::boolalpha << boost::algorithm::none_of(vElements.begin(), vElements.end(), [](char c) -> bool { return ::isalnum(c); }) << std::endl; // --> false

			std::cout << std::endl;
		}

		// one_of

		{
			
			std::vector<std::pair<int, int> > vElements{ {2, 2}, {2, 3}, {1, 1}, {2, 2}, {1, 3} };

			std::cout << std::boolalpha << boost::algorithm::one_of(vElements, [](auto pair) -> bool { return (pair.first == pair.second); }) << std::endl; // --> false
			std::cout << std::boolalpha << boost::algorithm::one_of(vElements.begin(), vElements.end(), [](auto pair) -> bool { return ((pair.first + pair.second) == 2); }) << std::endl; // --> true

			std::cout << std::endl;
		}
	}

	// Other algorithms
	{
		// clamp
		{
			std::vector<int> vSequence{ -3, -2, 1, 3, 5, 6, 11, 15};

			boost::algorithm::clamp_range(vSequence.begin(), vSequence.end(), vSequence.begin(), 0, 10);

			std::copy(vSequence.begin(), vSequence.end(), std::ostream_iterator<int>(std::cout, ", "));  // 0 0 1 3 5 6 10 10

			std::cout << std::endl << std::endl;
		}

		// gather
		{
			std::vector<int> vSequence{ -1, 2, -3, 4, -5, 6, -7 };

			auto pair = boost::algorithm::gather(vSequence.begin(), vSequence.end(), vSequence.begin() + 3, [](int nElement) { return nElement < 0; }); // collect negative elements near pivot elements(4).

			std::cout << "Start position: " << std::distance(vSequence.begin(), pair.first)
				<< ". End position: " << std::distance(vSequence.begin(), pair.second) << std::endl;

			std::copy(vSequence.begin(), vSequence.end(), std::ostream_iterator<int>(std::cout, ", ")); //  Result: (2 - 1 - 3 - 5 - 7 4 6)

			std::cout << std::endl << std::endl;

		}

		// hex

		{
			std::string sResult;

			std::vector<char> vSequence{ 'a', 'B', 'e', '1', '!' }, vIdenticalSequence;

			std::for_each(vSequence.begin(), vSequence.end(), [](char c) { std::cout << std::hex << static_cast<int>(c) << " "; });
			
			std::cout << std::endl;

			boost::algorithm::hex(vSequence.begin(), vSequence.end(), std::back_inserter(sResult));
			
			for (int i = 0; i < sResult.size(); i++)
			{
				std::cout << sResult[i] << (i % 2 != 0 ? " " : "");
			}

			std::cout << std::endl << std::endl;

			boost::algorithm::unhex(sResult.begin(), sResult.end(), std::back_inserter(vIdenticalSequence));

			std::cout << std::boolalpha << boost::algorithm::equal(vIdenticalSequence.begin(), vIdenticalSequence.end(), vSequence.begin(), vSequence.end()); // --> true
		}
	}
	return 0;
}