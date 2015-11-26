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

struct check_hello_string_visitor
{
	typedef bool result_type;

	template <class T>
	result_type operator()(T const &) const
	{
		return false;
	}

	result_type operator()(std::string const &element) const
	{
		return ("hello" == element);
	}
};

BOOST_AUTO_TEST_CASE(variant_copy_assignment_same)
{
	typedef variant<int, std::vector<int>, std::array<int, 3>, std::string> test_variant;
	test_variant v(std::string("hello"));
	test_variant w(std::string("world"));
	w = v;
	BOOST_CHECK(v.apply_visitor(check_hello_string_visitor{}));
	BOOST_CHECK(w.apply_visitor(check_hello_string_visitor{}));
}

BOOST_AUTO_TEST_CASE(variant_copy_assignment_different)
{
	typedef variant<int, std::vector<int>, std::array<int, 3>, std::string> test_variant;
	test_variant v(std::vector<int>(10));
	test_variant w(std::string("hello"));
	BOOST_CHECK(!v.apply_visitor(check_hello_string_visitor{}));
	v = w;
	BOOST_CHECK(v.apply_visitor(check_hello_string_visitor{}));
	BOOST_CHECK(w.apply_visitor(check_hello_string_visitor{}));
}

BOOST_AUTO_TEST_CASE(variant_apply_visitor)
{
	typedef variant<int, std::vector<int>, std::array<int, 3>, std::string> test_variant;
	test_variant v(std::string("hello"));
	BOOST_CHECK(v.apply_visitor(check_hello_string_visitor{}));
}

BOOST_AUTO_TEST_CASE(variant_apply_visitor_const)
{
	typedef variant<int, std::vector<int>, std::array<int, 3>, std::string> test_variant;
	test_variant const v(std::string("hello"));
	BOOST_CHECK(v.apply_visitor(check_hello_string_visitor{}));
}

BOOST_AUTO_TEST_CASE(variant_sizeof)
{
	BOOST_CHECK_EQUAL(sizeof(std::uint8_t) * 2, sizeof(variant<std::uint8_t, bool, char>));
	BOOST_CHECK_EQUAL(sizeof(std::uint32_t) * 2, sizeof(variant<std::uint8_t, std::uint32_t>));
	BOOST_CHECK_EQUAL(sizeof(std::uint32_t) * 2, sizeof(variant<std::uint32_t, std::uint8_t>));
}
