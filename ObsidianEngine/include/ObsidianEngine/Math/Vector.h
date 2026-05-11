#ifndef __OBSIDIANENGINE_MATH_VECTOR_H__
#define __OBSIDIANENGINE_MATH_VECTOR_H__

#include <array>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <type_traits>
#include <utility>
#include <cmath>
#include <algorithm>

#include "ObsidianEngine/Math/MathUtils.h"
#include "ObsidianEngine/Math/Swizzle.h"
#include "ObsidianEngine/StringLiteral.h"

namespace ObsidianEngine
{
	namespace detail
	{
		template<size_t Rows, size_t Cols, typename T>
		struct Matrix;

		template<typename T, size_t N>
		struct Vector;
	}

	using Vector2 = detail::Vector<float, 2>;
	using Vector3 = detail::Vector<float, 3>;
	using Vector4 = detail::Vector<float, 4>;

	using Vector2d = detail::Vector<double, 2>;
	using Vector3d = detail::Vector<double, 3>;
	using Vector4d = detail::Vector<double, 4>;

	using Vector2i = detail::Vector<int, 2>;
	using Vector3i = detail::Vector<int, 3>;
	using Vector4i = detail::Vector<int, 4>;

	namespace detail
	{
		template<typename Derived, typename T, size_t N>
		struct VectorBase
		{
			static_assert(std::is_arithmetic_v<T>);

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

#pragma region Arithmetic Operators
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
					assert(std::abs(scalar) > Math<T>::Epsilon &&
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
					assert(std::abs(scalar) > Math<T>::Epsilon &&
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
#pragma endregion Arithmetic Operators

#pragma region Comparison Operators
			bool equals(const Derived& rhs) const
			{
				for (size_t i = 0; i < N; i++)
				{
					if constexpr (std::is_floating_point_v<T>)
					{
						if (std::abs(self()[i] - rhs[i]) > Math<T>::Epsilon)
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
#pragma endregion Comparison Operators

#pragma region Matrix Conversion
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
#pragma endregion Matrix Conversion

#pragma region Math Memeber Functions
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

			T magnitude() const
			{
				return std::sqrt(sqrMagnitude());
			}

			Derived normalized() const
			{
				T magSq = sqrMagnitude();

				if (magSq <= Math<T>::Epsilon)
				{
					return Derived::zero();
				}

				return self() / std::sqrt(magSq);
			}

			void normalize()
			{
				T magSq = sqrMagnitude();

				if (magSq <= Math<T>::Epsilon)
				{
					self() = Derived::zero();
					return;
				}

				self() /= std::sqrt(magSq);
			}
#pragma endregion Math Memeber Functions

#pragma region Math Static Functions
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

			static T distance(const Derived& a, const Derived& b)
			{
				return std::sqrt(sqrDistance(a, b));
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

			static Derived lerpUnclamped(const Derived& a, const Derived& b, T t)
			{
				return a + t * (b - a);
			}

			static Derived lerp(const Derived& a, const Derived& b, T t)
			{
				return lerpUnclamped(a, b, std::clamp(t, static_cast<T>(0), static_cast<T>(1)));
			}

			static Derived nlerpUnclamped(const Derived& a, const Derived& b, T t)
			{
				Derived v = lerpUnclamped(a, b, t);
				if (v.sqrMagnitude() < static_cast<T>(1e-7)) return Derived::zero();
				return v.normalized();
			}

			static Derived nlerp(const Derived& a, const Derived& b, T t)
			{
				return nlerpUnclamped(a, b, std::clamp(t, static_cast<T>(0), static_cast<T>(1)));
			}

			static Derived slerpUnclamped(const Derived& a, const Derived& b, T t)
			{
				Derived v1 = a.normalized();
				Derived v2 = b.normalized();

				T dot = v1.dot(v2);

				dot = std::clamp(dot, static_cast<T>(-1), static_cast<T>(1));

				if (std::abs(dot) > Math<T>::AlmostOne)
				{
					return nlerpUnclamped(a, b, t);
				}

				T theta_0 = std::acos(dot); 
				T theta = theta_0 * t;
				T sin_theta = std::sin(theta);
				T sin_theta_0 = std::sin(theta_0);

				T s0 = std::cos(theta) - dot * sin_theta / sin_theta_0;
				T s1 = sin_theta / sin_theta_0;

				T magA = a.magnitude();
				T magB = b.magnitude();
				T finalMag = magA + (magB - magA) * t;

				return ((v1 * s0) + (v2 * s1)) * finalMag;
			}

			static Derived slerp(const Derived& a, const Derived& b, T t)
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

			static Derived scale(const Derived& a, float scale)
			{
				Derived result{};
				for (size_t i = 0; i < N; i++)
				{
					result[i] = a[i] * scale;
				}
				return result;
			}

#pragma endregion Math Static Functions

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

		template<typename T, typename R, size_t N>
		auto operator*(T scalar, const Vector<R, N>& v) -> std::enable_if_t<std::is_arithmetic_v<T>, Vector<R, N>>
		{
			static_assert(std::is_arithmetic_v<T>);
			return v * static_cast<R>(scalar);
		}

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

			constexpr Vector(const Vector& v) : data{v[0]} {}

			constexpr Vector(T x_) : data{ x_ } {}
		};

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

			static constexpr Vector up() { return { Math<T>::val(0), Math<T>::val(1) }; }
			static constexpr Vector down() { return { Math<T>::val(0), Math<T>::val(-1) }; }
			static constexpr Vector left() { return { Math<T>::val(-1), Math<T>::val(0) }; }
			static constexpr Vector right() { return { Math<T>::val(1), Math<T>::val(0) }; }
		};

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

			static constexpr Vector identity() { return { Math<T>::val(0), Math<T>::val(0), Math<T>::val(0), Math<T>::val(1) }; }
		};

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

			constexpr Vector(const Math<T>::val(&arr)[N])
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
	}
}

#endif