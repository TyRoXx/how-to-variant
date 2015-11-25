#ifndef HOW_TO_VARIANT_HPP
#define HOW_TO_VARIANT_HPP

#include <utility>
#include <cstdint>
#include <array>
#include <new>

namespace how
{
	template <class T>
	struct variant_element
	{
		union
		{
			T value;
		};

		variant_element()
		{
		}

		~variant_element()
		{
		}
	};

	template <bool ...V>
	struct any;

	template <bool ...V>
	struct any<true, V...> : std::true_type
	{
	};

	template <bool ...V>
	struct any<false, V...> : any<V...>
	{
	};

	template <>
	struct any<> : std::false_type
	{
	};

	template <class Needle, class ...Haystack>
	struct find;

	template <class Needle, class Hay, class ...Haystack>
	struct find<Needle, Hay, Haystack...> : std::integral_constant<std::size_t, 1 + find<Needle, Haystack...>::value>
	{
	};

	template <class Needle, class ...Haystack>
	struct find<Needle, Needle, Haystack...> : std::integral_constant<std::size_t, 0>
	{
	};

	template <class ...T>
	struct variant : private variant_element<T>...
	{
		~variant() noexcept
		{
			static std::array<void(*)(variant &), sizeof...(T)> const destructors =
			{{
				&destroy<T>...
			}};
			destructors[which()](*this);
		}

		variant(variant &&other)
			: m_which(other.m_which)
		{
			static std::array<void(*)(variant &, variant &&), sizeof...(T)> const move_constructors =
			{{
				&move_construct<T>...
			}};
			move_constructors[which()](*this, std::move(other));
		}

		variant &operator = (variant &&other)
		{
			return *this;
		}

		variant(variant const &other)
			: m_which(other.m_which)
		{
			static std::array<void(*)(variant &, variant const &), sizeof...(T)> const copy_constructors =
			{{
				&copy_construct<T>...
			}};
			copy_constructors[which()](*this, other);
		}

		variant &operator = (variant const &other)
		{
			return *this;
		}

		template <class U, class = typename std::enable_if<any<std::is_same<typename std::decay<U>::type, T>::value...>::value, void>::type>
		variant(U &&value)
		{
			typedef typename std::decay<U>::type clean_element_type;
			clean_element_type &element = static_cast<variant_element<clean_element_type> &>(*this).value;
			new (static_cast<void *>(&element)) clean_element_type(std::forward<U>(value));
			m_which = find<clean_element_type, T...>::value;
		}

		std::size_t which() const noexcept
		{
			return m_which;
		}

	private:

		std::uint8_t m_which;

		template <class U>
		static void destroy(variant &this_) noexcept
		{
			static_cast<variant_element<U> &>(this_).value.~U();
		}

		template <class U>
		static void copy_construct(variant &to, variant const &from)
		{
			new (static_cast<void *>(&static_cast<variant_element<U> &>(to).value)) U(static_cast<variant_element<U> const &>(from).value);
		}

		template <class U>
		static void move_construct(variant &to, variant &&from)
		{
			new (static_cast<void *>(&static_cast<variant_element<U> &>(to).value)) U(std::move(static_cast<variant_element<U> const &>(from).value));
		}
	};
}

#endif
