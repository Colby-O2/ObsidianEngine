#ifndef __OBSIDIANENGINE_MATH_VECTORN_HPP__
#define __OBSIDIANENGINE_MATH_VECTORN_HPP__

#include "ObsidianEngine/Math/MathUtils.hpp"

#include "ObsidianEngine/Math/Vector/VectorBase.hpp"

namespace ObsidianEngine::detail
{
	template<typename T, size_t N>
	struct Vector : VectorBase<Vector<T, N>, T, N>
	{
		std::array<T, N> data;

		constexpr Vector() noexcept : data{} {}

		constexpr Vector(const Vector&) noexcept = default;

		template<typename U, size_t M>
		requires (!std::is_same_v<U, T> || M != N)
		constexpr explicit Vector(const Vector<U, M>& other) noexcept : data{}
		{
			constexpr size_t copyCount = (N < M) ? N : M;

			for (size_t i = 0; i < copyCount; i++)
			{
				data[i] = static_cast<T>(other[i]);
			}

			for (size_t i = copyCount; i < N; i++)
			{
				data[i] = static_cast<T>(0);
			}
		}

		template<typename... Args>
		requires (sizeof...(Args) == N) && (std::is_convertible_v<Args, T> && ...)
		constexpr Vector(Args... args) noexcept : data{ static_cast<T>(args)... } {}

		constexpr Vector(const T(&arr)[N]) noexcept
		{
			for (size_t i = 0; i < N; i++)
			{
				data[i] = arr[i];
			}
		}

		constexpr Vector(std::initializer_list<T> init) noexcept
		{
			size_t i = 0;
			for (T v : init)
			{
				if (i < N)
				{
					data[i++] = v;
				}
			}

			for (; i < N; i++)
			{
				data[i] = T{};
			}
		}

		constexpr Vector& operator=(const Vector& other) noexcept = default;
	};

	template<typename T>
	struct Vector<T, 1> : VectorBase<Vector<T, 1>, T, 1>
	{
		union
		{
			struct { T x; };
			struct { T r; };
			std::array<T, 1> data;;
		};

		constexpr Vector() noexcept : data{} {}

		constexpr Vector(const Vector&) noexcept = default;

		constexpr Vector(T x_) noexcept : data{ x_ } {}

		template<typename U, size_t M>
		requires (!std::is_same_v<U, T> || M != 1)
		constexpr explicit Vector(const Vector<U, M>& other) noexcept : data{ }
		{
			if constexpr (M >= 1) data[0] = static_cast<T>(other[0]);
		}

		constexpr Vector& operator=(const Vector& other) noexcept = default;
	};
}

#endif