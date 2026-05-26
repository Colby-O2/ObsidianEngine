#ifndef __OBSIDIANENGINE_MATH_VECTOR_TYPES_HPP__
#define __OBSIDIANENGINE_MATH_VECTOR_TYPES_HPP__

#include "ObsidianEngine/Math/Vector/Vector2.hpp"
#include "ObsidianEngine/Math/Vector/Vector3.hpp"
#include "ObsidianEngine/Math/Vector/Vector4.hpp"
#include "ObsidianEngine/Math/Vector/VectorN.hpp"

namespace ObsidianEngine
{
	using Vector2Float = detail::Vector<float, 2>;
	using Vector3Float = detail::Vector<float, 3>;
	using Vector4Float = detail::Vector<float, 4>;

	using Vector2Double = detail::Vector<double, 2>;
	using Vector3Double = detail::Vector<double, 3>;
	using Vector4Double = detail::Vector<double, 4>;

	using Vector2Int = detail::Vector<int, 2>;
	using Vector3Int = detail::Vector<int, 3>;
	using Vector4Int = detail::Vector<int, 4>;

	using Vector2 = Vector2Float;
	using Vector3 = Vector3Float;
	using Vector4 = Vector4Float;
}

#endif