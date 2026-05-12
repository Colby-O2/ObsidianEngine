#ifndef __OBSIDIANENGINE_MATH_VECTORN_H__
#define __OBSIDIANENGINE_MATH_VECTORN_H__

#include "ObsidianEngine/Math/MathUtils.h"

#include "ObsidianEngine/Math/Vector/VectorBase.h"

namespace ObsidianEngine::detail
{
	template<typename T, size_t N>
	struct Vector : VectorBase<Vector<T, N>, T, N>
	{
		T data[N];

		constexpr Vector() : data{}
		{
			for (size_t i = 0; i < N; i++)
			{
				data[i] = 0;
			}
		}

		constexpr Vector(const Vector& v) : data{}
		{
			for (size_t i = 0; i < N; i++)
			{
				data[i] = v[i];
			}
		}

		template<typename U>
		constexpr Vector(const Vector<U, N>& v) : data{}
		{
			for (size_t i = 0; i < N; i++)
			{
				data[i] = static_cast<T>(v[i]);
			}
		}

		template<typename U, size_t M>
		constexpr Vector(const Vector<U, M>& other) : data{}
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
		constexpr Vector(Args... args) : data{ static_cast<T>(args)... }
		{
			static_assert(sizeof...(Args) == N, "Number of arguments must match Vector size!");
		}

		constexpr Vector(const T(&arr)[N])
		{
			for (size_t i = 0; i < N; i++)
			{
				data[i] = arr[i];
			}
		}

		constexpr Vector(std::initializer_list<T> init)
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
	};

	template<typename T>
	struct Vector<T, 1> : VectorBase<Vector<T, 1>, T, 1>
	{
		union
		{
			struct { T x; };
			struct { T r; };
			T data[1];
		};

		constexpr Vector() : data{ 0 } {}

		constexpr Vector(const Vector& v) : data{ v[0] } {}

		constexpr Vector(T x_) : data{ x_ } {}

		template<typename U>
		constexpr Vector(const Vector<U, 1>& other) : data{ static_cast<T>(other[0]) } {}

		template<typename U, size_t M>
		constexpr Vector(const Vector<U, M>& other) : data{ }
		{
			if constexpr (M >= 1) data[0] = static_cast<T>(other[0]);
			if constexpr (M >= 2) data[1] = static_cast<T>(other[1]);
			if constexpr (M >= 3) data[2] = static_cast<T>(other[2]);
			if constexpr (M >= 4) data[3] = static_cast<T>(other[3]);
		}
	};

	template<typename T1, typename T2, size_t N>
	inline auto operator*(const Vector<T1, N>& lhs, const Vector<T2, N>& rhs) -> Vector<std::common_type_t<T1, T2>, N>
	{
		using ResultT = std::common_type_t<T1, T2>;
		Vector<ResultT, N> result;
		for (size_t i = 0; i < N; ++i) 
		{
			result[i] = static_cast<ResultT>(lhs[i]) * static_cast<ResultT>(rhs[i]);
		}
		return result;
	}

	template<typename T1, typename T2, size_t N>
	inline auto operator+(const Vector<T1, N>& lhs, const Vector<T2, N>& rhs) -> Vector<std::common_type_t<T1, T2>, N>
	{
		using ResultT = std::common_type_t<T1, T2>;
		Vector<ResultT, N> result;
		for (size_t i = 0; i < N; ++i) 
		{
			result[i] = static_cast<ResultT>(lhs[i]) + static_cast<ResultT>(rhs[i]);
		}
		return result;
	}

	template<typename T1, typename T2, size_t N>
	inline auto operator-(const Vector<T1, N>& lhs, const Vector<T2, N>& rhs) -> Vector<std::common_type_t<T1, T2>, N>
	{
		using ResultT = std::common_type_t<T1, T2>;
		Vector<ResultT, N> result;
		for (size_t i = 0; i < N; ++i) 
		{
			result[i] = static_cast<ResultT>(lhs[i]) - static_cast<ResultT>(rhs[i]);
		}
		return result;
	}
}

#endif