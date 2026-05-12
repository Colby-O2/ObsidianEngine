#ifndef __OBSIDIANENGINE_MATH_VECTOR2_H__
#define __OBSIDIANENGINE_MATH_VECTOR2_H__

#include "ObsidianEngine/Math/MathUtils.h"
#include "ObsidianEngine/Math/Vector/VectorBase.h"

namespace ObsidianEngine::detail
{
	template<typename T>
	struct Vector<T, 2> : VectorBase<Vector<T, 2>, T, 2>
	{
		union
		{
			struct { T x, y; };
			struct { T r, g; };
			T data[2];
		};

		constexpr Vector() : data{ 0,0 } {}

		constexpr Vector(const Vector& v) : data{ v[0], v[1] } {}

		constexpr Vector(T x_, T y_) : data{ x_, y_ } {}

		template<typename U>
		constexpr Vector(const Vector<U, 2>& other) : data{ static_cast<T>(other[0]), static_cast<T>(other[1]) } {}

		template<typename U, size_t M>
		constexpr Vector(const Vector<U, M>& other) : data{}
		{
			if constexpr (M >= 1) data[0] = static_cast<T>(other[0]);
			if constexpr (M >= 2) data[1] = static_cast<T>(other[1]);
			if constexpr (M >= 3) data[2] = static_cast<T>(other[2]);
			if constexpr (M >= 4) data[3] = static_cast<T>(other[3]);
		}

		static constexpr Vector up() { return { Math<T>::val(0), Math<T>::val(1) }; }
		static constexpr Vector down() { return { Math<T>::val(0), Math<T>::val(-1) }; }
		static constexpr Vector left() { return { Math<T>::val(-1), Math<T>::val(0) }; }
		static constexpr Vector right() { return { Math<T>::val(1), Math<T>::val(0) }; }
	};
}

#endif