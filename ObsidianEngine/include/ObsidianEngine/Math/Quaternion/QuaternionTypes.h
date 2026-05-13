#ifndef  __OBSIDIANENGINE_MATH_QUATERNION_TYPES_H__
#define  __OBSIDIANENGINE_MATH_QUATERNION_TYPES_H__

#include "ObsidianEngine/Math/Quaternion/Quaternion.h"

namespace ObsidianEngine
{
	using QuaternionFloat = detail::Quaternion<float>;
	using QuaternionDouble= detail::Quaternion<double>;
	using Quaternion = QuaternionFloat;
}

#endif