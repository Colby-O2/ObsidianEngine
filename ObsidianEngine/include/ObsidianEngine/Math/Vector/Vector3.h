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
			std::array<T, 3> data;
		};

		constexpr Vector() noexcept : data{} {}

		constexpr Vector(const Vector&) noexcept = default;

		constexpr Vector(T x_, T y_, T z_) noexcept : data{ x_, y_, z_ } {}

		template<typename U, size_t M>
		requires (!std::is_same_v<U, T> || M != 3)
		constexpr explicit Vector(const Vector<U, M>& other) noexcept : data{}
		{
			if constexpr (M >= 1) data[0] = static_cast<T>(other[0]);
			if constexpr (M >= 2) data[1] = static_cast<T>(other[1]);
			if constexpr (M >= 3) data[2] = static_cast<T>(other[2]);
		}

		static constexpr Vector up() noexcept { return { Math<T>::val(0), Math<T>::val(1), Math<T>::val(0) }; }
		static constexpr Vector down() noexcept { return { Math<T>::val(0), Math<T>::val(-1), Math<T>::val(0) }; }
		static constexpr Vector left() noexcept { return { Math<T>::val(-1), Math<T>::val(0), Math<T>::val(0) }; }
		static constexpr Vector right() noexcept { return { Math<T>::val(1), Math<T>::val(0), Math<T>::val(0) }; }
		static constexpr Vector forward() noexcept { return { Math<T>::val(0), Math<T>::val(0), Math<T>::val(1) }; }
		static constexpr Vector back() noexcept { return { Math<T>::val(0), Math<T>::val(0), Math<T>::val(-1) }; }

		Vector<T, 3> cross(const Vector<T, 3>& rhs) const noexcept
		{
			return Vector<T, 3>::cross(*this, rhs);
		}

		static Vector<T, 3> cross(const Vector<T, 3>& a, const Vector<T, 3>& b) noexcept
		{
			return
			{
				a.y * b.z - a.z * b.y,
				a.z * b.x - a.x * b.z,
				a.x * b.y - a.y * b.x
			};
		}

		constexpr Vector& operator=(const Vector& other) noexcept = default;
	};
}

#endif