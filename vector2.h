#pragma once

template <typename T>
struct Vector2
{
	T x;
	T y;

	Vector2()
		: x(0), y(0) { }

	Vector2(T x, T y)
		: x(x), y(y) { }

	Vector2& operator+=(const Vector2& add)
	{
		this->x += add.x;
		this->y += add.y;
		return *this;
	}
};

typedef Vector2<float> Vector2f;
typedef Vector2<unsigned int> Vector2u;
