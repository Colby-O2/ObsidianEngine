#ifndef MATRIX_H_
#define MATRIX_H_

#include "Vector.h"

#include <iostream>
#include <cmath>
#include <numbers>

namespace ObsidianEngine
{
	namespace detail
	{
		template<typename T>
		struct Matrix4x4;
	}

	using Matrix4x4 = detail::Matrix4x4<float>;

	namespace detail
	{
		float det3x3(float a00, float a01, float a02, float a10, float a11, float a12, float a20, float a21, float a22)
		{
			return a00 * (a11 * a22 - a12 * a21)
				- a01 * (a10 * a22 - a12 * a20)
				+ a02 * (a10 * a21 - a11 * a20);
		}

		template<typename T>
		struct Matrix4x4
		{
			static_assert(std::is_arithmetic_v<T>);

			union
			{
				struct
				{
					float m11, m21, m31, m41;
					float m12, m22, m32, m42;
					float m13, m23, m33, m43;
					float m14, m24, m34, m44;
				};

				Vector4 columns[4];
			};

			static Matrix4x4 identity()
			{
				return Matrix4x4{
					{1, 0, 0, 0},
					{0, 1, 0, 0},
					{0, 0, 1, 0},
					{0, 0, 0, 1},
				};
			}

			static Matrix4x4 zero()
			{
				return Matrix4x4{
					{0, 0, 0, 0},
					{0, 0, 0, 0},
					{0, 0, 0, 0},
					{0, 0, 0, 0},
				};
			}

			constexpr Matrix4x4() : columns{
					{1, 0, 0, 0},
					{0, 1, 0, 0},
					{0, 0, 1, 0},
					{0, 0, 0, 1},
			} {
			}

			constexpr Matrix4x4(const Vector4& c0, const Vector4& c1, const Vector4& c2, const Vector4& c3) : columns{ c0, c1, c2, c3 } {}

			constexpr Matrix4x4(const Matrix4x4& m)
			{
				columns[0] = m.columns[0];
				columns[1] = m.columns[1];
				columns[2] = m.columns[2];
				columns[3] = m.columns[3];
			}

			Vector4& operator[](size_t col)
			{
				assert(i < 4);
				return columns[col];
			}

			const Vector4& operator[](size_t col) const
			{
				assert(i < 4);
				return columns[col];
			}

			T& operator()(size_t row, size_t col)
			{
				assert(row < 4 && col < 4);
				return columns[col][row];
			}

			const T& operator()(size_t row, size_t col) const
			{
				assert(row < 4 && col < 4);
				return columns[col][row];
			}

			T determinat() const
			{
				return m11 * (m22 * m33 * m44 + m23 * m34 * m42 + m24 * m32 * m43 
					- m24 * m33 * m42 - m23 * m32 * m44 - m22 * m34 * m43)
					- m21 * (m12 * m33 * m44 + m13 * m34 * m42 + m14 * m32 * m43
						- m14 * m33 * m42 - m13 * m32 * m44 - m12 * m34 * m43)
					+ m31 * (m12 * m23 * m44 + m13 * m24 * m42 + m14 * m22 * m43 
						- m14 * m23 * m42 - m13 * m22 * m44 - m12 * m24 * m43)
					- m41 * (m12 * m23 * m34 + m13 * m24 * m32 + m14 * m22 * m33 
						- m14 * m23 * m32 - m13 * m22 * m34 - m12 * m24 * m33);
			}

			T minor(int row, int col) const
			{
				T sub[3][3];
				int r = 0;

				for (int i = 0; i < 4; i++)
				{
					if (i == row) continue;

					int c = 0;
					for (int j = 0; j < 4; j++)
					{
						if (j == col) continue;
						sub[r][c++] = (*this)(i, j);
					}
					r++;
				}

				return det3x3(sub[0][0], sub[0][1], sub[0][2], sub[1][0], sub[1][1], sub[1][2], sub[2][0], sub[2][1], sub[2][2]);
			}

			Matrix4x4 inverse() const
			{
				Matrix4x4 inv{};
				T det = determinat();

				if (std::abs(det) < T(1e-7))
				{
					return identity();
				}

				T invDet = T(1) / det;

				for (int row = 0; row < 4; row++)
				{
					for (int col = 0; col < 4; col++)
					{
						T cofactor = minor(row, col);

						if ((row + col) % 2 != 0)
						{
							cofactor = -cofactor;
						}

						inv(col, row) = cofactor * invDet;
					}
				}

				return inv;
			}


			Matrix4x4 transpose() const
			{
				Matrix4x4 transposed;

				for (int c = 0; c < 4; c++)
				{
					for (int r = 0; r < 4; r++)
					{
						transposed(r, c) = (*this)(c, r);
					}
				}

				return transposed;
			}


			bool isIdentity() const
			{
				return *this == identity();
			}

			Vector4 getColumn(size_t col) const
			{
				return columns[col];
			}

			Vector4 getRow(size_t row) const
			{
				return { columns[0][row], columns[1][row], columns[2][row], columns[3][row] };
			}

			void setColumn(size_t col, const Vector4& v)
			{
				columns[col] = v;
			}

			void setRow(size_t row, const Vector4& v)
			{
				for (int c = 0; c < 4; c++)
				{
					columns[c][row] = v[c];
				}
			}

			Vector3 lossyScale() const;
			Vector4 rotation() const;

			Vector3 multiplyPoint(const Vector3& p) const
			{
				Vector4 r = (*this) * Vector4{ p.x, p.y, p.z, 1 };
				return { r.x, r.y, r.z };
			}

			Vector3 multiplyDirection(const Vector3& v) const
			{
				Vector4 r = (*this) * Vector4{ v.x, v.y, v.z, 0 };
				return { r.x, r.y, r.z };
			}

			static Matrix4x4 rotateX(T r);
			static Matrix4x4 rotateY(T r);
			static Matrix4x4 rotateZ(T r);

			static Matrix4x4 rotate(const Vector3& eulerAngles)
			{
				return identity();
			}

			static Matrix4x4 translate(const Vector3& t)
			{
				Matrix4x4 m;
				m[3][0] = t.x;
				m[3][1] = t.y;
				m[3][2] = t.z;
				return m;

			}

			static Matrix4x4 scale(const Vector3& s)
			{
				Matrix4x4 m;
				m[0][0] = s.x;
				m[1][1] = s.y;
				m[2][2] = s.z;
				return m;
			}

			static Matrix4x4 trs(const Vector3& pos, const Vector3& rot, const Vector3& s)
			{
				return translate(pos) * rotate(rot) * scale(s);
			}

			static Matrix4x4 lookAt(const Vector3& eye, const Vector3& target, const Vector3& up)
			{
				Vector3 f = (target - eye).normalized();
				Vector3 s = Vector3::cross(f, up).normalized();
				Vector3 u = Vector3::cross(s, f).normalized();

				Matrix4x4 m {};
				m[0] = { s.x, u.x, -f.x, 0 };
				m[1] = { s.y, u.y, -f.y, 0 };
				m[2] = { s.z, u.z, -f.z, 0 };
				m[3] = { -Vector3::dot(s, eye), -Vector3::dot(u , eye), Vector3::dot(f, eye), 1};
				return m;
			}

			static Matrix4x4 perspective(T fov, T aspect, T nearZ, T farZ)
			{
				T fovDeg = fov * std::numbers::pi / 180.0;
				T f = 1.0 / std::tan(fovDeg / 2.0);

				Matrix4x4 m{};
				m[0][0] = f / aspect;
				m[1][1] = f;
				m[2][2] = -(farZ + nearZ) / (farZ - nearZ);
				m[2][3] = -1;
				m[3][2] = -2.0 * farZ * nearZ / (farZ - nearZ);
				m[3][3] = 0;
				return m;
			}

			static Matrix4x4 ortho(T left, T right, T bottom, T top, T nearZ, T farZ)
			{
				Matrix4x4 m{};
				m[0][0] = 2.0 / (right - left);
				m[1][1] = 2.0 / (top - bottom);
				m[2][2] = -2.0 / (farZ - nearZ);
				m[3][0] = -(right + left) / (right - left);
				m[3][1] = -(top + bottom) / (top - bottom);
				m[3][2] = -(farZ + nearZ) / (farZ - nearZ);
				return m;
			}

			static Matrix4x4 frustum(T left, T right, T bottom, T top, T nearZ, T farZ)
			{
				T rl = right - left;
				T tb = top - bottom;
				T fn = farZ - nearZ;

				Matrix4x4 m{};
				m[0][0] = (T(2) * nearZ) / rl;
				m[0][2] = (right + left) / rl;
				m[1][1] = (T(2) * nearZ) / tb;
				m[1][2] = (top + bottom) / tb;
				m[2][2] = -(farZ + nearZ) / (farZ - nearZ);
				m[2][3] = -1;
				m[3][2] = -2.0 * farZ * nearZ / (farZ - nearZ);
				m[3][3] = 0;
				return m;
			}

			bool vaildTRS() const
			{
				return determinat() != T(0);
			}

			Matrix4x4 operator*(const Matrix4x4& rhs) const
			{
				Matrix4x4 r{};

				for (int c = 0; c < 4; c++)
				{
					Vector4 v = rhs[c];
					r[c] = (*this) * v;
				}

				return r;
			}

			Matrix4x4& operator*=(const Matrix4x4& rhs)
			{
				*this = (*this) * rhs;
				return *this;
			}

			Vector3 operator*(const Vector3& v) const
			{
				return multiplyPoint(v);
			}

			Vector4 operator*(const Vector4& v) const 
			{
				return Vector4(
					m11 * v.x + m12 * v.y + m13 * v.z + m14 * v.w,
					m21 * v.x + m22 * v.y + m23 * v.z + m24 * v.w,
					m31 * v.x + m32 * v.y + m33 * v.z + m34 * v.w,
					m41 * v.x + m42 * v.y + m43 * v.z + m44 * v.w
				);
			}

			friend std::ostream& operator<<(std::ostream& os, const Matrix4x4& m)
			{
				os << m[0] << std::endl << m[1] << std::endl << m[2] << std::endl << m[3];
				return os;
			}
		};
	}
}

#endif
