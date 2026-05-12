#ifndef  __OBSIDIANENGINE_MATH_QUATERNION_TYPES_H__
#define  __OBSIDIANENGINE_MATH_QUATERNION_TYPES_H__

#include "ObsidianEngine/Math/Quaternion/Quaternion.h"

namespace ObsidianEngine
{
	using Quaternionf = detail::Quaternion<float>;
	using Quaterniond = detail::Quaternion<double>;
	using Quaternion = Quaternionf;
}

#endif