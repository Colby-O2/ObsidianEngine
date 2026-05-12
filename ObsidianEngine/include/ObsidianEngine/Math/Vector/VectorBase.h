#ifndef __OBSIDIANENGINE_MATH_VECTOR_BASE_H__
#define __OBSIDIANENGINE_MATH_VECTOR_BASE_H__

#include <array>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <type_traits>
#include <utility>

#include "ObsidianEngine/Math/MathUtils.h"
#include "ObsidianEngine/Math/Vector/Swizzle.h"
#include "ObsidianEngine/StringLiteral.h"

namespace ObsidianEngine::detail
{
	template<size_t Rows, size_t Cols, typename T>
	struct Matrix;

	template<typename Derived, typename T, size_t N>
	struct VectorBase
	{
		static_assert(std::is_arithmetic_v<T>);

		using ValueType = T;
		using FloatingPointT = std::common_type_t<T, float>;
		using VectorInt = Vector<int, N>;

		static constexpr size_t Size = N;

		Derived& self()
		{
			return static_cast<Derived&>(*this);
		}

		const Derived& self() const
		{
			return static_cast<const Derived&>(*this);
		}

		static constexpr Derived zero()
		{
			Derived result{};
			for (size_t i = 0; i < N; ++i)
			{
				result[i] = static_cast<T>(0);
			}
			return result;
		}

		static constexpr Derived one()
		{
			Derived result{};
			for (size_t i = 0; i < N; ++i)
			{
				result[i] = static_cast<T>(1);
			}
			return result;
		}

		template<StringLiteral Str>
		auto swizzle()
		{
			constexpr size_t len = sizeof(Str.value) - 1;
			static_assert(
				[]<size_t... I>(std::index_sequence<I...>)
			{
				return ((swizzleIndex(Str.value[I]) < N) && ...);
			}
			(std::make_index_sequence<len>{}),
				"Swizzle index out of range!"
				);
			return makeSwizzle<Str>(std::make_index_sequence<len>{});
		}

		template<StringLiteral Str, typename... Ts>
		Derived set(Ts... vals) const
		{
			constexpr size_t len = sizeof(Str.value) - 1;
			static_assert(len == sizeof...(Ts), "Argument count must match swizzle length!");
			static_assert(N <= 4, "Not implemented for vectors where N > 4!");
			static_assert(
				[]<size_t... I>(std::index_sequence<I...>)
			{
				return ((swizzleIndex(Str.value[I]) < N) && ...);
			}
			(std::make_index_sequence<len>{}),
				"Can't set an index out of range!"
				);
			static_assert(
				[]<size_t... Indices>(std::index_sequence<Indices...>)
			{
				return !HasDuplicates<
					swizzleIndex(Str.value[Indices])...
				>::value;
			}
			(std::make_index_sequence<len>{}),
				"Cannot set duplicate components!"
				);

			Derived result{ self() };
			size_t i = 0;
			((result[swizzleIndex(Str.value[i++])] = static_cast<T>(vals)), ...);
			return result;
		}

		T& operator[](size_t i)
		{
			assert(i < N && "Vector index out of range!");
			return self().data[i];
		}

		const T& operator[](size_t i) const
		{
			assert(i < N && "Vector index out of range!");
			return self().data[i];
		}

		Derived operator+(const Derived& rhs) const
		{
			Derived result{};
			for (size_t i = 0; i < N; i++)
			{
				result[i] = self()[i] + rhs[i];
			}
			return result;
		}

		Derived operator-(const Derived& rhs) const
		{
			Derived result{};
			for (size_t i = 0; i < N; i++)
			{
				result[i] = self()[i] - rhs[i];
			}
			return result;
		}

		Derived operator*(T scalar) const
		{
			Derived result{};
			for (size_t i = 0; i < N; i++)
			{
				result[i] = self()[i] * scalar;
			}
			return result;
		}

		Derived& operator*=(const Derived& rhs)
		{
			for (size_t i = 0; i < N; i++)
			{
				self()[i] *= rhs[i];
			}
			return self();
		}

		Derived operator*(const Derived& rhs) const
		{
			Derived result{};
			for (size_t i = 0; i < N; i++)
			{
				result[i] = self()[i] * rhs[i];
			}
			return result;
		}

		Derived operator/(T scalar) const
		{
			if constexpr (std::is_floating_point_v<T>)
			{
				assert(Math<T>::abs(scalar) > Math<T>::Epsilon &&
					"Division by zero!");
			}
			else
			{
				assert(scalar != static_cast<T>(0) &&
					"Division by zero!");
			}

			Derived result{};
			for (size_t i = 0; i < N; i++)
			{
				result[i] = self()[i] / scalar;
			}
			return result;
		}

		Derived operator-() const
		{
			Derived result{};
			for (size_t i = 0; i < N; i++)
			{
				result[i] = -self()[i];
			}
			return result;
		}

		Derived& operator+=(const Derived& rhs)
		{
			for (size_t i = 0; i < N; i++)
			{
				self()[i] += rhs[i];
			}
			return self();
		}

		Derived& operator-=(const Derived& rhs)
		{
			for (size_t i = 0; i < N; i++)
			{
				self()[i] -= rhs[i];
			}
			return self();
		}

		Derived& operator*=(T scalar)
		{
			for (size_t i = 0; i < N; i++)
			{
				self()[i] *= scalar;
			}
			return self();
		}

		Derived& operator/=(T scalar)
		{
			if constexpr (std::is_floating_point_v<T>)
			{
				assert(Math<T>::abs(scalar) > Math<T>::Epsilon &&
					"Division by zero!");
			}
			else
			{
				assert(scalar != static_cast<T>(0) &&
					"Division by zero!");
			}

			for (size_t i = 0; i < N; i++)
			{
				self()[i] /= scalar;
			}
			return self();
		}

		template<typename R>
		friend auto operator*(R scalar, const Derived& v) -> std::enable_if_t<std::is_arithmetic_v<R>, Derived>
		{
			static_assert(std::is_arithmetic_v<R>);
			return v * static_cast<T>(scalar);
		}

		friend std::ostream& operator<<(std::ostream& os, const Derived& v)
		{
			os << "(";

			for (size_t i = 0; i < N; i++)
			{
				os << v.self()[i];

				if (i != N - 1)
				{
					os << ", ";
				}
			}

			os << ")";

			return os;
		}

		bool equals(const Derived& rhs) const
		{
			for (size_t i = 0; i < N; i++)
			{
				if constexpr (std::is_floating_point_v<T>)
				{
					if (Math<T>::abs(self()[i] - rhs[i]) > Math<T>::Epsilon)
						return false;
				}
				else
				{
					if (self()[i] != rhs[i])
						return false;
				}
			}

			return true;
		}

		bool operator==(const Derived& rhs) const
		{
			return equals(rhs);
		}

		bool operator!=(const Derived& rhs) const
		{
			return !equals(rhs);
		}

		detail::Matrix<N, 1, T> asColumnMatrix() const
		{
			detail::Matrix<N, 1, T> m;
			m.setColumn(0, self());
			return m;
		}

		detail::Matrix<1, N, T> asRowMatrix() const
		{
			detail::Matrix<1, N, T> m;
			m.setRow(0, self());
			return m;
		}

		T dot(const Derived& rhs) const
		{
			return Derived::dot(self(), rhs);
		}

		Matrix<N, N, T> outer(const Derived& rhs) const
		{
			return Derived::outer(self(), rhs);
		}

		T sqrMagnitude() const
		{
			return Derived::dot(self(), self());
		}

		FloatingPointT magnitude() const
		{
			return Math<FloatingPointT>::sqrt(sqrMagnitude());
		}

		auto normalized() const
		{
			using FPT = std::common_type_t<T, float>;
			using ReturnVec = Vector<FPT, N>;

			FPT magSq = Math<FPT>::val(sqrMagnitude());
			if (magSq <= Math<FPT>::Epsilon) return ReturnVec::zero();

			FPT invMag = Math<FPT>::val(1.0f) / Math<FPT>::sqrt(magSq);

			ReturnVec result;
			for (size_t i = 0; i < N; ++i)
			{
				result[i] = Math<FPT>::val(self()[i]) * invMag;
			}

			return result;
		}

		void normalize() requires std::is_floating_point_v<T>
		{
			T magSq = sqrMagnitude();

			if (magSq <= Math<T>::Epsilon)
			{
				self() = Derived::zero();
				return;
			}

			self() /= Math<T>::sqrt(magSq);
		}

		static T dot(const Derived& a, const Derived& b)
		{
			T dotProduct = 0;
			for (size_t i = 0; i < N; i++)
			{
				dotProduct += a[i] * b[i];
			}
			return dotProduct;
		}

		static Matrix<N, N, T> outer(const Derived& a, const Derived& b)
		{
			return a.asColumnMatrix() * b.asRowMatrix();
		}

		static T sqrDistance(const Derived& a, const Derived& b)
		{
			T result = Math<T>::val(0);
			for (size_t i = 0; i < N; i++)
			{
				T d = b[i] - a[i];
				result += d * d;
			}
			return result;
		}

		static FloatingPointT distance(const Derived& a, const Derived& b)
		{
			return Math<FloatingPointT>::sqrt(sqrDistance(a, b));
		}

		Derived& floor() requires std::is_floating_point_v<T>
		{
			for (size_t i = 0; i < N; i++) self()[i] = Math<T>::floor(self()[i]);
			return self();
		}

		static Derived floor(const Derived& v) requires std::is_floating_point_v<T>
		{
			Derived result{};
			for (size_t i = 0; i < N; i++) result[i] = Math<T>::floor(v[i]);
			return result;
		}

		Derived& ceil() requires std::is_floating_point_v<T>
		{
			for (size_t i = 0; i < N; i++) self()[i] = Math<T>::ceil(self()[i]);
			return self();
		}

		static Derived ceil(const Derived& v) requires std::is_floating_point_v<T>
		{
			Derived result{};
			for (size_t i = 0; i < N; i++) result[i] = Math<T>::ceil(v[i]);
			return result;
		}

		Derived& round() requires std::is_floating_point_v<T>
		{
			for (size_t i = 0; i < N; i++) self()[i] = Math<T>::round(self()[i]);
			return self();
		}

		static Derived round(const Derived& v) requires std::is_floating_point_v<T>
		{
			Derived result{};
			for (size_t i = 0; i < N; i++) result[i] = Math<T>::round(v[i]);
			return result;
		}

		static VectorInt floorToInt(const Derived& v) requires std::is_floating_point_v<T>
		{
			VectorInt result;
			for (size_t i = 0; i < N; i++)
			{
				result[i] = Math<T>::floorToInt(v[i]);
			}
			return result;
		}

		VectorInt floorToInt() const requires std::is_floating_point_v<T> 
		{
			VectorInt result;
			for (size_t i = 0; i < N; i++) 
			{
				result[i] = Math<T>::floorToInt(self()[i]);
			}
			return result;
		}

		static VectorInt ceilToInt(const Derived& v) requires std::is_floating_point_v<T>
		{
			VectorInt result;
			for (size_t i = 0; i < N; i++)
			{
				result[i] = Math<T>::ceilToInt(v[i]);
			}
			return result;
		}

		VectorInt ceilToInt() const requires std::is_floating_point_v<T>
		{
			VectorInt result;
			for (size_t i = 0; i < N; i++)
			{
				result[i] = Math<T>::ceilToInt(self()[i]);
			}
			return result;
		}

		static VectorInt roundToInt(const Derived& v) requires std::is_floating_point_v<T>
		{
			VectorInt result;
			for (size_t i = 0; i < N; i++)
			{
				result[i] = Math<T>::roundToInt(v[i]);
			}
			return result;
		}

		VectorInt roundToInt() const requires std::is_floating_point_v<T>
		{
			VectorInt result;
			for (size_t i = 0; i < N; i++)
			{
				result[i] = Math<T>::roundToInt(self()[i]);
			}
			return result;
		}

		static Derived min(const Derived& a, const Derived& b)
		{
			Derived result{};
			for (size_t i = 0; i < N; i++)
			{
				result[i] = std::min(a[i], b[i]);
			}
			return result;
		}

		static Derived max(const Derived& a, const Derived& b)
		{
			Derived result{};
			for (size_t i = 0; i < N; i++)
			{
				result[i] = std::max(a[i], b[i]);
			}
			return result;
		}

		static Derived lerpUnclamped(const Derived& a, const Derived& b, T t) requires std::is_floating_point_v<T>
		{
			return a + t * (b - a);
		}

		static Derived lerp(const Derived& a, const Derived& b, T t) requires std::is_floating_point_v<T>
		{
			return lerpUnclamped(a, b, std::clamp(t, static_cast<T>(0), static_cast<T>(1)));
		}

		static Derived nlerpUnclamped(const Derived& a, const Derived& b, T t) requires std::is_floating_point_v<T>
		{
			Derived v = lerpUnclamped(a, b, t);
			if (v.sqrMagnitude() < static_cast<T>(1e-7)) return Derived::zero();
			return v.normalized();
		}

		static Derived nlerp(const Derived& a, const Derived& b, T t) requires std::is_floating_point_v<T>
		{
			return nlerpUnclamped(a, b, std::clamp(t, static_cast<T>(0), static_cast<T>(1)));
		}

		static Derived slerpUnclamped(const Derived& a, const Derived& b, T t) requires std::is_floating_point_v<T>
		{
			Derived v1 = a.normalized();
			Derived v2 = b.normalized();

			T dot = v1.dot(v2);

			dot = std::clamp(dot, static_cast<T>(-1), static_cast<T>(1));

			if (Math<T>::abs(dot) > Math<T>::AlmostOne)
			{
				return nlerpUnclamped(a, b, t);
			}

			T theta_0 = Math<T>::acos(dot);
			T theta = theta_0 * t;
			T sin_theta = Math<T>::sin(theta);
			T sin_theta_0 = Math<T>::sin(theta_0);

			T s0 = Math<T>::cos(theta) - dot * sin_theta / sin_theta_0;
			T s1 = sin_theta / sin_theta_0;

			T magA = a.magnitude();
			T magB = b.magnitude();
			T finalMag = magA + (magB - magA) * t;

			return ((v1 * s0) + (v2 * s1)) * finalMag;
		}

		static Derived slerp(const Derived& a, const Derived& b, T t) requires std::is_floating_point_v<T>
		{
			return slerpUnclamped(a, b, std::clamp(t, static_cast<T>(0), static_cast<T>(1)));
		}

		static Derived clamp(const Derived& v, const Derived& min, const Derived& max)
		{
			Derived result{};
			for (size_t i = 0; i < N; i++)
			{
				result[i] = std::clamp(v[i], min[i], max[i]);
			}
			return result;
		}

		static Derived clamp(const Derived& v, T min, T max)
		{
			Derived result{};
			for (size_t i = 0; i < N; i++)
			{
				result[i] = std::clamp(v[i], min, max);
			}
			return result;
		}

		static Derived scale(const Derived& a, const Derived& b)
		{
			Derived result{};
			for (size_t i = 0; i < N; i++)
			{
				result[i] = a[i] * b[i];
			}
			return result;
		}

		static Derived scale(const Derived& a, T scale)
		{
			Derived result{};
			for (size_t i = 0; i < N; i++)
			{
				result[i] = a[i] * scale;
			}
			return result;
		}

	private:
		template<StringLiteral Str, size_t... I>
		auto makeSwizzle(std::index_sequence<I...>)
		{
			return SwizzleProxy<
				T,
				swizzleIndex(Str.value[I])...
			>{ self().data };
		}
	};
}

#endif