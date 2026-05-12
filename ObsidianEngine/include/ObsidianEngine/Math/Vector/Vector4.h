#ifndef __OBSIDIANENGINE_MATH_VECTOR4_H__
#define __OBSIDIANENGINE_MATH_VECTOR4_H__

#include "ObsidianEngine/Math/MathUtils.h"
#include "ObsidianEngine/Math/Vector/VectorBase.h"

namespace ObsidianEngine::detail
{
	template<typename T>
	struct Vector<T, 4> : VectorBase<Vector<T, 4>, T, 4>
	{
		union
		{
			struct { T x, y, z, w; };
			struct { T r, g, b, a; };
			T data[4];
		};

		constexpr Vector() : data{ 0,0,0,0 } {}

		constexpr Vector(const Vector& v) : data{ v[0], v[1], v[2], v[3] } {}

		constexpr Vector(T x_, T y_, T z_, T w_) : data{ x_, y_, z_, w_ } {}

		template<typename U>
		constexpr Vector(const Vector<U, 4>& other) : data{ static_cast<T>(other[0]), static_cast<T>(other[1]), static_cast<T>(other[2]), static_cast<T>(other[3]) } {}

		template<typename U, size_t M>
		constexpr Vector(const Vector<U, M>& other) : data{ }
		{
			if constexpr (M >= 1) data[0] = static_cast<T>(other[0]);
			if constexpr (M >= 2) data[1] = static_cast<T>(other[1]);
			if constexpr (M >= 3) data[2] = static_cast<T>(other[2]);
			if constexpr (M >= 4) data[3] = static_cast<T>(other[3]);
		}

		static constexpr Vector identity() { return { Math<T>::val(0), Math<T>::val(0), Math<T>::val(0), Math<T>::val(1) }; }
	};
}

#endif