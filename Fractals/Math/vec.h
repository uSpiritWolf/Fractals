#pragma once

namespace math
{
	template<typename T>
	struct vec2
	{
		typedef T type;

		union
		{
			T raw[2];

			struct
			{
				union
				{
					type x;
					type width;
				};

				union
				{
					T y;
					T height;
				};
			};
		};

		vec2() : x(0), y(0) {}

		vec2(type x, type y) : x(x), y(y) {}

		vec2<type> operator+(const vec2<type>& rhs) const
		{
			return vec2<type>(this->x + rhs.x, this->y + rhs.y);
		}

		vec2<type> operator-(const vec2<type>& rhs) const
		{
			return vec2<type>(this->x - rhs.x, this->y - rhs.y);
		}

		vec2<type> operator*(const vec2<type>& rhs) const
		{
			return vec2<type>(this->x * rhs.x, this->y * rhs.y);
		}

		vec2<type> operator*(const type rhs) const
		{
			return vec2<type>(this->x * rhs, this->y * rhs);
		}

		friend vec2<type> operator*(const type lhs, const vec2<type>& rhs)
		{
			return vec2<type>(lhs * rhs.x, lhs * rhs.y);
		}

		vec2<type> operator/(const type rhs) const
		{
			return vec2<type>(this->x / rhs, this->y / rhs);
		}

		friend vec2<type> operator/(const type lhs, const vec2<type>& rhs)
		{
			return vec2<type>(lhs / rhs.x, lhs / rhs.y);
		}

		vec2<type>& operator+=(const vec2<type>& rhs)
		{
			this->x += rhs.x;
			this->y += rhs.y;
			return *this;
		}

		bool operator==(const vec2<type>& rhs) const
		{
			return x == rhs.x && y == rhs.y;
		}
	};

	using vec2f = vec2<float>;
	using vec2i = vec2<int>;
	using vec2d = vec2<double>;

	inline vec2d toVec2d(const vec2f& value)
	{
		return vec2d(value.x, value.y);
	}

	inline vec2f toVec2f(const vec2d& value)
	{
		return vec2f(static_cast<float>(value.x), static_cast<float>(value.y));
	}

	template<class T>
	inline T dot(const vec2<T> a, const vec2<T> b)
	{
		return a.x * b.x + a.y * b.y;
	}
}