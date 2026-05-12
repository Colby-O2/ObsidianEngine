#ifndef __OBSIDIANENGINE_MATH_VECTOR3_H__
#define __OBSIDIANENGINE_MATH_VECTOR3_H__

#include "ObsidianEngine/Math/MathUtils.h"
#include "ObsidianEngine/Math/Vector/VectorBase.h"

namespace ObsidianEngine::detail
{
	template<typename T>
	struct Vector<T, 3> : VectorBase<Vector<T, 3>, T, 3>
	{
		union
		{
			struct { T x, y, z; };
			struct { T r, g, b; };
			T data[3];
		};

		constexpr Vector() : data{ 0,0,0 } {}

		constexpr Vector(const Vector& v) : data{ v[0], v[1], v[2] } {}

		constexpr Vector(T x_, T y_, T z_) : data{ x_, y_, z_ } {}

		template<typename U>
		constexpr Vector(const Vector<U, 3>& other) : data{ static_cast<T>(other[0]), static_cast<T>(other[1]), static_cast<T>(other[2]) } {}

		template<typename U, size_t M>
		constexpr Vector(const Vector<U, M>& other) : data{}
		{
			if constexpr (M >= 1) data[0] = static_cast<T>(other[0]);
			if constexpr (M >= 2) data[1] = static_cast<T>(other[1]);
			if constexpr (M >= 3) data[2] = static_cast<T>(other[2]);
			if constexpr (M >= 4) data[3] = static_cast<T>(other[3]);
		}

		static constexpr Vector up() { return { Math<T>::val(0), Math<T>::val(1), Math<T>::val(0) }; }
		static constexpr Vector down() { return { Math<T>::val(0), Math<T>::val(-1), Math<T>::val(0) }; }
		static constexpr Vector left() { return { Math<T>::val(-1), Math<T>::val(0), Math<T>::val(0) }; }
		static constexpr Vector right() { return { Math<T>::val(1), Math<T>::val(0), Math<T>::val(0) }; }
		static constexpr Vector forward() { return { Math<T>::val(0), Math<T>::val(0), Math<T>::val(1) }; }
		static constexpr Vector back() { return { Math<T>::val(0), Math<T>::val(0), Math<T>::val(-1) }; }

		Vector<T, 3> cross(const  Vector<T, 3>& rhs) const
		{
			return Vector<T, 3>::cross(*this, rhs);
		}

		static Vector<T, 3> cross(const  Vector<T, 3>& a, const  Vector<T, 3>& b)
		{
			return
			{
				a.y * b.z - a.z * b.y,
				a.z * b.x - a.x * b.z,
				a.x * b.y - a.y * b.x
			};
		}
	};
}

#endif