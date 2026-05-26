#ifndef __OBSIDIANENGINE_MATH_MATRIXMXN_HPP__
#define __OBSIDIANENGINE_MATH_MATRIXMXN_HPP__

#include "ObsidianEngine/Math/MathUtils.hpp"
#include "ObsidianEngine/Math/Quaternion/QuaternionTypes.hpp"
#include "ObsidianEngine/Math/Vector/VectorTypes.hpp"

#include <iostream>
#include <iomanip>
#include <numbers>
#include <string>
#include <sstream>

namespace ObsidianEngine::detail
{
	template<size_t RowsCount, size_t ColsCount, typename T>
	struct Matrix
	{
		static_assert(std::is_arithmetic_v<T>, "Matrix type must be numeric!");

		using ValueType = T;

		static constexpr size_t Rows = RowsCount;
		static constexpr size_t Cols = ColsCount;

		std::array<T, Rows * Cols> data;

		constexpr Matrix() noexcept;
		constexpr Matrix(std::initializer_list<Vector<T, RowsCount>> columns) noexcept;
		constexpr Matrix(const Matrix&) noexcept = default;
		constexpr Matrix(Matrix&&) noexcept = default;

		constexpr Matrix& operator=(const Matrix&) noexcept = default;
		constexpr Matrix& operator=(Matrix&&) noexcept = default;

		template<typename U>
		requires (!std::is_same_v<U, T>)
		constexpr explicit Matrix(const Matrix<RowsCount, ColsCount, U>& other) noexcept;

		template<typename... Args>
		requires (sizeof...(Args) == RowsCount * ColsCount) && (std::is_convertible_v<Args, T> && ...)
		constexpr Matrix(Args... args) noexcept;

		template<typename U>
		requires std::is_arithmetic_v<U>
		auto cast() const noexcept;

		template<size_t R, size_t C>
		constexpr auto reshape() const noexcept;

		template<size_t R, size_t C, typename U>
		requires std::is_arithmetic_v<U>
		constexpr auto reshapeAs() const noexcept;

		static constexpr Matrix fromRows(std::initializer_list<Vector<T, ColsCount>> rows) noexcept;

		template<typename... Args>
		static constexpr Matrix fromRows(Args... args) noexcept;

		static constexpr Matrix fromColumns(std::initializer_list<Vector<T, RowsCount>> rows) noexcept;

		template<typename... Args>
		static constexpr Matrix fromColumns(Args... args) noexcept;

		template<typename = void>
		Vector<T, (RowsCount > ColsCount ? RowsCount : ColsCount)> asVector() const noexcept;

		Vector<T, RowsCount> getColumn(size_t col) const noexcept;

		Vector<T, ColsCount> getRow(size_t row) const noexcept;

		void setColumn(size_t col, const Vector<T, RowsCount>& v) noexcept;

		void setRow(size_t row, const Vector<T, ColsCount>& v) noexcept;

		template<size_t SubRows, size_t SubCols>
		Matrix<SubRows, SubCols, T> slice(size_t startRow = 0, size_t startCol = 0) const noexcept;

		template<size_t SubRows, size_t SubCols>
		void setBlock(const Matrix<SubRows, SubCols, T>& block, size_t startRow = 0, size_t startCol = 0) noexcept;

		Vector<T, RowsCount>& operator[](size_t col) noexcept;

		const Vector<T, RowsCount>& operator[](size_t col) const noexcept;

		T& operator()(size_t row, size_t col) noexcept;

		const T& operator()(size_t row, size_t col) const noexcept;

		static constexpr Matrix identity() noexcept;

		static constexpr Matrix zero() noexcept;

		static constexpr Matrix one() noexcept;

		Matrix& divAssign(T scalar) noexcept;

		Matrix div(T scalar) const noexcept;

		Matrix operator/(T scalar) const noexcept;

		Matrix operator/=(T scalar) const noexcept;

		Matrix& mulAssign(T scalar) noexcept;

		Matrix mul(T scalar) const noexcept;

		Matrix operator*(T scalar) const noexcept;

		Matrix operator*=(T scalar) noexcept;

		Matrix& mulAssign(const Matrix& rhs) noexcept requires (RowsCount == ColsCount);

		template<size_t NewCols>
		Matrix<RowsCount, NewCols, T> mul(const Matrix<ColsCount, NewCols, T>& rhs) const noexcept;

		template<size_t NewCols>
		Matrix<RowsCount, NewCols, T> operator*(const Matrix<ColsCount, NewCols, T>& rhs) const noexcept;

		Matrix& operator*=(const Matrix& rhs) noexcept requires (RowsCount == ColsCount);

		Vector <T, RowsCount> mul(const Vector<T, ColsCount>& rhs) const noexcept;

		Vector <T, ColsCount> mulLeft(const Vector<T, RowsCount>& rhs) const noexcept;

		Vector<T, RowsCount> operator*(const Vector<T, ColsCount>& rhs) const noexcept;

		Matrix& addAssign(const Matrix& rhs) noexcept;

		Matrix add(const Matrix& rhs) const noexcept;

		Matrix operator+(const Matrix& rhs) const noexcept;

		Matrix& operator+=(const Matrix<RowsCount, ColsCount, T>& rhs) noexcept;

		Matrix& subAssign(const Matrix& rhs) noexcept;

		Matrix sub(const Matrix& rhs) const noexcept;

		Matrix operator-(const Matrix& rhs) const noexcept;

		Matrix& operator-=(const Matrix& rhs) noexcept;

		Matrix negate() const noexcept;

		Matrix operator-() const noexcept;

		Matrix<ColsCount, RowsCount, T> transpose() const noexcept;

		bool isIdentity() const noexcept;

		T trace() const noexcept;

		T determinant() const noexcept;

		T minorValue(size_t row, size_t col) const noexcept;

		T cofactor(size_t row, size_t col) const noexcept;

		auto inverse() const noexcept;

		static bool isEqual(const Matrix& rhs, const Matrix& lhs, T epsilon = Math<T>::Epsilon) noexcept;

		bool equals(const Matrix& rhs, T epsilon = Math<T>::Epsilon) const noexcept;

		bool operator==(const Matrix& rhs) const noexcept;

		bool operator!=(const Matrix& rhs) const noexcept;

		std::string toString(int precision = 2) const;

	private:
		template<size_t Dim>
		static T calculateDeterminant(const Matrix<Dim, Dim, T>& m) noexcept;
	}; // class Matrix

	template<size_t RowsCount, size_t ColsCount, typename T>
	Vector<T, ColsCount> operator*(const Vector<T, RowsCount>& lhs, const Matrix<RowsCount, ColsCount, T>& rhs) noexcept;

	template<size_t RowsCount, size_t ColsCount, typename T>
	Matrix<RowsCount, ColsCount, T> operator*(T scalar, const Matrix<RowsCount, ColsCount, T>& m) noexcept;

	template<size_t RowsCount, size_t ColsCount, typename T>
	std::ostream& operator<<(std::ostream& os, const Matrix<RowsCount, ColsCount, T>& m);

} // namespace ObsidianEngine::detail

#include "MatrixMxN.ipp"

#endif