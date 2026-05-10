#ifndef MATRIX_H_
#define MATRIX_H_

#include "Vector.h"

#include <iostream>
#include <iomanip>
#include <cmath>
#include <numbers>

namespace ObsidianEngine
{
	namespace detail
	{
		template<typename T>
		struct Quaternion;

		template<size_t Rows, size_t Cols, typename T>
		struct Matrix;

		template<typename T>
		struct Matrix4x4;
	}

	using Matrix2x2 = detail::Matrix<2, 2, float>;
	using Matrix3x3 = detail::Matrix<3, 3, float>;
	using Matrix4x4 = detail::Matrix4x4<float>;

	using Matrix3x4 = detail::Matrix<3, 4, float>;
	using Matrix4x3 = detail::Matrix<4, 3, float>;
	using Matrix2x3 = detail::Matrix<2, 3, float>;

	using Matrix2x2d = detail::Matrix<2, 2, double>;
	using Matrix3x3d = detail::Matrix<3, 3, double>;
	using Matrix4x4d = detail::Matrix4x4<double>;

	using Matrix2x2i = detail::Matrix<2, 2, int>;

	namespace detail
	{
		template<size_t Rows, size_t Cols, typename T>
		struct Matrix
		{
			static_assert(std::is_arithmetic_v<T>, "Matrix type must be numeric!");

			std::array<T, Rows* Cols> data;

			constexpr Matrix() : data{ 0 } {}

			constexpr Matrix(std::initializer_list<Vector<T, Rows>> columns) : data{}
			{
				size_t c = 0;
				for (const auto& colVec : columns)
				{
					if (c < Cols)
					{
						for (size_t r = 0; r < Rows; ++r)
						{
							(*this)(r, c) = colVec[r];
						}
						c++;
					}
				}
			}

			template<typename... Args>
			constexpr Matrix(Args... args) : data{ static_cast<T>(args)... }
			{
				static_assert(sizeof...(Args) == Rows * Cols, "Matrix constructor must receive exactly Rows * Cols arguments!");
			}

			static constexpr Matrix fromRows(std::initializer_list<Vector<T, Cols>> rows)
			{
				Matrix m;
				size_t r = 0;
				for (const auto& rowVec : rows)
				{
					if (r < Rows)
					{
						for (size_t c = 0; c < Cols; ++c)
						{
							m(r, c) = rowVec[c];
						}
						r++;
					}
				}
				return m;
			}

			template<typename... Args>
			static constexpr Matrix fromRows(Args... args)
			{
				static_assert(sizeof...(Args) == Rows * Cols, "fromRows must receive exactly Rows * Cols arguments!");

				T vals[] = { static_cast<T>(args)... };

				Matrix m;
				size_t i = 0;
				for (size_t r = 0; r < Rows; ++r)
				{
					for (size_t c = 0; c < Cols; ++c)
					{
						m(r, c) = vals[i++];
					}
				}
				return m;
			}

			static constexpr Matrix fromColumns(std::initializer_list<Vector<T, Cols>> rows)
			{
				return Matrix(rows);
			}

			template<typename... Args>
			static constexpr Matrix fromColumns(Args... args)
			{
				static_assert(sizeof...(Args) == Rows * Cols, "fromColumns must receive exactly Rows * Cols arguments!");

				return Matrix(args...);
			}

			template<typename = void>
			Vector<T, (Rows > Cols ? Rows : Cols)> asVector() const
			{
				static_assert(Rows == 1 || Cols == 1, "asVector() requires the matrix to be either a single row or a single column.");

				if constexpr (Cols == 1)
				{
					return  getColumn(0);
				}
				else
				{
					return getRow(0);
				}
			}

			Vector<T, Rows> getColumn(size_t col) const
			{
				assert(col < Cols);
				return (*this)[col];
			}

			Vector<T, Cols> getRow(size_t row) const
			{
				assert(row < Rows);
				Vector<T, Cols> result;
				for (size_t c = 0; c < Cols; ++c)
				{
					result[c] = (*this)(row, c);
				}
				return result;
			}

			void setColumn(size_t col, const Vector<T, Rows>& v)
			{
				assert(col < Cols);
				for (size_t r = 0; r < Rows; ++r)
				{
					(*this)(r, col) = v[r];
				}
			}

			void setRow(size_t row, const Vector<T, Cols>& v)
			{
				assert(row < Rows);
				for (size_t c = 0; c < Cols; ++c)
				{
					(*this)(row, c) = v[c];
				}
			}

			template<size_t SubRows, size_t SubCols>
			Matrix<SubRows, SubCols, T> slice(size_t startRow = 0, size_t startCol = 0) const
			{
				static_assert(SubRows <= Rows, "Slice rows exceed matrix rows.");
				static_assert(SubCols <= Cols, "Slice columns exceed matrix columns.");

				assert(startRow + SubRows <= Rows && "Slice out of row bounds!");
				assert(startCol + SubCols <= Cols && "Slice out of column bounds!");

				Matrix<SubRows, SubCols, T> result;
				for (size_t c = 0; c < SubCols; ++c)
				{
					for (size_t r = 0; r < SubRows; ++r)
					{
						result(r, c) = (*this)(startRow + r, startCol + c);
					}
				}
				return result;
			}

			template<size_t SubRows, size_t SubCols>
			void setBlock(const Matrix<SubRows, SubCols, T>& block, size_t startRow = 0, size_t startCol = 0)
			{
				assert(startRow + SubRows <= Rows && "Block out of row bounds!");
				assert(startCol + SubCols <= Cols && "Block out of column bounds!");

				for (size_t c = 0; c < SubCols; ++c)
				{
					for (size_t r = 0; r < SubRows; ++r)
					{
						(*this)(startRow + r, startCol + c) = block(r, c);
					}
				}
			}

			Vector<T, Rows>& operator[](size_t col)
			{
				assert(col < Cols);
				return reinterpret_cast<Vector<T, Rows>&>(data.at(col * Rows));
			}

			const Vector<T, Rows>& operator[](size_t col) const
			{
				assert(col < Cols);
				return reinterpret_cast<const Vector<T, Rows>&>(data.at(col * Rows));
			}

			T& operator()(size_t row, size_t col)
			{
				assert(row < Rows && col < Cols);
				return data.at(col * Rows + row);
			}

			const T& operator()(size_t row, size_t col) const
			{
				assert(row < Rows && col < Cols);
				return data.at(col * Rows + row);
			}

			static constexpr Matrix identity() 
			{
				Matrix m;
				size_t n = (Rows < Cols) ? Rows : Cols;
				for (size_t i = 0; i < n; ++i) m(i, i) = T(1);
				return m;
			}

			static constexpr Matrix zero()
			{
				return Matrix();
			}

			template<size_t NewCols>
			Matrix<Rows, NewCols, T> operator*(const Matrix<Cols, NewCols, T>& rhs) const 
			{
				Matrix<Rows, NewCols, T> result;
				for (size_t r = 0; r < Rows; ++r) {
					for (size_t c = 0; c < NewCols; ++c) {
						T sum = 0;
						for (size_t k = 0; k < Cols; ++k) {
							sum += (*this)(r, k) * rhs(k, c);
						}
						result(r, c) = sum;
					}
				}
				return result;
			}

			Vector<T, Rows> operator*(const Vector<T, Cols>& v) const
			{
				Vector<T, Rows> result;

				for (size_t r = 0; r < Rows; ++r)
				{
					T sum = 0;
					for (size_t c = 0; c < Cols; ++c)
					{
						sum += (*this)(r, c) * v[c];
					}
					result[r] = sum;
				}

				return result;
			}

			Matrix<Cols, Rows, T> transpose() const
			{
				Matrix<Cols, Rows, T> result;

				for (size_t c = 0; c < Cols; ++c)
				{
					for (size_t r = 0; r < Rows; ++r)
					{
						result(c, r) = (*this)(r, c);
					}
				}

				return result;
			}

			bool isIdentity() const
			{
				return *this == identity();
			}

			T trace() const
			{
				static_assert(Rows == Cols, "Trace is usually defined for square matrices only!");

				T sum = 0;

				constexpr size_t n = (Rows < Cols) ? Rows : Cols;

				for (size_t i = 0; i < n; ++i)
				{
					sum += (*this)(i, i);
				}

				return sum;
			}

			T determinant() const 
			{
				static_assert(Rows == Cols, "Determinant can only be calculated for square matrices!");
				return calculate_determinant(*this);
			}

			T minor_value(size_t row, size_t col) const
			{
				static_assert(Rows == Cols && Rows > 1, "Minor requires a square matrix of at least 2x2");

				Matrix<Rows - 1, Cols - 1, T> sub;
				size_t sub_r = 0;
				for (size_t i = 0; i < Rows; ++i)
				{
					if (i == row) continue;
					size_t sub_c = 0;
					for (size_t j = 0; j < Cols; ++j)
					{
						if (j == col) continue;
						sub(sub_r, sub_c++) = (*this)(i, j);
					}
					sub_r++;
				}
				return sub.determinant();
			}

			T cofactor(size_t row, size_t col) const
			{
				T m = minor_value(row, col);
				return ((row + col) % 2 == 0) ? m : -m;
			}

			Matrix inverse() const
			{
				static_assert(Rows == Cols, "Only square matrices can be inverted.");

				T det = this->determinant();

				if (std::abs(det) < static_cast<T>(1e-7))
				{
					return identity();
				}

				T invDet = T(1) / det;
				Matrix result;

				for (size_t r = 0; r < Rows; ++r)
				{
					for (size_t c = 0; c < Cols; ++c)
					{
						result(c, r) = cofactor(r, c) * invDet;
					}
				}

				return result;
			}

			bool operator==(const Matrix& rhs) const
			{
				return data == rhs.data;
			}

			bool operator!=(const Matrix& rhs) const
			{
				return !(*this == rhs);
			}

			friend std::ostream& operator<<(std::ostream& os, const Matrix& m) 
			{
				std::ios_base::fmtflags f(os.flags());

				os << "[\n";
				for (size_t r = 0; r < Rows; ++r) 
				{
					os << "  ";
					for (size_t c = 0; c < Cols; ++c) 
					{
						os << std::setw(10) << std::left << std::setprecision(4) << m(r, c);

						if (c < Cols - 1) os << " ";
					}
					os << "\n";
				}
				os << "]";

				os.flags(f);
				return os;
			}

		private:
			template<size_t Dim>
			static T calculate_determinant(const Matrix<Dim, Dim, T>& m)
			{
				if constexpr (Dim == 1)
				{
					return m(0, 0);
				}
				else if constexpr (Dim == 2)
				{
					return m(0, 0) * m(1, 1) - m(1, 0) * m(0, 1);
				}
				else
				{
					T det = 0;
					for (size_t c = 0; c < Dim; ++c)
					{
						Matrix<Dim - 1, Dim - 1, T> sub;
						for (size_t i = 1; i < Dim; ++i)
						{
							size_t sub_c = 0;
							for (size_t j = 0; j < Dim; ++j)
							{
								if (j == c) continue;
								sub(i - 1, sub_c++) = m(i, j);
							}
						}

						T term = m(0, c) * calculate_determinant(sub);
						if (c % 2 == 1) det -= term;
						else det += term;
					}
					return det;
				}
			}
		};

		template<size_t Rows, size_t Cols, typename T>
		Vector<T, Cols> operator*(const Vector<T, Rows>& v, const Matrix<Rows, Cols, T>& m)
		{
			Vector<T, Cols> result;

			for (size_t c = 0; c < Cols; ++c)
			{
				T sum = 0;
				for (size_t r = 0; r < Rows; ++r)
				{
					sum += v[r] * m(r, c);
				}
				result[c] = sum;
			}

			return result;
		}

		template<typename T>
		struct Matrix4x4 : public detail::Matrix<4, 4, T>
		{
			using Base = detail::Matrix<4, 4, T>;
			using Base::Base;

			Matrix4x4(const Base& other) : Base(other) {}

			T determinant() const
			{
				const auto& m = *this;
				return m(0, 0) * (m(1, 1) * m(2, 2) * m(3, 3) + m(1, 2) * m(2, 3) * m(3, 1) + m(1, 3) * m(2, 1) * m(3, 2)
					- m(1, 3) * m(2, 2) * m(3, 1) - m(1, 2) * m(2, 1) * m(3, 3) - m(1, 1) * m(2, 3) * m(3, 2))
					- m(1, 0) * (m(0, 1) * m(2, 2) * m(3, 3) + m(0, 2) * m(2, 3) * m(3, 1) + m(0, 3) * m(2, 1) * m(3, 2)
						- m(0, 3) * m(2, 2) * m(3, 1) - m(0, 2) * m(2, 1) * m(3, 3) - m(0, 1) * m(2, 3) * m(3, 2))
					+ m(2, 0) * (m(0, 1) * m(1, 2) * m(3, 3) + m(0, 2) * m(1, 3) * m(3, 1) + m(0, 3) * m(1, 1) * m(3, 2)
						- m(0, 3) * m(1, 2) * m(3, 1) - m(0, 2) * m(1, 1) * m(3, 3) - m(0, 1) * m(1, 3) * m(3, 2))
					- m(3, 0) * (m(0, 1) * m(1, 2) * m(2, 3) + m(0, 2) * m(1, 3) * m(2, 1) + m(0, 3) * m(1, 1) * m(2, 2)
						- m(0, 3) * m(1, 2) * m(2, 1) - m(0, 2) * m(1, 1) * m(2, 3) - m(0, 1) * m(1, 3) * m(2, 2));
			}

			Matrix4x4 inverse() const
			{
				T det = this->determinant();

				if (std::abs(det) < static_cast<T>(1e-7))
				{
					return Base::identity();
				}

				T invDet = T(1) / det;
				Matrix4x4 result;

				for (size_t r = 0; r < 4; ++r)
				{
					for (size_t c = 0; c < 4; ++c)
					{
						result(c, r) = this->cofactor(r, c) * invDet;
					}
				}
				return result;
			}

			Vector3 multiplyPoint(const Vector3& p) const
			{
				Vector4 r = (*this) * Vector4{ p.x, p.y, p.z, 1.0f };
				return { r.x, r.y, r.z };
			}

			Vector3 multiplyDirection(const Vector3& v) const
			{
				Vector4 r = (*this) * Vector4{ v.x, v.y, v.z, 0.0f };
				return { r.x, r.y, r.z };
			}

			Vector3 lossyScale() const
			{
				T x = Vector3{ (*this)(0, 0), (*this)(1, 0), (*this)(2, 0) }.sqrMagnitude();
				T y = Vector3{ (*this)(0, 1), (*this)(1, 1), (*this)(2, 1) }.sqrMagnitude();
				T z = Vector3{ (*this)(0, 2), (*this)(1, 2), (*this)(2, 2) }.sqrMagnitude();

				return Vector3{
					std::sqrt(x),
					std::sqrt(y),
					std::sqrt(z)
				};
			}

			Quaternion<T> rotation() const
			{
				Matrix4x4 m = *this;

				Vector3 right(
					m(0, 0), m(1, 0), m(2, 0)
				);

				Vector3 up(
					m(0, 1), m(1, 1), m(2, 1)
				);

				Vector3 forward(
					m(0, 2), m(1, 2), m(2, 2)
				);

				right.normalize();
				up.normalize();
				forward.normalize();

				m(0, 0) = right.x;
				m(1, 0) = right.y;
				m(2, 0) = right.z;

				m(0, 1) = up.x;
				m(1, 1) = up.y;
				m(2, 1) = up.z;

				m(0, 2) = forward.x;
				m(1, 2) = forward.y;
				m(2, 2) = forward.z;

				return Quaternion<T>::fromMatrix(m);
			}

			Vector3 position() const
			{
				return Vector3{
					(*this)(0, 3),
					(*this)(1, 3),
					(*this)(2, 3)
				};
			}

			static Matrix4x4 rotateX(T r)
			{
				T c = std::cos(r);
				T s = std::sin(r);
				Matrix4x4 m = Base::identity();
				m(1, 1) = c;  m(1, 2) = -s;
				m(2, 1) = s;  m(2, 2) = c;
				return m;
			}

			static Matrix4x4 rotateY(T r)
			{
				T c = std::cos(r);
				T s = std::sin(r);
				Matrix4x4 m = Base::identity();
				m(0, 0) = c;  m(0, 2) = s;
				m(2, 0) = -s; m(2, 2) = c;
				return m;
			}

			static Matrix4x4 rotateZ(T r)
			{
				T c = std::cos(r);
				T s = std::sin(r);
				Matrix4x4 m = Base::identity();
				m(0, 0) = c;  m(0, 1) = -s;
				m(1, 0) = s;  m(1, 1) = c;
				return m;
			}

			static Matrix4x4 rotate(const Quaternion<T>& q)
			{
				return q.toMatrix();
			}

			static Matrix4x4 rotate(const Vector3& e)
			{
				return Quaternion<T>::fromEuler(e).toMatrix();
			}

			static Matrix4x4 translate(const Vector3& t)
			{
				Matrix4x4 m = Base::identity();
				m(0, 3) = t.x;
				m(1, 3) = t.y;
				m(2, 3) = t.z;
				return m;
			}

			static Matrix4x4 scale(const Vector3& s)
			{
				Matrix4x4 m = Base::identity();
				m(0, 0) = s.x;
				m(1, 1) = s.y;
				m(2, 2) = s.z;
				return m;
			}

			static Matrix4x4 trs(const Vector3& pos, const Quaternion<T>& rot, const Vector3& s)
			{
				return translate(pos) * rotate(rot) * scale(s);
			}

			bool validTRS() const
			{
				return std::abs(this->determinant()) > static_cast<T>(1e-9);
			}

			static Matrix4x4 lookAt(const Vector3& eye, const Vector3& target, const Vector3& up)
			{
				Vector3 f = (target - eye).normalized();
				Vector3 s = Vector3::cross(f, up).normalized();
				Vector3 u = Vector3::cross(s, f).normalized();

				Matrix4x4 m = Base::zero();

				m(0, 0) = s.x;  m(1, 0) = u.x;  m(2, 0) = -f.x; m(3, 0) = 0;
				m(0, 1) = s.y;  m(1, 1) = u.y;  m(2, 1) = -f.y; m(3, 1) = 0;
				m(0, 2) = s.z;  m(1, 2) = u.z;  m(2, 2) = -f.z; m(3, 2) = 0;

				m(0, 3) = -Vector3::dot(s, eye);
				m(1, 3) = -Vector3::dot(u, eye);
				m(2, 3) = Vector3::dot(f, eye);
				m(3, 3) = 1;

				return m;
			}

			static Matrix4x4 perspective(T fov, T aspect, T nearZ, T farZ)
			{
				T fovRad = fov * std::numbers::pi_v<T> / static_cast<T>(180.0);
				T f = static_cast<T>(1.0) / std::tan(fovRad / static_cast<T>(2.0));

				Matrix4x4 m = Base::zero();
				m(0, 0) = f / aspect;
				m(1, 1) = f;
				m(2, 2) = -(farZ + nearZ) / (farZ - nearZ);
				m(2, 3) = -(static_cast<T>(2) * farZ * nearZ) / (farZ - nearZ);
				m(3, 2) = -1;
				m(3, 3) = 0;
				return m;
			}

			static Matrix4x4 ortho(T left, T right, T bottom, T top, T nearZ, T farZ)
			{
				Matrix4x4 m = Base::identity();
				m(0, 0) = static_cast<T>(2) / (right - left);
				m(1, 1) = static_cast<T>(2) / (top - bottom);
				m(2, 2) = -static_cast<T>(2) / (farZ - nearZ);

				m(0, 3) = -(right + left) / (right - left);
				m(1, 3) = -(top + bottom) / (top - bottom);
				m(2, 3) = -(farZ + nearZ) / (farZ - nearZ);
				return m;
			}

			static Matrix4x4 frustum(T left, T right, T bottom, T top, T nearZ, T farZ)
			{
				T rl = right - left;
				T tb = top - bottom;
				T fn = farZ - nearZ;

				Matrix4x4 m = Base::zero();
				m(0, 0) = (static_cast<T>(2) * nearZ) / rl;
				m(1, 1) = (static_cast<T>(2) * nearZ) / tb;

				m(0, 2) = (right + left) / rl;
				m(1, 2) = (top + bottom) / tb;
				m(2, 2) = -(farZ + nearZ) / fn;
				m(2, 3) = -(static_cast<T>(2) * farZ * nearZ) / fn;

				m(3, 2) = -1;
				m(3, 3) = 0;
				return m;
			}
		};
	}
}

#endif
