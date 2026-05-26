#ifndef __OBSIDIANENGINE_MATH_UTILS_HPP__
#define __OBSIDIANENGINE_MATH_UTILS_HPP__

#include <cmath>
#include <algorithm>
#include <numbers>
#include <limits>

namespace ObsidianEngine
{
	namespace detail
	{
		template<typename T>
		class Math;
	}

	using Mathf = detail::Math<float>;
	using Mathd = detail::Math<double>;
	using Mathi = detail::Math<int>;

	namespace detail
	{
		template<typename T>
		class Math
		{
		public:
			static constexpr bool IsArithmetic = std::is_arithmetic_v<T>;
			static constexpr bool IsFloatingPoint =  std::is_floating_point_v<T>; 

			static constexpr T Epsilon = static_cast<T>(1e-7);
			static constexpr T AlmostOne = static_cast<T>(1.0) - Epsilon;
			static constexpr T AlmostNegativeOne = -AlmostOne;
			static constexpr T SqrEpsilon = Epsilon * Epsilon;

			static constexpr T Infinity = std::numeric_limits<T>::infinity();
			static constexpr T NegativeInfinity = -std::numeric_limits<T>::infinity();

			static constexpr T Pi = static_cast<T>(3.1415926535897932384626433832795028841971693993751058209749445923078164062);
			static constexpr T Tau = Pi * static_cast<T>(2);
			static constexpr T HalfPi = Pi * static_cast<T>(0.5);

			static constexpr T Deg2Rad = Pi / static_cast<T>(180);
			static constexpr T Rad2Deg = static_cast<T>(180) / Pi;

			template<typename U>
			static constexpr T val(U value) 
			{
				return static_cast<T>(value);
			}

			static bool isEqual(T a, T b, T epsilon = Math::Epsilon)
			{
				if constexpr (std::is_floating_point_v<T>)
				{
					return abs(a - b) < epsilon;
				}

				return a == b;
			}

			static T sin(T val) 
			{
				return std::sin(val);
			}

			static T cos(T val)
			{
				return std::cos(val);
			}

			static T tan(T val)
			{
				return std::tan(val);
			}

			static T asin(T val)
			{
				return std::asin(val);
			}

			static T acos(T val)
			{
				return std::acos(val);
			}

			static T atan(T val)
			{
				return std::atan(val);
			}

			static T atan2(T  y, T x)
			{
				return std::atan2(y, x);
			}

			static T abs(T val)
			{
				return std::abs(val);
			}

			static T sqrt(T val)
			{
				return std::sqrt(val);
			}

			static T max(T a, T b)
			{
				return std::max(a, b);
			}

			static T min(T a, T b)
			{
				return std::min(a, b);
			}

			static T clamp(T val, T min, T max)
			{
				return std::clamp(val, min, max);
			}

			static T floor(T val)
			{
				return std::floor(val);
			}

			static T ceil(T val)
			{
				return std::ceil(val);
			}

			static T round(T val)
			{
				return std::round(val);
			}

			static int floorToInt(T val) 
			{ 
				return Math<int>::val(std::floor(val));
			}

			static int ceilToInt(T val) 
			{ 
				return Math<int>::val(std::ceil(val));
			}

			static int roundToInt(T val) 
			{ 
				return Math<int>::val(std::round(val));
			}


			static T mod(T a, T b)
			{
				return std::fmod(a, b);
			}

			static T lerpUnclamped();
			static T lerp();
			static T slerpUnclamped();
			static T slerp();
		};
	}
}

#endif