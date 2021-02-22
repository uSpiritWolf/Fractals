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
			return vec2f(this->x + rhs.x, this->y + rhs.y);
		}

		vec2<type> operator-(const vec2<type>& rhs) const
		{
			return vec2f(this->x - rhs.x, this->y - rhs.y);
		}

		vec2<type> operator*(const vec2<type>& rhs) const
		{
			return vec2f(this->x * rhs.x, this->y * rhs.y);
		}

		vec2<type> operator*(const float rhs) const
		{
			return vec2f(this->x * rhs, this->y * rhs);
		}

		vec2<type>& operator+=(const vec2<type>& rhs)
		{
			this->x += rhs.x;
			this->y += rhs.y;
			return *this;
		}
	};

	using vec2f = vec2<float>;
	using vec2i = vec2<int>;
}