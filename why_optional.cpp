#include "optional.hpp"
#include <vector>
#include <array>
using namespace how;

namespace
{
	struct no_default_constructor
	{
		explicit no_default_constructor(int)
		{
		}

		no_default_constructor(no_default_constructor const &) = delete;
		no_default_constructor &operator = (no_default_constructor const &) = delete;
	};

	optional<no_default_constructor> n;
	void h()
	{
		n.emplace(123);
	}

	struct configuration
	{
		// -1 for not applicable
		int id_of_something;

		optional<int> better_id_of_something;
	};

	optional<int> sqrt(int);

	//none: 1,2,3
	//' ':  1, 2, 3
	//'\n': 1,\n2,\n3
	void format_stuff(std::ostream &, std::vector<int> const &, optional<char> separator);

	void g(optional<std::string> const &)
	{
	}

	void f(bool b)
	{
		optional<std::string> i;
		if (b)
		{
			i = std::string("2");
		}
		g(i);
	}
}
