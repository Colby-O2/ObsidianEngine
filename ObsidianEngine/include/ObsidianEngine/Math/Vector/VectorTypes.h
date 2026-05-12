#ifndef __OBSIDIANENGINE_MATH_VECTOR_TYPES_H__
#define __OBSIDIANENGINE_MATH_VECTOR_TYPES_H__

#include "ObsidianEngine/Math/Vector/Vector2.h"
#include "ObsidianEngine/Math/Vector/Vector3.h"
#include "ObsidianEngine/Math/Vector/Vector4.h"
#include "ObsidianEngine/Math/Vector/VectorN.h"

namespace ObsidianEngine
{
	using Vector2f = detail::Vector<float, 2>;
	using Vector3f = detail::Vector<float, 3>;
	using Vector4f = detail::Vector<float, 4>;

	using Vector2d = detail::Vector<double, 2>;
	using Vector3d = detail::Vector<double, 3>;
	using Vector4d = detail::Vector<double, 4>;

	using Vector2i = detail::Vector<int, 2>;
	using Vector3i = detail::Vector<int, 3>;
	using Vector4i = detail::Vector<int, 4>;

	using Vector2 = Vector2f;
	using Vector3 = Vector3f;
	using Vector4 = Vector4f;
}

#endif