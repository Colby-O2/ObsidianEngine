#ifndef __OBSIDIANENGINE_MATH_MATRIX_TYPES_H__
#define __OBSIDIANENGINE_MATH_MATRIX_TYPES_H__

#include "ObsidianEngine/Math/Matrix/MatrixMxN.h"
#include "ObsidianEngine/Math/Matrix/Matrix4x4.h"

namespace ObsidianEngine
{
	// Matrix 4x4 
	using Matrix4x4Float = detail::Matrix4x4<float>;
	using Matrix4x4Double = detail::Matrix4x4<double>;
	using Matrix4x4Int = detail::Matrix<4, 4, int>;
	using Matrix4x4 = Matrix4x4Float;

	// Square Matrices
	using Matrix2x2Float = detail::Matrix<2, 2, float>;
	using Matrix3x3Float = detail::Matrix<3, 3, float>;

	using Matrix2x2Double = detail::Matrix<2, 2, double>;
	using Matrix3x3Double = detail::Matrix<3, 3, double>;

	using Matrix2x2Int = detail::Matrix<2, 2, int>;
	using Matrix3x3Int = detail::Matrix<3, 3, int>;

	using Matrix2x2 = Matrix2x2Float;
	using Matrix3x3 = Matrix3x3Float;

	// Rectangular Matrices
	using Matrix3x4Float = detail::Matrix<3, 4, float>;
	using Matrix4x3Float = detail::Matrix<4, 3, float>;
	using Matrix2x3Float = detail::Matrix<2, 3, float>;

	using Matrix3x4Double = detail::Matrix<3, 4, double>;
	using Matrix4x3Double = detail::Matrix<4, 3, double>;
	using Matrix2x3Double = detail::Matrix<2, 3, double>;

	using Matrix3x4Int = detail::Matrix<3, 4, int>;
	using Matrix4x3Int = detail::Matrix<4, 3, int>;
	using Matrix2x3Int = detail::Matrix<2, 3, int>;

	using Matrix3x4 = Matrix3x4Float;
	using Matrix4x3 = Matrix4x3Float;
	using Matrix2x3 = Matrix2x3Float;
}
#endif