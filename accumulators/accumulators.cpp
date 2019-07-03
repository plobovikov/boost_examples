#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/moment.hpp>
#include <boost/accumulators/statistics/min.hpp>

#include <boost/accumulators/framework/accumulator_base.hpp>
#include <boost/accumulators/framework/parameters/sample.hpp>

namespace boost {                          
	namespace accumulators {                    
		namespace impl {                            

			template<typename Sample>
			struct sum_of_squares_accumulator                    
				: accumulator_base                        
			{
				typedef Sample result_type;            

				template<typename Args>                 
				sum_of_squares_accumulator(Args const & args)
					: sum(args[sample | Sample()] * args[sample | Sample()])
														
				{                                       
				}                                       

				template<typename Args>                 
				void operator ()(Args const & args)     
				{                                      
					this->sum += args[sample] * args[sample];
				}

				result_type result(dont_care) const     
				{                                       
					return this->sum;                  
				}
			private:
				Sample sum;
			};

			template<typename Sample>
			struct sqrt_from_sum_of_squares_accumulator : accumulator_base
			{
				typedef Sample result_type;
				sqrt_from_sum_of_squares_accumulator(dont_care) {}

				template<typename Args>
				result_type result(Args const &args) const
				{
					return sqrt(sum_of_squares(args[accumulator]));
				}
			};

		}
	}
}

namespace boost {
	namespace accumulators {
		namespace tag {

			struct sum_of_squares
				: depends_on<>
			{
				typedef accumulators::impl::sum_of_squares_accumulator< mpl::_1> impl;
			};

			struct sqrt_from_sum_of_squares
				: depends_on<sum_of_squares>
			{
				typedef accumulators::impl::sqrt_from_sum_of_squares_accumulator< mpl::_1> impl;
			};
		}
	}
}

namespace boost {
	namespace accumulators {                
		namespace extract {                    

			extractor< tag::sum_of_squares > const sum_of_squares = {}; 
			extractor< tag::sqrt_from_sum_of_squares > const sqrt_from_sum_of_squares = {};
												
		}

		using extract::sum_of_squares;
		using extract::sqrt_from_sum_of_squares;
												
	}
}

using namespace boost::accumulators;

int main()
{
	std::vector<double> vElements;
	std::generate_n(std::back_inserter(vElements), 10, []() -> double { return { static_cast<double>(rand() % 10 + 1) }; });
	std::copy(vElements.begin(), vElements.end(), std::ostream_iterator<double>(std::cout, ", "));

	accumulator_set<double, 
		features<
				 tag::mean, 
				 droppable<tag::sum_of_squares>, 
				 droppable<tag::sqrt_from_sum_of_squares> 
				> 
	> accumulator;

	std::for_each<decltype(vElements)::iterator,
		std::add_lvalue_reference<decltype(accumulator)>::type >(vElements.begin(), vElements.end(), accumulator);
	
	std::cout << "\nMean:   " << mean(accumulator) << std::endl;
	std::cout << "Sum of square: " << sum_of_squares(accumulator) << std::endl;
	std::cout << "Sqrt from sum of square: " << sqrt_from_sum_of_squares(accumulator) << std::endl;

	accumulator.drop<tag::sum_of_squares>();
	accumulator.drop<tag::sqrt_from_sum_of_squares>();

	accumulator(5.5);

	std::cout << "Mean:   " << mean(accumulator) << std::endl;
	std::cout << "Sum of squares: " << sum_of_squares(accumulator) << std::endl;
	std::cout << "Sqrt from sum of squares: " << sqrt_from_sum_of_squares(accumulator) << std::endl;

	return 0;
}