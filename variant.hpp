#ifndef HOW_TO_VARIANT_HPP
#define HOW_TO_VARIANT_HPP

#include <utility>
#include <cstdint>
#include <array>
#include <new>

namespace how
{
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
	struct union_of;

	template <>
	struct union_of<>
	{
	};

	template <class Head, class ...Tail>
	struct union_of<Head, Tail...>
	{
		union
		{
			Head head;
			union_of<Tail...> tail;
		};

		union_of()
		{
		}

		~union_of()
		{
		}
	};

	struct invalid_variant_exception : std::logic_error
	{
	};

	template <class ...T>
	struct variant
	{
		~variant() noexcept
		{
			destroy_value();
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

		//TODO
		variant &operator = (variant &&other) = delete;

		variant &operator = (variant const &other)
		{
			if (which() == other.which())
			{
				static std::array<void(*)(variant &, variant const &), sizeof...(T)> const assign =
				{{
					&copy_assign<T>...
				}};
				assign[which()](*this, other);
			}
			else
			{
				destroy_value();
				try
				{
					m_which = other.m_which;
					copy_construct_value(other);
				}
				catch (...)
				{
					m_which = invalid_which;
					throw;
				}
			}
			return *this;
		}

		variant(variant const &other)
			: m_which(other.m_which)
		{
			copy_construct_value(other);
		}

		template <class U, class = typename std::enable_if<any<std::is_same<typename std::decay<U>::type, T>::value...>::value, void>::type>
		variant(U &&value)
		{
			typedef typename std::decay<U>::type clean_element_type;
			clean_element_type &element = get<clean_element_type>();
			new (static_cast<void *>(&element)) clean_element_type(std::forward<U>(value));
			m_which = find<clean_element_type, T...>::value;
		}

		std::size_t which() const noexcept
		{
			return m_which;
		}

		template <class Visitor>
		auto apply_visitor(Visitor &&visitor)
		{
			typedef typename std::decay<Visitor>::type clean_visitor;
			typedef typename clean_visitor::result_type result_type;
			static std::array<result_type(*)(variant &, Visitor &), sizeof...(T)> const methods =
			{{
				&apply_visitor_method<result_type, T, Visitor>...
			}};
			return methods[which()](*this, visitor);
		}

		template <class Visitor>
		auto apply_visitor(Visitor &&visitor) const
		{
			typedef typename std::decay<Visitor>::type clean_visitor;
			typedef typename clean_visitor::result_type result_type;
			static std::array<result_type(*)(variant const &, Visitor &), sizeof...(T)> const methods =
			{{
				&apply_visitor_method_const<result_type, T, Visitor>...
			}};
			return methods[which()](*this, visitor);
		}

	private:

		static constexpr std::uint8_t invalid_which = 255;

		union_of<T...> m_value_storage;
		std::uint8_t m_which;

		void require_valid()
		{
			if (m_which != invalid_which)
			{
				return;
			}
			throw invalid_variant_exception();
		}

		template <class U>
		U &get()
		{
			return reinterpret_cast<U &>(m_value_storage);
		}

		template <class U>
		U const &get() const
		{
			return reinterpret_cast<U const &>(m_value_storage);
		}

		void destroy_value()
		{
			static std::array<void(*)(variant &), sizeof...(T)> const destructors =
			{{
				&destroy<T>...
			}};
			destructors[which()](*this);
		}

		void copy_construct_value(variant const &from)
		{
			static std::array<void(*)(variant &, variant const &), sizeof...(T)> const copy_constructors =
			{{
				&copy_construct<T>...
			}};
			copy_constructors[which()](*this, from);
		}

		template <class U>
		static void destroy(variant &this_) noexcept
		{
			this_.get<U>().~U();
		}

		template <class U>
		static void copy_construct(variant &to, variant const &from)
		{
			new (static_cast<void *>(&to.get<U>())) U(from.get<U>());
		}

		template <class U>
		static void move_construct(variant &to, variant &&from)
		{
			new (static_cast<void *>(&to.get<U>())) U(std::move(from.get<U>()));
		}

		template <class U>
		static void copy_assign(variant &to, variant const &from)
		{
			to.get<U>() = from.get<U>();
		}

		template <class Result, class U, class Visitor>
		static Result apply_visitor_method(variant &this_, Visitor &visitor)
		{
			return std::forward<Visitor>(visitor)(this_.get<U>());
		}

		template <class Result, class U, class Visitor>
		static Result apply_visitor_method_const(variant const &this_, Visitor &visitor)
		{
			return std::forward<Visitor>(visitor)(this_.get<U>());
		}
	};
}

#endif
