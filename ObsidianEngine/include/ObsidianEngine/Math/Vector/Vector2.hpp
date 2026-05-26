#ifndef __OBSIDIANENGINE_MATH_VECTOR2_HPP__
#define __OBSIDIANENGINE_MATH_VECTOR2_HPP__

#include "ObsidianEngine/Math/MathUtils.hpp"
#include "ObsidianEngine/Math/Vector/VectorBase.hpp"

namespace ObsidianEngine::detail
{
	template<typename T>
	struct Vector<T, 2> : VectorBase<Vector<T, 2>, T, 2>
	{
		union
		{
			struct { T x, y; };
			struct { T r, g; };
			std::array<T, 2> data;
		};
		
		constexpr Vector() noexcept : data{} {}

		constexpr Vector(const Vector&) noexcept = default;

		constexpr Vector(T x_, T y_) noexcept : data{ x_, y_ } {}

		template<typename U, size_t M>
		requires (!std::is_same_v<U, T> || M != 2)
		constexpr explicit Vector(const Vector<U, M>& other) noexcept : data{}
		{
			if constexpr (M >= 1) data[0] = static_cast<T>(other[0]);
			if constexpr (M >= 2) data[1] = static_cast<T>(other[1]);
		}

		static constexpr Vector up() noexcept { return { Math<T>::val(0), Math<T>::val(1) }; }
		static constexpr Vector down() noexcept { return { Math<T>::val(0), Math<T>::val(-1) }; }
		static constexpr Vector left() noexcept { return { Math<T>::val(-1), Math<T>::val(0) }; }
		static constexpr Vector right() noexcept { return { Math<T>::val(1), Math<T>::val(0) }; }

		constexpr Vector& operator=(const Vector& other) noexcept = default;
	};
}

#endif