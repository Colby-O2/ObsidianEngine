#ifndef __OBSIDIANENGINE_MATH_MATRIX_TYPES_H__
#define __OBSIDIANENGINE_MATH_MATRIX_TYPES_H__

#include "ObsidianEngine/Math/Matrix/MatrixMxN.h"
#include "ObsidianEngine/Math/Matrix/Matrix4x4.h"

namespace ObsidianEngine
{
	// Matrix 4x4 
	using Matrix4x4f = detail::Matrix4x4<float>;
	using Matrix4x4d = detail::Matrix4x4<double>;
	using Matrix4x4i = detail::Matrix<4, 4, int>;
	using Matirx4x4 = Matrix4x4f;

	// Square Matrices
	using Matrix2x2f = detail::Matrix<2, 2, float>;
	using Matrix3x3f = detail::Matrix<3, 3, float>;

	using Matrix2x2d = detail::Matrix<2, 2, double>;
	using Matrix3x3d = detail::Matrix<3, 3, double>;

	using Matrix2x2i = detail::Matrix<2, 2, int>;
	using Matrix3x3i = detail::Matrix<3, 3, int>;

	using Matrix2x2 = Matrix2x2f;
	using Matrix3x3 = Matrix3x3f;

	// Rectangular Matrices
	using Matrix3x4f = detail::Matrix<3, 4, float>;
	using Matrix4x3f = detail::Matrix<4, 3, float>;
	using Matrix2x3f = detail::Matrix<2, 3, float>;

	using Matrix3x4d = detail::Matrix<3, 4, double>;
	using Matrix4x3d = detail::Matrix<4, 3, double>;
	using Matrix2x3d = detail::Matrix<2, 3, double>;

	using Matrix3x4i = detail::Matrix<3, 4, int>;
	using Matrix4x3i = detail::Matrix<4, 3, int>;
	using Matrix2x3i = detail::Matrix<2, 3, int>;

	using Matrix3x4 = Matrix3x4f;
	using Matrix4x3 = Matrix4x3f;
	using Matrix2x3 = Matrix2x3f;
}
#endif