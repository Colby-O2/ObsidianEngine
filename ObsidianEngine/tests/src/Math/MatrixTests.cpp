#include "doctest/doctest.h"

#include "helpers.h"
#include "ObsidianEngine/Math/Math.h"
#include <string>

using namespace ObsidianEngine::detail;

template<size_t R, size_t C, typename T, size_t... Is>
void test_static_factory_variadic(std::index_sequence<Is...>)
{
    using Mat = ObsidianEngine::detail::Matrix<R, C, T>;

    SUBCASE("Constructor Variadic (Column-Major Order)")
    {
        Mat m(static_cast<T>(Is)...);

        for (size_t i = 0; i < (R * C); ++i)
        {
            CHECK_CLOSE(m.data[i], static_cast<T>(i));
        }
    }

    SUBCASE("fromRows Variadic")
    {
        Mat m = Mat::fromRows(static_cast<T>(Is)...);

        size_t expected_val = 0;
        for (size_t r = 0; r < R; ++r)
        {
            for (size_t c = 0; c < C; ++c)
            {
                CHECK_CLOSE(m(r, c), static_cast<T>(expected_val++));
            }
        }
    }

    SUBCASE("fromColumns Variadic")
    {
        Mat m = Mat::fromColumns(static_cast<T>(Is)...);

        size_t expected_val = 0;
        for (size_t c = 0; c < C; ++c)
        {
            for (size_t r = 0; r < R; ++r)
            {
                CHECK_CLOSE(m(r, c), static_cast<T>(expected_val++));
            }
        }
    }
}

template<size_t R, size_t C, typename T>
void test_matrix_constructors()
{
    using Mat = ObsidianEngine::detail::Matrix<R, C, T>;
    using VecCol = ObsidianEngine::detail::Vector<T, R>;
    using VecRow = ObsidianEngine::detail::Vector<T, C>;

    SUBCASE("Default Constructor")
    {
        Mat m;
        CHECK_CLOSE_MATRIX(m, Mat::zero());
    }

    SUBCASE("Column-Major Initializer Vector Constructor")
    {
        VecCol col1;
        for (size_t i = 0; i < R; ++i) col1[i] = static_cast<T>(i + 1);

        Mat m({ col1 });

        CHECK_CLOSE_VECTOR(m[0], col1);

        if constexpr (C > 1)
        {
            for (size_t c = 1; c < C; ++c)
            {
                CHECK_CLOSE_VECTOR(m[c], VecCol::zero());
            }
        }
    }

    SUBCASE("Variadic Initializers")
    {
        test_static_factory_variadic<R, C, T>(std::make_index_sequence<R * C>{});
    }

    SUBCASE("fromRows Vector Initializer")
    {
        VecRow firstRow;
        for (size_t i = 0; i < C; ++i) firstRow[i] = static_cast<T>((i + 1) * 10);

        auto m = Mat::fromRows({ firstRow });

        CHECK_CLOSE_VECTOR(m.getRow(0), firstRow);

        if constexpr (R > 1)
        {
            CHECK_CLOSE_VECTOR(m.getRow(1), VecRow::zero());
        }
    }

    SUBCASE("fromColumns Vector Initializer")
    {
        VecCol firstCol;
        for (size_t i = 0; i < R; ++i) firstCol[i] = static_cast<T>((i + 1) * 5);

        auto m = Mat::fromColumns({ firstCol });

        CHECK_CLOSE_VECTOR(m[0], firstCol);
    }

    SUBCASE("Zero Matrix")
    {
        Mat m = Mat::zero();
        for (size_t i = 0; i < (R * C); ++i)
        {
            CHECK_CLOSE(m.data[i], T(0));
        }
    }

    SUBCASE("Identity Matrix")
    {
        Mat m = Mat::identity();
        for (size_t c = 0; c < C; ++c)
        {
            for (size_t r = 0; r < R; ++r)
            {
                T expected = (r == c) ? T(1) : T(0);
                CHECK_CLOSE(m(r, c), expected);
            }
        }
    }
}

template<size_t R, size_t C, typename T>
void test_matrix_accessors()
{
    using Mat = ObsidianEngine::detail::Matrix<R, C, T>;
    using VecCol = ObsidianEngine::detail::Vector<T, R>;
    using VecRow = ObsidianEngine::detail::Vector<T, C>;

    SUBCASE("Get and Set Column")
    {
        Mat m = Mat::zero();
        VecCol newCol;
        for (size_t i = 0; i < R; ++i) newCol[i] = static_cast<T>(i + 5);

        size_t targetCol = C - 1;
        m.setColumn(targetCol, newCol);

        VecCol retrieved = m.getColumn(targetCol);
        CHECK_CLOSE_VECTOR(retrieved, newCol);

        CHECK_CLOSE(m(0, targetCol), newCol[0]);

        if constexpr (C > 1) 
        {
            CHECK_CLOSE(m(0, 0), T(0));
        }
    }

    SUBCASE("Get and Set Row")
    {
        Mat m = Mat::zero();
        VecRow newRow;
        for (size_t i = 0; i < C; ++i) newRow[i] = static_cast<T>(i + 2);

        m.setRow(0, newRow);

        VecRow retrieved = m.getRow(0);
        CHECK_CLOSE_VECTOR(retrieved, newRow);

        CHECK_CLOSE(m(0, 0), newRow[0]);

        if constexpr (R > 1) 
        {
            CHECK_CLOSE(m(R - 1, 0), T(0));
        }
    }
}

template<size_t R, size_t C, typename T>
void test_matrix_slicing()
{
    using Mat = ObsidianEngine::detail::Matrix<R, C, T>;

    Mat m;
    for (size_t r = 0; r < R; ++r) {
        for (size_t c = 0; c < C; ++c) {
            m(r, c) = static_cast<T>(r * 10 + c);
        }
    }

    SUBCASE("Corner Slice (1x1)")
    {
        auto sub = m.template slice<1, 1>(0, 0);
        CHECK_CLOSE(sub(0, 0), m(0, 0));
    }

    if constexpr (R >= 2 && C >= 2)
    {
        SUBCASE("Sub-Matrix Slice (2x2)")
        {
            auto sub = m.template slice<2, 2>(0, 0);

            Matrix<2, 2, T> expected = Matrix<2, 2, T>::fromRows(
                m(0, 0), m(0, 1),
                m(1, 0), m(1, 1)
            );

            CHECK_CLOSE_MATRIX(sub, expected);
        }
    }

    if constexpr (R >= 2)
    {
        SUBCASE("Row Slice (1xCols)")
        {
            auto sub = m.template slice<1, C>(1, 0);

            Matrix<1, C, T> expected;
            for (size_t c = 0; c < C; ++c) expected(0, c) = m(1, c);

            CHECK_CLOSE_MATRIX(sub, expected);
        }
    }

    if constexpr (C >= 2)
    {
        SUBCASE("Column Slice (Rows x 1)")
        {
            auto sub = m.template slice<R, 1>(0, 1);

            ObsidianEngine::detail::Matrix<R, 1, T> expected;
            for (size_t r = 0; r < R; ++r) expected(r, 0) = m(r, 1);

            CHECK_CLOSE_MATRIX(sub, expected);
        }
    }
}

template<size_t R, size_t C, typename T>
void test_matrix_set_block()
{
    using Mat = ObsidianEngine::detail::Matrix<R, C, T>;

    SUBCASE("Set 1x1 Block")
    {
        Mat canvas = Mat::zero();
        Matrix<1, 1, T> block(static_cast<T>(99));

        canvas.template setBlock<1, 1>(block, R - 1, C - 1);

        CHECK_CLOSE(canvas(R - 1, C - 1), T(99));

        if constexpr (R > 1 || C > 1) 
        {
            CHECK_CLOSE(canvas(0, 0), T(0));
        }
    }

    if constexpr (R >= 2 && C >= 2)
    {
        SUBCASE("Set 2x2 Sub Matrix Block")
        {
            Mat canvas = Mat::zero();

            Matrix<2, 2, T> block(
                static_cast<T>(5), static_cast<T>(7),
                static_cast<T>(6), static_cast<T>(8)
            );

            canvas.template setBlock<2, 2>(block, 0, 0);

            auto placedPart = canvas.template slice<2, 2>(0, 0);
            CHECK_CLOSE_MATRIX(placedPart, block);

            if constexpr (R > 2 && C > 2) {
                CHECK_CLOSE(canvas(2, 2), T(0));
            }
        }
    }

    if constexpr (C >= 3)
    {
        SUBCASE("Set Column Strip Block (Rows x 1)")
        {
            Mat canvas = Mat::zero();
            ObsidianEngine::detail::Matrix<R, 1, T> strip;
            for (size_t r = 0; r < R; ++r) strip(r, 0) = static_cast<T>(r + 1);

            canvas.template setBlock<R, 1>(strip, 0, 1);

            auto placedCol = canvas.template slice<R, 1>(0, 1);
            CHECK_CLOSE_MATRIX(placedCol, strip);

            CHECK_CLOSE(canvas(0, 0), T(0));
        }
    }

    if constexpr (R >= 2)
    {
        SUBCASE("Set Row Strip Block (1 x Cols)")
        {
            Mat canvas = Mat::zero();

            ObsidianEngine::detail::Matrix<1, C, T> rowStrip;
            for (size_t c = 0; c < C; ++c)
            {
                rowStrip(0, c) = static_cast<T>((c + 1) * 7);
            }

            canvas.template setBlock<1, C>(rowStrip, 1, 0);

            auto placedRow = canvas.template slice<1, C>(1, 0);
            CHECK_CLOSE_MATRIX(placedRow, rowStrip);

            CHECK_CLOSE(canvas(0, 0), T(0));
        }
    }
}

template <size_t R, size_t C, typename T>
void test_matrix_indexing()
{
    using Mat = ObsidianEngine::detail::Matrix<R, C, T>;
    using Vec = ObsidianEngine::detail::Vector<T, R>;

    SUBCASE("Operator () Parentheses Indexing")
    {
        Mat m;
        Mat expected;

        for (size_t c = 0; c < C; ++c)
        {
            for (size_t r = 0; r < R; ++r)
            {
                T val = static_cast<T>(r + c * 10);
                m(r, c) = val;
                expected.data[c * R + r] = val; 
            }
        }

        CHECK_CLOSE_MATRIX(m, expected);

        const Mat& cm = m;
        CHECK_CLOSE(cm(0, 0), m(0, 0));
    }

    SUBCASE("Operator [] Column Access as Vector")
    {
        Mat m = Mat::zero();
        Vec expectedCol;
        for (size_t r = 0; r < R; ++r)
        {
            T val = static_cast<T>(r + 1);
            m(r, 0) = val;
            expectedCol[r] = val;
        }

        Vec& col0 = m[0];
        CHECK_CLOSE_VECTOR(col0, expectedCol);

        col0[0] = static_cast<T>(99);
        CHECK_CLOSE(m(0, 0), T(99));

        const Mat& cm = m;
        const Vec& cCol0 = cm[0];

        CHECK_CLOSE_VECTOR(cCol0, col0);
    }

    SUBCASE("Memory Layout Alignment")
    {
        Mat m = Mat::zero();
        if constexpr (C >= 2)
        {
            m(R - 1, 0) = T(123);
            m(0, 1) = T(456);

            CHECK_CLOSE(m.data[R - 1], T(123));
            CHECK_CLOSE(m.data[R], T(456));
        }
    }

    SUBCASE("Const Column Access and Alignment")
    {
        Mat m;
        for (size_t i = 0; i < R * C; ++i) m.data[i] = static_cast<T>(i);

        const Mat& cm = m;

        for (size_t c = 0; c < C; ++c)
        {
            const Vec& columnRef = cm[c];

            for (size_t r = 0; r < R; ++r)
            {
                CHECK_CLOSE(columnRef[r], m(r, c));
            }
        }
    }
}

template <typename T>
void test_matrix_multiplication()
{
    using namespace ObsidianEngine::detail;

    SUBCASE("Matrix * Matrix (2x3 * 3x2 -> 2x2)")
    {
        Matrix<2, 3, T> A = Matrix<2, 3, T>::fromRows(
            T(1), T(2), T(3),
            T(4), T(5), T(6)
        );

        Matrix<3, 2, T> B = Matrix<3, 2, T>::fromRows(
            T(7), T(8),
            T(9), T(10),
            T(11), T(12)
        );

        auto C = A * B;

        Matrix<2, 2, T> expected = Matrix<2, 2, T>::fromRows(
            T(58), T(64),
            T(139), T(154)
        );

        CHECK_CLOSE_MATRIX(C, expected);
    }

    SUBCASE("Matrix * Vector (3x2 * 2 -> 3)")
    {
        Matrix<3, 2, T> M = Matrix<3, 2, T>::fromRows(
            T(1), T(2),
            T(3), T(4),
            T(5), T(6)
        );
        Vector<T, 2> V({ T(10), T(20) });

        Vector<T, 3> R = M * V;
        Vector<T, 3> expected({ T(50), T(110), T(170) });

        CHECK_CLOSE_VECTOR(R, expected);
    }

    SUBCASE("Vector * Matrix (2 * 2x3 -> 3)")
    {
        Vector<T, 2> V({ T(1), T(2) });
        Matrix<2, 3, T> M = Matrix<2, 3, T>::fromRows(
            T(10), T(20), T(30),
            T(40), T(50), T(60)
        );

        Vector<T, 3> R = V * M;
        Vector<T, 3> expected({ T(90), T(120), T(150) });

        CHECK_CLOSE_VECTOR(R, expected);
    }

    SUBCASE("Multiplication by Identity")
    {
        Matrix<3, 3, T> A = Matrix<3, 3, T>::fromRows(
            T(1), T(2), T(3),
            T(4), T(5), T(6),
            T(7), T(8), T(9)
        );

        Matrix<3, 3, T> I = Matrix<3, 3, T>::identity();

        auto resultLeft = I * A;
        auto resultRight = A * I;

        CHECK_CLOSE_MATRIX(resultLeft, A);
        CHECK_CLOSE_MATRIX(resultRight, A);
    }
}

template <size_t R, size_t C, typename T>
void test_matrix_inverse()
{
    using Mat = ObsidianEngine::detail::Matrix<R, C, T>;

    SUBCASE("Determinant")
    {
        if constexpr (R == C)
        {
            Mat identity = Mat::identity();
            CHECK_CLOSE(identity.determinant(), T(1));

            Mat zero = Mat::zero();
            CHECK_CLOSE(zero.determinant(), T(0));

            if constexpr (R == 2)
            {
                Mat m = Mat::fromRows(
                    T(1), T(2),
                    T(3), T(4)
                );
                CHECK_CLOSE(m.determinant(), T(-2));
            }
            else if constexpr (R == 3)
            {
                Mat m = Mat::fromRows(
                    T(6), T(1), T(1),
                    T(4), T(-2), T(5),
                    T(2), T(8), T(7)
                );
                CHECK_CLOSE(m.determinant(), T(-306));
            }
            else if constexpr (R > 3)
            {
                Mat m = Mat::zero();
                T expected_det = 1;
                for (size_t i = 0; i < R; ++i)
                {
                    T val = T(i + 1);
                    m(i, i) = val;
                    expected_det *= val;
                }
                CHECK_CLOSE(m.determinant(), expected_det);
            }
        }
    }

    SUBCASE("Minors and Cofactors")
    {
        if constexpr (R == C && R >= 2)
        {
            Mat m;
            for (size_t i = 0; i < R * C; ++i) m.data[i] = static_cast<T>(i + 1);

            if constexpr (R == 2)
            {
                CHECK_CLOSE(m.minorValue(0, 0), T(4));
                CHECK_CLOSE(m.cofactor(0, 1), T(-2));
            }
            else if constexpr (R == 3)
            {
                CHECK_CLOSE(m.minorValue(0, 0), T(-3));
                CHECK_CLOSE(m.cofactor(0, 1), T(6));
            }
            else
            {
                Mat id = Mat::identity();

                CHECK_CLOSE(id.minorValue(0, 0), T(1));
                CHECK_CLOSE(id.minorValue(0, 1), T(0));
            }
        }
    }

    SUBCASE("Matrix Inverse")
    {
        if constexpr (R == C)
        {
            Mat id = Mat::identity();
            CHECK_CLOSE_MATRIX(id.inverse(), id);

            if constexpr (R == 2)
            {
                Mat m = Mat::fromRows(
                    T(4), T(7),
                    T(2), T(6)
                );
                Mat inv = m.inverse();

                CHECK_CLOSE(inv(0, 0), T(0.6));
                CHECK_CLOSE(inv(0, 1), T(-0.7));
                CHECK_CLOSE(inv(1, 0), T(-0.2));
                CHECK_CLOSE(inv(1, 1), T(0.4));
            }

            Mat m;
            for (size_t i = 0; i < R * C; ++i) m.data[i] = static_cast<T>(i * i + 1);

            if (std::abs(m.determinant()) > T(1e-6))
            {
                Mat inv = m.inverse();
                Mat result = m * inv;
                CHECK_CLOSE_MATRIX(result, Mat::identity());
            }
        }
    }

    SUBCASE("Singular Matrix")
    {
        if constexpr (R == C && R >= 2)
        {
            Mat m = Mat::zero();
            for (size_t c = 0; c < C; ++c) 
            {
                m(0, c) = static_cast<T>(c + 1);
                m(1, c) = static_cast<T>(c + 1) * 2;
            }

            CHECK(std::abs(m.determinant()) < T(1e-7));
            CHECK_CLOSE_MATRIX(m.inverse(), Mat::identity());
        }
    }
}

template <size_t R, size_t C, typename T>
void test_matrix_utilities()
{
    using Mat = ObsidianEngine::detail::Matrix<R, C, T>;

    SUBCASE("Transpose")
    {
        Mat m;
        for (size_t r = 0; r < R; ++r) {
            for (size_t c = 0; c < C; ++c) {
                m(r, c) = static_cast<T>(r * 10 + c);
            }
        }

        auto t = m.transpose();

        static_assert(decltype(t)::Rows == C && decltype(t)::Cols == R, "Transpose dimensions must be swapped!");

        for (size_t r = 0; r < R; ++r) 
        {
            for (size_t c = 0; c < C; ++c)
            {
                CHECK_CLOSE(t(c, r), m(r, c));
            }
        }
    }

    SUBCASE("isIdentity")
    {
        Mat i = Mat::identity();
        Mat z = Mat::zero();

        CHECK(i.isIdentity() == true);
        CHECK(z.isIdentity() == false);

        i(0, 0) = T(2);
        CHECK(i.isIdentity() == false);
    }

    SUBCASE("Trace")
    {
        if constexpr (R == C)
        {
            Mat m;
            T expected_diagonal_sum = 0;

            for (size_t r = 0; r < R; ++r)
            {
                for (size_t c = 0; c < C; ++c)
                {
                    T val = static_cast<T>((r + 1) * (c + 1));
                    m(r, c) = val;

                    if (r == c)
                    {
                        expected_diagonal_sum += val;
                    }
                }
            }

            CHECK_CLOSE(m.trace(), expected_diagonal_sum);
        }
    }

    SUBCASE("Equality Operators")
    {
        Mat m1 = Mat::zero();
        Mat m2 = Mat::zero();

        CHECK(m1 == m2);
        CHECK_FALSE(m1 != m2);

        m2.data[0] = static_cast<T>(2);;
        CHECK(m1 != m2);
        CHECK_FALSE(m1 == m2);

        CHECK(m1 == m1);

        Mat id1 = Mat::identity();
        Mat id2 = Mat::identity();
        CHECK(id1 == id2);

        id2(0, 0) = static_cast<T>(0);
        CHECK(id1 != id2);
    }
}

template<typename T>
void test_matrix_vector_conversion()
{
    using namespace ObsidianEngine::detail;

    SUBCASE("Column Matrix to Vector (Nx1)")
    {
        using MatCol = Matrix<3, 1, T>;
        MatCol m(static_cast<T>(10), static_cast<T>(20), static_cast<T>(30));

        auto v = m.asVector();

        CHECK_CLOSE_VECTOR(v, (Vector<T, 3>{static_cast<T>(10), static_cast<T>(20), static_cast<T>(30)}));

        static_assert(std::is_same_v<decltype(v), Vector<T, 3>>, "asVector should return Vector<T, 3>");
    }

    SUBCASE("Row Matrix to Vector (1xM)")
    {
        using MatRow = Matrix<1, 4, T>;
        auto m = MatRow::fromRows(static_cast<T>(1), static_cast<T>(2), static_cast<T>(3), static_cast<T>(4));

        auto v = m.asVector();

        CHECK_CLOSE_VECTOR(v, (Vector<T, 4>{static_cast<T>(1), static_cast<T>(2), static_cast<T>(3), static_cast<T>(4)}));

        static_assert(std::is_same_v<decltype(v), Vector<T, 4>>, "asVector should return Vector<T, 4>");
    }

    SUBCASE("Single Element (1x1)")
    {
        using MatOne = Matrix<1, 1, T>;
        MatOne m(static_cast<T>(5));

        auto v = m.asVector();

        CHECK_CLOSE(v[0], static_cast<T>(5));

        static_assert(std::is_same_v<decltype(v), Vector<T, 1>>, "asVector should return Vector<T, 1>");
    }

    SUBCASE("Explicit 1x1 Matrix to Vector")
    {
        using MatOne = Matrix<1, 1, T>;
        MatOne m(static_cast<T>(42));

        auto v = m.asVector();

        static_assert(std::is_same_v<decltype(v), Vector<T, 1>>);
        CHECK_CLOSE(v[0], T(42));
    }
}

TEST_SUITE("Math")
{
    TEST_SUITE("MatrixNxM")
    {
        TEST_CASE_TEMPLATE("Constructors", T, float, double, int)
        {
            SUBCASE("Matrix 2x2") { test_matrix_constructors<2, 2, T>(); }
            SUBCASE("Matrix 3x3") { test_matrix_constructors<3, 3, T>(); }
            SUBCASE("Matrix 4x4") { test_matrix_constructors<4, 4, T>(); }

            SUBCASE("Matrix 3x4") { test_matrix_constructors<3, 4, T>(); }
            SUBCASE("Matrix 4x3") { test_matrix_constructors<4, 3, T>(); }
            SUBCASE("Matrix 4x3") { test_matrix_constructors<2, 3, T>(); }
        }
        TEST_CASE_TEMPLATE("Matrix as Vector Conversion", T, float, double, int)
        {
            test_matrix_vector_conversion<T>();
        }
        TEST_CASE_TEMPLATE("Row and Column Manipulation", T, float, double, int)
        {
            SUBCASE("Matrix 2x2") { test_matrix_accessors<2, 2, T>(); }
            SUBCASE("Matrix 3x3") { test_matrix_accessors<3, 3, T>(); }
            SUBCASE("Matrix 4x4") { test_matrix_accessors<4, 4, T>(); }

            SUBCASE("Matrix 3x4") { test_matrix_accessors<3, 4, T>(); }
            SUBCASE("Matrix 4x3") { test_matrix_accessors<4, 3, T>(); }
            SUBCASE("Matrix 4x3") { test_matrix_accessors<2, 3, T>(); }

            SUBCASE("Matrix 1x4") { test_matrix_accessors<1, 4, T>(); }
            SUBCASE("Matrix 4x1") { test_matrix_accessors<4, 1, T>(); }
        }
        TEST_CASE_TEMPLATE("Matrix Slicing Logic", T, float, double, int)
        {
            SUBCASE("Matrix 2x2") { test_matrix_slicing<2, 2, T>(); }
            SUBCASE("Matrix 3x3") { test_matrix_slicing<3, 3, T>(); }
            SUBCASE("Matrix 4x4") { test_matrix_slicing<4, 4, T>(); }

            SUBCASE("Matrix 3x4") { test_matrix_slicing<3, 4, T>(); }
            SUBCASE("Matrix 4x3") { test_matrix_slicing<4, 3, T>(); }
            SUBCASE("Matrix 4x3") { test_matrix_slicing<2, 3, T>(); }

            SUBCASE("Matrix 1x4") { test_matrix_accessors<1, 4, T>(); }
            SUBCASE("Matrix 4x1") { test_matrix_accessors<4, 1, T>(); }
        }
        TEST_CASE_TEMPLATE("Matrix Block Modification", T, float, double, int)
        {
            SUBCASE("Matrix 2x2") { test_matrix_set_block<2, 2, T>(); }
            SUBCASE("Matrix 3x3") { test_matrix_set_block<3, 3, T>(); }
            SUBCASE("Matrix 4x4") { test_matrix_set_block<4, 4, T>(); }

            SUBCASE("Matrix 3x4") { test_matrix_set_block<3, 4, T>(); }
            SUBCASE("Matrix 4x3") { test_matrix_set_block<4, 3, T>(); }
            SUBCASE("Matrix 4x3") { test_matrix_set_block<2, 3, T>(); }

            SUBCASE("Matrix 1x4") { test_matrix_set_block<1, 4, T>(); }
            SUBCASE("Matrix 4x1") { test_matrix_set_block<4, 1, T>(); }
        }
        TEST_CASE_TEMPLATE("Matrix Indexing and Storage", T, float, double, int)
        {
            SUBCASE("Matrix 2x2") { test_matrix_indexing<2, 2, T>(); }
            SUBCASE("Matrix 3x3") { test_matrix_indexing<3, 3, T>(); }
            SUBCASE("Matrix 4x4") { test_matrix_indexing<4, 4, T>(); }

            SUBCASE("Matrix 3x4") { test_matrix_indexing<3, 4, T>(); }
            SUBCASE("Matrix 4x3") { test_matrix_indexing<4, 3, T>(); }
            SUBCASE("Matrix 4x3") { test_matrix_indexing<2, 3, T>(); }

            SUBCASE("Matrix 1x4") { test_matrix_indexing<1, 4, T>(); }
            SUBCASE("Matrix 4x1") { test_matrix_indexing<4, 1, T>(); }
        }
        TEST_CASE_TEMPLATE("Matrix Multiplication", T, float, double, int)
        {
            test_matrix_multiplication<T>();
        }
        TEST_CASE_TEMPLATE("Matrix Inverse", T, float, double) /*int*/
        {
            SUBCASE("Matrix 2x2") { test_matrix_inverse<2, 2, T>(); }
            SUBCASE("Matrix 3x3") { test_matrix_inverse<3, 3, T>(); }
            SUBCASE("Matrix 4x4") { test_matrix_inverse<4, 4, T>(); }
        }
        TEST_CASE_TEMPLATE("Matrix Utilities", T, float, double, int)
        {
            SUBCASE("Matrix 2x2") { test_matrix_utilities<2, 2, T>(); }
            SUBCASE("Matrix 3x3") { test_matrix_utilities<3, 3, T>(); }
            SUBCASE("Matrix 4x4") { test_matrix_utilities<4, 4, T>(); }

            SUBCASE("Matrix 3x4") { test_matrix_utilities<3, 4, T>(); }
            SUBCASE("Matrix 4x3") { test_matrix_utilities<4, 3, T>(); }
            SUBCASE("Matrix 4x3") { test_matrix_utilities<2, 3, T>(); }

            SUBCASE("Matrix 1x4") { test_matrix_utilities<1, 4, T>(); }
            SUBCASE("Matrix 4x1") { test_matrix_utilities<4, 1, T>(); }
        }
    }
}