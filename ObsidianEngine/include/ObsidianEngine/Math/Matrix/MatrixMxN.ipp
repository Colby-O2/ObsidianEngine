namespace ObsidianEngine::detail
{
    template<size_t RowsCount, size_t ColsCount, typename T>
    constexpr Matrix<RowsCount, ColsCount, T>::Matrix() noexcept : data{} {}

    template<size_t RowsCount, size_t ColsCount, typename T>
    constexpr Matrix<RowsCount, ColsCount, T>::Matrix(std::initializer_list<Vector<T, RowsCount>> columns) noexcept : data{}
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

    template<size_t RowsCount, size_t ColsCount, typename T>
    template<typename U>
    requires (!std::is_same_v<U, T>)
    constexpr Matrix<RowsCount, ColsCount, T>::Matrix(const Matrix<RowsCount, ColsCount, U>& other) noexcept : data{}
    {
        for (size_t i = 0; i < Rows * Cols; ++i)
        {
            data[i] = static_cast<T>(other.data[i]);
        }
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    template<typename... Args>
    requires (sizeof...(Args) == RowsCount * ColsCount) && (std::is_convertible_v<Args, T> && ...)
    constexpr Matrix<RowsCount, ColsCount, T>::Matrix(Args... args) noexcept : data{ static_cast<T>(args)... } {}

    template<size_t RowsCount, size_t ColsCount, typename T>
    template<typename U>
    requires std::is_arithmetic_v<U>
    auto Matrix<RowsCount, ColsCount, T>::cast() const noexcept
    {
        Matrix<Rows, Cols, U> result;

        for (size_t i = 0; i < Rows * Cols; ++i)
        {
            result.data[i] = static_cast<U>(data[i]);
        }

        return result;
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    template<size_t R, size_t C>
    constexpr auto Matrix<RowsCount, ColsCount, T>::reshape() const noexcept
    {
        Matrix<R, C, T> result{};

        constexpr size_t minR = (R < Rows) ? R : Rows;
        constexpr size_t minC = (C < Cols) ? C : Cols;

        for (size_t r = 0; r < minR; ++r)
        {
            for (size_t c = 0; c < minC; ++c)
            {
                result(r, c) = (*this)(r, c);
            }
        }

        return result;
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    template<size_t R, size_t C, typename U>
    requires std::is_arithmetic_v<U>
    constexpr auto Matrix<RowsCount, ColsCount, T>::reshapeAs() const noexcept
    {
        Matrix<R, C, U> result{};

        constexpr size_t minR = (R < Rows) ? R : Rows;
        constexpr size_t minC = (C < Cols) ? C : Cols;

        for (size_t r = 0; r < minR; ++r)
        {
            for (size_t c = 0; c < minC; ++c)
            {
                result(r, c) = static_cast<U>((*this)(r, c));
            }
        }

        return result;
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    constexpr Matrix<RowsCount, ColsCount, T> Matrix<RowsCount, ColsCount, T>::fromRows(std::initializer_list<Vector<T, ColsCount>> rows) noexcept
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

    template<size_t RowsCount, size_t ColsCount, typename T>
    template<typename... Args>
    static constexpr Matrix<RowsCount, ColsCount, T> Matrix<RowsCount, ColsCount, T>::fromRows(Args... args) noexcept
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

    template<size_t RowsCount, size_t ColsCount, typename T>
    constexpr Matrix<RowsCount, ColsCount, T> Matrix<RowsCount, ColsCount, T>::fromColumns(std::initializer_list<Vector<T, RowsCount>> rows) noexcept
    {
        return Matrix(rows);
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    template<typename... Args>
    static constexpr Matrix<RowsCount, ColsCount, T> Matrix<RowsCount, ColsCount, T>::fromColumns(Args... args) noexcept
    {
        static_assert(sizeof...(Args) == Rows * Cols, "fromColumns must receive exactly Rows * Cols arguments!");

        return Matrix(args...);
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    template<typename = void>
    Vector<T, (RowsCount > ColsCount ? RowsCount : ColsCount)> Matrix<RowsCount, ColsCount, T>::asVector() const noexcept
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

    template<size_t RowsCount, size_t ColsCount, typename T>
    Vector<T, RowsCount> Matrix<RowsCount, ColsCount, T>::getColumn(size_t col) const noexcept
    {
        assert(col < Cols);
        return (*this)[col];
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    Vector<T, ColsCount> Matrix<RowsCount, ColsCount, T>::getRow(size_t row) const noexcept
    {
        assert(row < Rows);
        Vector<T, Cols> result;
        for (size_t c = 0; c < Cols; ++c)
        {
            result[c] = (*this)(row, c);
        }
        return result;
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    void Matrix<RowsCount, ColsCount, T>::setColumn(size_t col, const Vector<T, RowsCount>& v) noexcept
    {
        assert(col < Cols);
        for (size_t r = 0; r < Rows; ++r)
        {
            (*this)(r, col) = v[r];
        }
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    void Matrix<RowsCount, ColsCount, T>::setRow(size_t row, const Vector<T, ColsCount>& v) noexcept
    {
        assert(row < Rows);
        for (size_t c = 0; c < Cols; ++c)
        {
            (*this)(row, c) = v[c];
        }
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    template<size_t SubRows, size_t SubCols>
    Matrix<SubRows, SubCols, T> Matrix<RowsCount, ColsCount, T>::slice(size_t startRow, size_t startCol) const noexcept
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

    template<size_t RowsCount, size_t ColsCount, typename T>
    template<size_t SubRows, size_t SubCols>
    void Matrix<RowsCount, ColsCount, T>::setBlock(const Matrix<SubRows, SubCols, T>& block, size_t startRow, size_t startCol) noexcept
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

    template<size_t RowsCount, size_t ColsCount, typename T>
    Vector<T, RowsCount>& Matrix<RowsCount, ColsCount, T>::operator[](size_t col) noexcept
    {
        assert(col < Cols);
        return reinterpret_cast<Vector<T, Rows>&>(data[col * Rows]);
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    const Vector<T, RowsCount>& Matrix<RowsCount, ColsCount, T>::operator[](size_t col) const noexcept
    {
        assert(col < Cols);
        return reinterpret_cast<const Vector<T, Rows>&>(data[col * Rows]);
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    T& Matrix<RowsCount, ColsCount, T>::operator()(size_t row, size_t col) noexcept
    {
        assert(row < Rows && col < Cols);
        return data[col * Rows + row];
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    const T& Matrix<RowsCount, ColsCount, T>::operator()(size_t row, size_t col) const noexcept
    {
        assert(row < Rows && col < Cols);
        return data[col * Rows + row];
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    constexpr Matrix<RowsCount, ColsCount, T> Matrix<RowsCount, ColsCount, T>::identity() noexcept
    {
        Matrix m;
        size_t n = (Rows < Cols) ? Rows : Cols;
        for (size_t i = 0; i < n; ++i) m(i, i) = static_cast<T>(1);
        return m;
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    constexpr Matrix<RowsCount, ColsCount, T> Matrix<RowsCount, ColsCount, T>::zero() noexcept
    {
        return Matrix();
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    constexpr Matrix<RowsCount, ColsCount, T> Matrix<RowsCount, ColsCount, T>::one() noexcept
    {
        Matrix m;
        for (size_t i = 0; i < Rows * Cols; ++i)
        {
            data[i] = static_cast<T>(1);
        }
        return m;
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    Matrix<RowsCount, ColsCount, T>& Matrix<RowsCount, ColsCount, T>::divAssign(T scalar) noexcept
    {
        if constexpr (std::is_floating_point_v<T>)
        {
            assert(Math<T>::abs(scalar) > Math<T>::Epsilon && "Division by zero!");

            T invScalar = static_cast<T>(1) / scalar;
            for (size_t i = 0; i < Rows * Cols; ++i)
            {
                data[i] *= invScalar;
            }
        }
        else
        {
            assert(scalar != static_cast<T>(0) && "Division by zero!");

            for (size_t i = 0; i < Rows * Cols; ++i)
            {
                data[i] /= scalar;
            }
        }

        return *this;
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    Matrix<RowsCount, ColsCount, T> Matrix<RowsCount, ColsCount, T>::div(T scalar) const noexcept
    {
        Matrix<Rows, Cols, T> result = *this;
        result.divAssign(scalar);
        return result;
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    Matrix<RowsCount, ColsCount, T> Matrix<RowsCount, ColsCount, T>::operator/(T scalar) const noexcept
    {
        return div(scalar);
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    Matrix<RowsCount, ColsCount, T> Matrix<RowsCount, ColsCount, T>::operator/=(T scalar) const noexcept
    {
        return divAssign(scalar);
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    Matrix<RowsCount, ColsCount, T>& Matrix<RowsCount, ColsCount, T>::mulAssign(T scalar) noexcept
    {
        for (size_t i = 0; i < Rows * Cols; ++i)
        {
            data[i] *= scalar;
        }
        return *this;
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    Matrix<RowsCount, ColsCount, T> Matrix<RowsCount, ColsCount, T>::mul(T scalar) const noexcept
    {
        Matrix<Rows, Cols, T> result = *this;
        result.mulAssign(scalar);
        return result;
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    Matrix<RowsCount, ColsCount, T> Matrix<RowsCount, ColsCount, T>::operator*(T scalar) const noexcept
    {
        return mul(scalar);
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    Matrix<RowsCount, ColsCount, T> operator*(T scalar, const Matrix<RowsCount, ColsCount, T>& m) noexcept
    {
        return m.mul(scalar);
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    Matrix<RowsCount, ColsCount, T> Matrix<RowsCount, ColsCount, T>::operator*=(T scalar) noexcept
    {
        return mulAssign(scalar);
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    Matrix<RowsCount, ColsCount, T>& Matrix<RowsCount, ColsCount, T>::mulAssign(const Matrix<RowsCount, ColsCount, T>& rhs) noexcept requires (RowsCount == ColsCount)
    {
        *this = (*this) * rhs;
        return *this;
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    template<size_t NewCols>
    Matrix<RowsCount, NewCols, T> Matrix<RowsCount, ColsCount, T>::mul(const Matrix<ColsCount, NewCols, T>& rhs) const noexcept
    {
        Matrix<Rows, NewCols, T> result;
        for (size_t r = 0; r < Rows; ++r)
        {
            for (size_t c = 0; c < NewCols; ++c)
            {
                T sum = 0;
                for (size_t k = 0; k < Cols; ++k)
                {
                    sum += (*this)(r, k) * rhs(k, c);
                }
                result(r, c) = sum;
            }
        }
        return result;
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    template<size_t NewCols>
    Matrix<RowsCount, NewCols, T> Matrix<RowsCount, ColsCount, T>::operator*(const Matrix<ColsCount, NewCols, T>& rhs) const noexcept
    {
        return mul(rhs);
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    Matrix<RowsCount, ColsCount, T>& Matrix<RowsCount, ColsCount, T>::operator*=(const Matrix<RowsCount, ColsCount, T>& rhs) noexcept requires (RowsCount == ColsCount)
    {
        return mulAssign(rhs);
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    Vector <T, RowsCount> Matrix<RowsCount, ColsCount, T>::mul(const Vector<T, ColsCount>& rhs) const noexcept
    {
        Vector<T, Rows> result;

        for (size_t r = 0; r < Rows; ++r)
        {
            T sum = 0;
            for (size_t c = 0; c < Cols; ++c)
            {
                sum += (*this)(r, c) * rhs[c];
            }
            result[r] = sum;
        }

        return result;
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    Vector <T, ColsCount> Matrix<RowsCount, ColsCount, T>::mulLeft(const Vector<T, RowsCount>& rhs) const noexcept
    {
        Vector<T, Cols> result;

        for (size_t c = 0; c < Cols; ++c)
        {
            T sum = 0;
            for (size_t r = 0; r < Rows; ++r)
            {
                sum += rhs[r] * (*this)(r, c);
            }
            result[c] = sum;
        }

        return result;
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    Vector<T, RowsCount> Matrix<RowsCount, ColsCount, T>::operator*(const Vector<T, ColsCount>& rhs) const noexcept
    {
        return mul(rhs);
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    Vector<T, ColsCount> operator*(const Vector<T, RowsCount>& lhs, const Matrix<RowsCount, ColsCount, T>& rhs) noexcept
    {
        return rhs.mulLeft(lhs);
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    Matrix<RowsCount, ColsCount, T>& Matrix<RowsCount, ColsCount, T>::addAssign(const Matrix<RowsCount, ColsCount, T>& rhs) noexcept
    {
        for (size_t i = 0; i < Rows * Cols; ++i)
        {
            (*this)[i] += rhs[i];
        }

        return *this;
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    Matrix<RowsCount, ColsCount, T> Matrix<RowsCount, ColsCount, T>::add(const Matrix<RowsCount, ColsCount, T>& rhs) const noexcept
    {
        Matrix<Rows, Cols, T> result = *this;
        return result.addAssign(rhs);
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    Matrix<RowsCount, ColsCount, T> Matrix<RowsCount, ColsCount, T>::operator+(const Matrix<RowsCount, ColsCount, T>& rhs) const noexcept
    {
        return add(rhs);
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    Matrix<RowsCount, ColsCount, T>& Matrix<RowsCount, ColsCount, T>::operator+=(const Matrix<RowsCount, ColsCount, T>& rhs) noexcept
    {
        return addAssign(rhs);
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    Matrix<RowsCount, ColsCount, T>& Matrix<RowsCount, ColsCount, T>::subAssign(const Matrix<RowsCount, ColsCount, T>& rhs) noexcept
    {
        for (size_t i = 0; i < Rows * Cols; ++i)
        {
            (*this)[i] -= rhs[i];
        }

        return *this;
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    Matrix<RowsCount, ColsCount, T> Matrix<RowsCount, ColsCount, T>::sub(const Matrix<RowsCount, ColsCount, T>& rhs) const noexcept
    {
        Matrix<Rows, Cols, T> result = *this;
        return result.subAssign(rhs);
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    Matrix<RowsCount, ColsCount, T> Matrix<RowsCount, ColsCount, T>::operator-(const Matrix<RowsCount, ColsCount, T>& rhs) const noexcept
    {
        return sub(rhs);
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    Matrix<RowsCount, ColsCount, T>& Matrix<RowsCount, ColsCount, T>::operator-=(const Matrix<RowsCount, ColsCount, T>& rhs) noexcept
    {
        return subAssign(rhs);
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    Matrix<RowsCount, ColsCount, T> Matrix<RowsCount, ColsCount, T>::negate() const noexcept
    {
        Matrix<Rows, Cols, T> result{};

        for (size_t i = 0; i < Rows * Cols; ++i)
        {
            result[i] = -(*this)[i];
        }

        return result;
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    Matrix<RowsCount, ColsCount, T> Matrix<RowsCount, ColsCount, T>::operator-() const noexcept
    {
        return negate();
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    Matrix<ColsCount, RowsCount, T> Matrix<RowsCount, ColsCount, T>::transpose() const noexcept
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

    template<size_t RowsCount, size_t ColsCount, typename T>
    bool Matrix<RowsCount, ColsCount, T>::isIdentity() const noexcept
    {
        return *this == identity();
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    T Matrix<RowsCount, ColsCount, T>::trace() const noexcept
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

    template<size_t RowsCount, size_t ColsCount, typename T>
    T Matrix<RowsCount, ColsCount, T>::determinant() const noexcept
    {
        static_assert(Rows == Cols, "Determinant can only be calculated for square matrices!");
        return calculateDeterminant(*this);
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    T Matrix<RowsCount, ColsCount, T>::minorValue(size_t row, size_t col) const noexcept
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

    template<size_t RowsCount, size_t ColsCount, typename T>
    T Matrix<RowsCount, ColsCount, T>::cofactor(size_t row, size_t col) const noexcept
    {
        T m = minorValue(row, col);
        return ((row + col) % 2 == 0) ? m : -m;
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    auto Matrix<RowsCount, ColsCount, T>::inverse() const noexcept
    {
        static_assert(Rows == Cols, "Only square matrices can be inverted!");

        using FPT = std::common_type_t<T, float>;

        FPT det = Math<FPT>::val(this->determinant());

        if (std::abs(det) < Math<FPT>::Epsilon)
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

    template<size_t RowsCount, size_t ColsCount, typename T>
    bool Matrix<RowsCount, ColsCount, T>::isEqual(const Matrix& rhs, const Matrix& lhs, T epsilon) noexcept
    {
        for (size_t i = 0; i < Rows * Cols; ++i)
        {
            if (!Math<T>::isEqual(lhs.data[i], rhs.data[i], epsilon)) return false;
        }

        return true;
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    bool Matrix<RowsCount, ColsCount, T>::equals(const Matrix& rhs, T epsilon) const noexcept
    {
        return isEqual(*this, rhs, epsilon);
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    bool Matrix<RowsCount, ColsCount, T>::operator==(const Matrix& rhs) const noexcept
    {
        return equals(rhs);
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    bool Matrix<RowsCount, ColsCount, T>::operator!=(const Matrix& rhs) const noexcept
    {
        return !equals(rhs);
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    std::string Matrix<RowsCount, ColsCount, T>::toString(int precision) const
    {
        std::ostringstream ss;

        ss << std::fixed << std::setprecision(precision);

        for (size_t r = 0; r < Rows; ++r)
        {
            ss << "| ";

            for (size_t c = 0; c < Cols; ++c)
            {
                ss << std::setw(precision + 4) << std::right << (*this)(r, c);
                if (c < Cols - 1) ss << "  ";
            }

            ss << " |" << "\n";
        }

        return ss.str();
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    std::ostream& operator<<(std::ostream& os, const Matrix<RowsCount, ColsCount, T>& m)
    {
        os << m.toString();
        return os;
    }

    template<size_t RowsCount, size_t ColsCount, typename T>
    template<size_t Dim>
    static T Matrix<RowsCount, ColsCount, T>::calculateDeterminant(const Matrix<Dim, Dim, T>& m) noexcept
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

                T term = m(0, c) * calculateDeterminant(sub);
                if (c % 2 == 1) det -= term;
                else det += term;
            }
            return det;
        }
    }
} // namespace ObsidianEngine::detail