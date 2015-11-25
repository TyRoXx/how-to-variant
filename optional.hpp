#ifndef HOW_TO_OPTIONAL_HPP
#define HOW_TO_OPTIONAL_HPP

#include <utility>

namespace how
{
	struct in_place_t
	{
	};

	constexpr in_place_t in_place;

	struct none_t
	{
	};

	constexpr none_t none;

	template <class T>
	struct optional
	{
		optional() noexcept
			: m_is_set(false)
		{
		}

		~optional() noexcept
		{
			if (!m_is_set)
			{
				return;
			}
			m_value.~T();
		}

		optional(optional &&other) noexcept(noexcept(T(std::declval<T>())))
			: m_is_set(other.m_is_set)
		{
			if (!m_is_set)
			{
				return;
			}
			new (static_cast<void *>(&m_value)) T(std::move(other.m_value));
		}

		optional &operator = (optional &&other) noexcept(noexcept(std::declval<T &>() = std::declval<T>()) && noexcept(T(std::declval<T>())))
		{
			if (m_is_set)
			{
				if (other.m_is_set)
				{
					m_value = std::move(other.m_value);
				}
				else
				{
					*this = none;
				}
			}
			else
			{
				if (other.m_is_set)
				{
					new (static_cast<void *>(&m_value)) T(std::move(other.m_value));
					m_is_set = true;
				}
				else
				{
					//both empty already
				}
			}
			return *this;
		}

		optional(optional const &other) noexcept(noexcept(T(std::declval<T const &>())))
			: m_is_set(other.m_is_set)
		{
			if (!m_is_set)
			{
				return;
			}
			new (static_cast<void *>(&m_value)) T(other.m_value);
		}

		optional &operator = (optional const &other) noexcept(noexcept(std::declval<T &>() = std::declval<T const &>()) && noexcept(T(std::declval<T const &>())))
		{
			if (m_is_set)
			{
				if (other.m_is_set)
				{
					m_value = other.m_value;
				}
				else
				{
					*this = none;
				}
			}
			else
			{
				if (other.m_is_set)
				{
					new (static_cast<void *>(&m_value)) T(other.m_value);
					m_is_set = true;
				}
				else
				{
					//both empty already
				}
			}
			return *this;
		}

		optional(none_t) noexcept
			: m_is_set(false)
		{
		}

		optional &operator = (none_t) noexcept
		{
			if (!m_is_set)
			{
				return *this;
			}
			m_value.~T();
			m_is_set = false;
			return *this;
		}

		template <class ...Args>
		optional(in_place_t, Args &&...args) noexcept(noexcept(T{std::forward<Args>(args)...}))
			: m_is_set(true)
		{
			new (static_cast<void *>(&m_value)) T{std::forward<Args>(args)...};
		}

		template <class ...Args>
		void emplace(Args &&...args) noexcept(noexcept(T{std::forward<Args>(args)...}))
		{
			if (m_is_set)
			{
				m_value.~T();
				m_is_set = false;
			}
			new (static_cast<void *>(&m_value)) T{std::forward<Args>(args)...};
		}

		explicit operator bool() const noexcept
		{
			return m_is_set;
		}

		T &operator *() noexcept
		{
			return m_value;
		}

		T const &operator *() const noexcept
		{
			return m_value;
		}

		T *operator ->() noexcept
		{
			return &m_value;
		}

		T const *operator ->() const noexcept
		{
			return &m_value;
		}

	private:

		union
		{
			T m_value;
		};
		bool m_is_set;
	};
}

#endif
