#ifndef  __OBSIDIANENGINE_MATH_QUATERNION_TYPES_HPP__
#define  __OBSIDIANENGINE_MATH_QUATERNION_TYPES_HPP__

#include "ObsidianEngine/Math/Quaternion/Quaternion.hpp"

namespace ObsidianEngine
{
	using QuaternionFloat = detail::Quaternion<float>;
	using QuaternionDouble= detail::Quaternion<double>;
	using Quaternion = QuaternionFloat;
}

#endif