#ifndef HOW_TO_VARIANT_HPP
#define HOW_TO_VARIANT_HPP

#include <utility>
#include <cstdint>

namespace how
{
	template <class T>
	struct variant_element
	{
		union
		{
			T value;
		};
	};

	template <bool ...V>
	struct all;

	template <bool ...V>
	struct all<true, V...> : all<V...>
	{
	};

	template <bool ...V>
	struct all<false, V...> : std::false_type
	{
	};

	template <>
	struct all<> : std::true_type
	{
	};

	template <class ...T>
	struct variant : private variant_element<T>...
	{
		~variant() noexcept
		{
		}

		variant(variant &&other)
		{
		}

		variant &operator = (variant &&other)
		{
			return *this;
		}

		variant(variant const &other)
		{
		}

		variant &operator = (variant const &other)
		{
			return *this;
		}

		template <class U, class = std::enable_if<all<std::is_same<typename std::decay<U>::type, T>::value...>::value, void>>
		variant(U &&value)
		{

		}

		std::size_t which() const noexcept
		{
			return m_which;
		}

	private:

		std::uint8_t m_which;
	};
}

#endif
