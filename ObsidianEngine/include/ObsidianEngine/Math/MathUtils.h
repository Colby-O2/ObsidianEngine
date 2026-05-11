#ifndef __OBSIDIANENGINE_MATH_UTILS_H__
#define __OBSIDIANENGINE_MATH_UTILS_H__

#include <cmath>
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
			static constexpr T Epsilon = static_cast<T>(1e-7);
			static constexpr T AlmostOne = static_cast<T>(1.0) - Epsilon;
			static constexpr T AlmostNegativeOne = -AlmostOne;
			static constexpr T SqrEpsilon = Epsilon * Epsilon;

			static constexpr T Infinity = std::numeric_limits<T>::infinity();
			static constexpr T NegativeInfinity = -std::numeric_limits<T>::infinity();

			static constexpr T Pi = std::numbers::pi_v<T>;
			static constexpr T Tau = Pi * static_cast<T>(2);
			static constexpr T HalfPi = Pi * static_cast<T>(0.5);

			static constexpr T Deg2Rad = Pi / static_cast<T>(180);
			static constexpr T Rad2Deg = static_cast<T>(180) / Pi;

			template<typename U>
			static constexpr T val(U value) 
			{
				return static_cast<T>(value);
			}

			static T lerpUnclamped();
			static T lerp();
			static T slerpUnclamped();
			static T slerp();
		};
	}
}

#endif