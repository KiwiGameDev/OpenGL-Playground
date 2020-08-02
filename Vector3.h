#pragma once

template <typename T>
struct Vector3
{
	T x;
	T y;
	T z;

	Vector3()
		: x(0), y(0), z(0) { }

	Vector3(T x, T y, T z)
		: x(x), y(y), z(z) { }

	Vector3& operator+=(const Vector3& add)
	{
		this->x += add.x;
		this->y += add.y;
		this->z += add.z;
		return *this;
	}
};

typedef Vector3<float> Vector3f;
typedef Vector3<unsigned int> Vector3u;
