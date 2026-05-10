#ifndef OBSIDIANENGINE_MATH_H_
#define OBSIDIANENGINE_MATH_H_

#include <cmath>
#include <numbers>

namespace ObsidianEngine
{
	namespace detail
	{
		template<typename T>
		class MathUtils;
	}

	using Mathf = detail::MathUtils<float>;
	using Mathd = detail::MathUtils<double>;
	using Mathi = detail::MathUtils<int>;

	namespace detail
	{
		template<typename T>
		class MathUtils
		{
			static T lerpUnclamped();
			static T lerp();
			static T slerpUnclamped();
			static T slerp();
		};
	}
}

#endif