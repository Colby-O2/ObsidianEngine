#ifndef VECTOR_H_
#define VECTOR_H_

#include <array>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <type_traits>
#include <utility>
#include <cmath>
#include <algorithm>

#include "../include/StringLiteral.h"

namespace ObsidianEngine
{
	namespace detail
	{
		template<typename T, size_t N>
		struct Vector;
	}

	using Vector2 = detail::Vector<float, 2>;
	using Vector3 = detail::Vector<float, 3>;
	using Vector4 = detail::Vector<float, 4>;

	using Vector2Int = detail::Vector<int, 2>;
	using Vector3Int = detail::Vector<int, 3>;
	using Vector4Int = detail::Vector<int, 4>;

	namespace detail
	{
		constexpr size_t swizzleIndex(char c)
		{
			switch (c)
			{
				case 'x': return 0;
				case 'y': return 1;
				case 'z': return 2;
				case 'w': return 3;

				case 'r': return 0;
				case 'g': return 1;
				case 'b': return 2;
				case 'a': return 3;
			}

			return static_cast<size_t>(c - '0');
		}

		template<size_t...>
		struct HasDuplicates : std::false_type {};

		template<size_t First, size_t... Rest>
		struct HasDuplicates<First, Rest...>
		{
			static constexpr bool value = ((First == Rest) || ...) || HasDuplicates<Rest...>::value;
		};

		template<typename T, size_t... Indices>
		struct SwizzleProxy
		{
			static constexpr size_t Size = sizeof...(Indices);

			T* data;

			operator Vector<T, Size>() const
			{
				return eval();
			}

			Vector<T, Size> eval() const
			{
				return Vector<T, Size>{ data[Indices]... };
			}

#pragma region Arithmetic Operators
			SwizzleProxy& operator=(const Vector<T, Size>& rhs)
			{
				static_assert(!HasDuplicates<Indices...>::value, "Cannot assign to swizzle with duplicate components!");

				size_t i = 0;
				((data[Indices] = rhs[i++]), ...);

				return *this;
			}

			T& operator[](size_t i)
			{
				assert(i < Size && "Vector index out of range!");
				constexpr size_t lookup[] = { Indices... };
				return data[lookup[i]];
			}

			const T& operator[](size_t i) const
			{
				assert(i < Size && "Vector index out of range!");
				constexpr size_t lookup[] = { Indices... };
				return data[lookup[i]];
			}
#pragma endregion Arithmetic Operators

			friend std::ostream& operator<<(std::ostream& os, const SwizzleProxy& v)
			{
				os << v.eval();
				return os;
			}
		};

		template<typename Derived, typename T, size_t N>
		struct VectorBase
		{
			static_assert(std::is_arithmetic_v<T>);

			Derived& self()
			{
				return static_cast<Derived&>(*this);
			}

			const Derived& self() const
			{
				return static_cast<const Derived&>(*this);
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

			Derived operator/(T scalar) const
			{
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
			bool equals(const Derived& rhs, T epsilon = static_cast<T>(0.00001)) const
			{
				for (size_t i = 0; i < N; i++)
				{
					if (std::abs(self()[i] - rhs[i]) > epsilon)
					{
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


#pragma region Math Memeber Functions
			T dot(const Derived& rhs) const
			{
				return Derived::dot(self(), rhs);
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
				Derived result{};
				result = self() / magnitude();
				return result;
			}

			void normalize()
			{
				self() /= magnitude();
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

			static T sqrDistance(const Derived& a, const Derived& b)
			{
				T result = T(0);
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

			static Derived lerp(const Derived& a, const Derived& b, T t)
			{
				float clampedT = std::clamp(t, T(0), T(1));
				return a + clampedT * (b - a);
			}

			static Derived lerpUnclamped(const Derived& a, const Derived& b, T t)
			{
				return a + t * (b - a);
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
		Vector<R, N> operator*(T scalar, const Vector<R, N>& v)
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

			Vector<T, 3> cross(const  Vector<T, 3>& rhs)
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
	}
}

#endif