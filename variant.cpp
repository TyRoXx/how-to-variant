#include "variant.hpp"
#include <boost/test/unit_test.hpp>
#include <vector>
#include <array>
using namespace how;

BOOST_AUTO_TEST_CASE(variant_construction)
{
	typedef variant<int, std::vector<int>, std::array<int, 3>, std::string> test_variant;
	test_variant v(std::string("hello"));
	BOOST_CHECK_EQUAL(3u, v.which());
	test_variant w(v);
	BOOST_CHECK_EQUAL(3u, w.which());
	test_variant x(std::move(w));
	BOOST_CHECK_EQUAL(3u, w.which());
	BOOST_CHECK_EQUAL(3u, x.which());
}

#if 0
BOOST_AUTO_TEST_CASE(variant_copy_assignment_same)
{
	typedef variant<int, std::vector<int>, std::array<int, 3>, std::string> test_variant;
	test_variant v(std::string("hello"));
	test_variant w(std::string("world"));
	v = w;
}
#endif

struct expected_string_visitor
{
	typedef int result_type;

	template <class T>
	result_type operator()(T const &) const
	{
		BOOST_FAIL("unexpected element type");
		return 0;
	}

	result_type operator()(std::string const &element) const
	{
		BOOST_CHECK_EQUAL("hello", element);
		return 23;
	}
};

BOOST_AUTO_TEST_CASE(variant_apply_visitor)
{
	typedef variant<int, std::vector<int>, std::array<int, 3>, std::string> test_variant;
	test_variant v(std::string("hello"));
	BOOST_CHECK_EQUAL(23, v.apply_visitor(expected_string_visitor{}));
}

BOOST_AUTO_TEST_CASE(variant_apply_visitor_const)
{
	typedef variant<int, std::vector<int>, std::array<int, 3>, std::string> test_variant;
	test_variant const v(std::string("hello"));
	BOOST_CHECK_EQUAL(23, v.apply_visitor(expected_string_visitor{}));
}

BOOST_AUTO_TEST_CASE(variant_sizeof)
{
	BOOST_CHECK_EQUAL(sizeof(std::uint8_t) * 2, sizeof(variant<std::uint8_t, bool, char>));
}
