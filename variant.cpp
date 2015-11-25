#include "variant.hpp"
#include <boost/test/unit_test.hpp>
#include <vector>
#include <array>
using namespace how;

BOOST_AUTO_TEST_CASE(variant_construction)
{
	typedef variant<int, std::vector<int>, std::array<int, 3>, std::string> test_variant;
	test_variant v(std::string("hello"));
	test_variant w(v);
}
