#ifndef __OBSIDIANENGINE_MATH_MATRIXMXN_H__
#define __OBSIDIANENGINE_MATH_MATRIXMXN_H__

#include "ObsidianEngine/Math/MathUtils.h"
#include "ObsidianEngine/Math/Quaternion/QuaternionTypes.h"
#include "ObsidianEngine/Math/Vector/VectorTypes.h"

#include <iostream>
#include <iomanip>
#include <numbers>

namespace ObsidianEngine::detail
{
	template<size_t RowsCount, size_t ColsCount, typename T>
	struct Matrix
	{
		using ValueType = T;

		static constexpr size_t Rows = RowsCount;
		static constexpr size_t Cols = ColsCount;

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

		template<typename U>
		constexpr Matrix(const Matrix<Rows, Cols, U>& other) : data{}
		{
			for (size_t i = 0; i < Rows * Cols; ++i)
			{
				data[i] = static_cast<T>(other.data[i]);
			}
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

		static constexpr Matrix fromColumns(std::initializer_list<Vector<T, Rows>> rows)
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
			for (size_t i = 0; i < n; ++i) m(i, i) = static_cast<T>(1);
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

		auto inverse() const -> Matrix<Rows, Cols, std::common_type_t<T, float>>
		{
			static_assert(Rows == Cols, "Only square matrices can be inverted!");

			using FPT = std::common_type_t<T, float>;

			FPT det = Math<FPT>::val(this->determinant());

			FPT epsilon = Math<FPT>::val(1e-7);

			if (std::abs(det) < epsilon)
			{
				return Matrix<Rows, Cols, FPT>::identity();
			}

			FPT invDet = Math<FPT>::val(1.0) / det;
			Matrix<Rows, Cols, FPT> result;

			for (size_t r = 0; r < Rows; ++r)
			{
				for (size_t c = 0; c < Cols; ++c)
				{
					result(c, r) = Math<FPT>::val(cofactor(r, c)) * invDet;
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
	inline Vector<T, Cols> operator*(const Vector<T, Rows>& v, const Matrix<Rows, Cols, T>& m)
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

	template<size_t R, size_t K, size_t C, typename T1, typename T2>
	inline auto operator*(const Matrix<R, K, T1>& lhs, const Matrix<K, C, T2>& rhs) -> Matrix<R, C, std::common_type_t<T1, T2>>
	{
		using ResultT = std::common_type_t<T1, T2>;
		Matrix<R, C, ResultT> result;

		for (size_t r = 0; r < R; ++r)
		{
			for (size_t c = 0; c < C; ++c)
			{
				ResultT sum = 0;
				for (size_t k = 0; k < K; ++k)
				{
					sum += static_cast<ResultT>(lhs(r, k)) * static_cast<ResultT>(rhs(k, c));
				}
				result(r, c) = sum;
			}
		}
		return result;
	}

	template<size_t R, size_t C, typename T1, typename T2>
	inline auto operator*(const Matrix<R, C, T1>& m, const Vector<T2, C>& v)-> Vector<std::common_type_t<T1, T2>, R>
	{
		using ResultT = std::common_type_t<T1, T2>;
		Vector<ResultT, R> result;

		for (size_t r = 0; r < R; ++r)
		{
			ResultT sum = 0;
			for (size_t c = 0; c < C; ++c)
			{
				sum += static_cast<ResultT>(m(r, c)) * static_cast<ResultT>(v[c]);
			}
			result[r] = sum;
		}
		return result;
	}

	template<size_t R, size_t C, typename T1, typename T2>
	inline auto operator*(const Vector<T1, R>& v, const Matrix<R, C, T2>& m) -> Vector<std::common_type_t<T1, T2>, C>
	{
		using ResultT = std::common_type_t<T1, T2>;
		Vector<ResultT, C> result;

		for (size_t c = 0; c < C; ++c)
		{
			ResultT sum = 0;
			for (size_t r = 0; r < R; ++r)
			{
				sum += static_cast<ResultT>(v[r]) * static_cast<ResultT>(m(r, c));
			}
			result[c] = sum;
		}
		return result;
	}
}

#endif