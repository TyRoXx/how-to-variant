#define BOOST_TEST_MAIN
#include "optional.hpp"
#include <boost/test/unit_test.hpp>
#include <vector>
#include <array>
using namespace how;

struct immovable
{
	immovable()
	{
	}

	immovable(immovable const &) = delete;
	immovable &operator = (immovable const &) = delete;
};

BOOST_AUTO_TEST_CASE(optional_emplace)
{
	optional<immovable> o;
	o.emplace();
}

struct empty
{
};

BOOST_AUTO_TEST_CASE(optional_sizeof)
{
	BOOST_CHECK_EQUAL(sizeof(bool) * 2, sizeof(optional<bool>));
	BOOST_CHECK_EQUAL(sizeof(bool) * 2, sizeof(optional<empty>));
}

BOOST_AUTO_TEST_CASE(optional_in_place)
{
	optional<std::vector<int>> o(in_place, 1, 2, 3, 4);
	BOOST_REQUIRE(o);
	std::array<int, 4> const expected{{1, 2, 3, 4}};
	BOOST_CHECK_EQUAL_COLLECTIONS(expected.begin(), expected.end(), o->begin(), o->end());
}

BOOST_AUTO_TEST_CASE(optional_move)
{
	optional<std::vector<int>> const original(in_place, 1, 2, 3, 4);
	optional<std::vector<int>> moved_from = original;
	optional<std::vector<int>> move_constructed(std::move(moved_from));
	BOOST_REQUIRE(moved_from);
	BOOST_CHECK(moved_from->empty());
	BOOST_CHECK_EQUAL_COLLECTIONS(original->begin(), original->end(), move_constructed->begin(), move_constructed->end());
	optional<std::vector<int>> move_assigned;
	move_assigned = std::move(move_constructed);
	BOOST_REQUIRE(move_constructed);
	BOOST_CHECK(move_constructed->empty());
	BOOST_CHECK_EQUAL_COLLECTIONS(original->begin(), original->end(), move_assigned->begin(), move_assigned->end());
}

BOOST_AUTO_TEST_CASE(optional_copy)
{
	optional<std::vector<int>> o(in_place, 1, 2, 3, 4);
	optional<std::vector<int>> copy_constructed(o);
	BOOST_CHECK_EQUAL_COLLECTIONS(o->begin(), o->end(), copy_constructed->begin(), copy_constructed->end());
	optional<std::vector<int>> copy_assigned;
	copy_assigned = o;
	BOOST_CHECK_EQUAL_COLLECTIONS(o->begin(), o->end(), copy_assigned->begin(), copy_assigned->end());
}

BOOST_AUTO_TEST_CASE(optional_dereference)
{
	optional<std::vector<int>> o(in_place, 1, 2, 3, 4);
	std::vector<int> &ref = *o;
	BOOST_CHECK_EQUAL_COLLECTIONS(o->begin(), o->end(), ref.begin(), ref.end());
}
