#include "doctest/doctest.h"


#include "helpers.h"
#include "ObsidianEngine/Math/Math.h"
#include <string>

using namespace ObsidianEngine::detail;

template<size_t N, typename T, size_t... Is>
void test_vector_variadic_constructor(std::index_sequence<Is...>)
{
    SUBCASE("Variadic Constructor")
    {
        Vector<T, N> v(static_cast<T>(Is)...);
        for (size_t i = 0; i < N; ++i)
        {
            CHECK_CLOSE(v[i], static_cast<T>(i));
        }
    }
}

template<size_t N, typename T>
void test_vector_constructors()
{
    SUBCASE("Default Constructor")
    {
        Vector<T, N> v;
        for (size_t i = 0; i < N; ++i) CHECK_CLOSE(v[i], static_cast<T>(0));
    }

    SUBCASE("Zero")
    {
        Vector<T, N> v = Vector<T, N>::zero();
        for (size_t i = 0; i < N; ++i) CHECK_CLOSE(v[i], static_cast<T>(0));
    }

    SUBCASE("One")
    {
        Vector<T, N> v = Vector<T, N>::one();
        for (size_t i = 0; i < N; ++i) CHECK_CLOSE(v[i], static_cast<T>(1));
    }

    if constexpr (N <= 4)
    {
        SUBCASE("Variadic Initialization")
        {
            test_vector_variadic_constructor<N, T>(std::make_index_sequence<N>{});
        }
    }
}

template<size_t N, typename T>
void test_vector_swizzling()
{
    Vector<T, N> v;
    for (size_t i = 0; i < N; ++i) v[i] = static_cast<T>(i + 1);

    if constexpr (N >= 2)
    {
        SUBCASE("Swizzle Reading")
        {
            Vector<T, 2> v2 = v.template swizzle<"yx">();
            CHECK_CLOSE(v2.x, v[1]);
            CHECK_CLOSE(v2.y, v[0]);

            Vector<T, 2> rg = v.template swizzle<"rg">();
            CHECK_CLOSE(rg[0], v[0]);
            CHECK_CLOSE(rg[1], v[1]);

            if constexpr (N >= 3)
            {
                Vector<T, 3> v3 = v.template swizzle<"zzx">();
                CHECK_CLOSE(v3.x, v.z);
                CHECK_CLOSE(v3.y, v.z);
                CHECK_CLOSE(v3.z, v.x);
            }

            if constexpr (N >= 4)
            {
                Vector<T, 4> v4 = v.template swizzle<"wzyx">();
                CHECK_CLOSE(v4.x, v.w);
                CHECK_CLOSE(v4.y, v.z);
                CHECK_CLOSE(v4.z, v.y);
                CHECK_CLOSE(v4.w, v.x);
            }
        }
        SUBCASE("Swizzle Assignment")
        {
            Vector<T, N> testV = Vector<T, N>::zero();

            testV.template swizzle<"xy">() = Vector<T, 2>(static_cast<T>(10), static_cast<T>(20));
            CHECK_CLOSE(testV[0], static_cast<T>(10));
            CHECK_CLOSE(testV[1], static_cast<T>(20));

            if constexpr (N >= 3)
            {
                testV.template swizzle<"zxy">() = Vector<T, 3>(static_cast<T>(100), static_cast<T>(200), static_cast<T>(300));
                CHECK_CLOSE(testV.x, static_cast<T>(200));
                CHECK_CLOSE(testV.y, static_cast<T>(300));
                CHECK_CLOSE(testV.z, static_cast<T>(100));
            }
        }
        SUBCASE("Arithmetic Between Proxy and Vector")
        {
            Vector<T, 2> other(static_cast<T>(10), static_cast<T>(20));

            auto res1 = v.template swizzle<"xy">() + other;
            CHECK_CLOSE(res1.x, v[0] + static_cast<T>(10));
            CHECK_CLOSE(res1.y, v[1] + static_cast<T>(20));

            auto res2 = other - v.template swizzle<"yx">();
            CHECK_CLOSE(res2.x, static_cast<T>(10) - v[1]);
            CHECK_CLOSE(res2.y, static_cast<T>(20) - v[0]);

            if constexpr (N >= 3)
            {
                Vector<T, 3> other3(static_cast<T>(5), static_cast<T>(5), static_cast<T>(5));
                auto res3 = v.template swizzle<"xyz">() * other3;
                CHECK_CLOSE(res3.x, v.x * static_cast<T>(5));
                CHECK_CLOSE(res3.z, v.z * static_cast<T>(5));
            }
        }
        SUBCASE("Arithmetic between Proxy and Proxy")
        {
            auto res = v.template swizzle<"xy">() + v.template swizzle<"yx">();

            CHECK_CLOSE(res.x, v[0] + v[1]);
            CHECK_CLOSE(res.y, v[1] + v[0]);

            if constexpr (N >= 4)
            {
                Vector<T, 4> combined = v.template swizzle<"xxxx">() + v.template swizzle<"xyzw">();
                CHECK_CLOSE(combined.x, v.x + v.x);
                CHECK_CLOSE(combined.w, v.x + v.w);
            }
        }
        SUBCASE("Unary and Compounding")
        {
            Vector<T, 2> negated = -v.template swizzle<"xy">();
            CHECK_CLOSE(negated.x, -v[0]);

            Vector<T, 2> complex = (v.template swizzle<"xy">() + v.template swizzle<"yx">()) * -v.template swizzle<"xx">();
            CHECK_CLOSE(complex.x, -(v[0] + v[1]) * v[0]);
            CHECK_CLOSE(complex.y, -(v[1] + v[0]) * v[0]);
        }
        SUBCASE("Cross Vector Swizzle Interaction")
        {
            Vector<T, N> vOther;
            for (size_t i = 0; i < N; ++i) vOther[i] = static_cast<T>(10);

            Vector<T, 2> res = v.template swizzle<"xy">() + vOther.template swizzle<"yx">();
            CHECK_CLOSE(res.x, v[0] + vOther[1]);
            CHECK_CLOSE(res.y, v[1] + vOther[0]);
        }
        SUBCASE("Cross Dimension Arithmetic")
        {
            Vector<T, 2> v2(static_cast<T>(1), static_cast<T>(2));
            Vector<T, 4> v4(static_cast<T>(10), static_cast<T>(20), static_cast<T>(30), static_cast<T>(40));

            auto res1 = v2.template swizzle<"xxxx">() + v4;

            static_assert(decltype(res1)::Size == 4, "Result must be Vector4");
            CHECK_CLOSE(res1.x, static_cast<T>(11));
            CHECK_CLOSE(res1.y, static_cast<T>(21));
            CHECK_CLOSE(res1.z, static_cast<T>(31));
            CHECK_CLOSE(res1.w, static_cast<T>(41));

            auto res2 = v4.template swizzle<"xw">() + v2;

            static_assert(decltype(res2)::Size == 2, "Result must be Vector2");
            CHECK_CLOSE(res2.x, static_cast<T>(11));
            CHECK_CLOSE(res2.y, static_cast<T>(42));
        }

        SUBCASE("Swizzle Logic")
        {
            Vector<T, 4> v4(static_cast<T>(1), static_cast<T>(2), static_cast<T>(3), static_cast<T>(4));

            Vector<T, 4> result = v4.template swizzle<"wx">().eval().swizzle<"xxxx">();

            CHECK_CLOSE(result.x, static_cast<T>(4));
            CHECK_CLOSE(result.y, static_cast<T>(4));
            CHECK_CLOSE(result.z, static_cast<T>(4));
            CHECK_CLOSE(result.w, static_cast<T>(4));
        }
        SUBCASE("Deep Swizzle Chaining")
        {
            Vector<float, 4> v4(1.0f, 2.0f, 3.0f, 4.0f);

            Vector<float, 4> result = v4.template swizzle<"wzyx">().template swizzle<"x">().template swizzle<"xxxx">();

            CHECK_CLOSE(result.x, 4.0f);
            CHECK_CLOSE(result.y, 4.0f);
            CHECK_CLOSE(result.z, 4.0f);
            CHECK_CLOSE(result.w, 4.0f);
        }
        SUBCASE("Deep Swizzle Assignment")
        {
            Vector<float, 4> v4(1.0f, 2.0f, 3.0f, 4.0f);
            v4.template swizzle<"wzyx">().template swizzle<"x">() = { 99.0f };
            CHECK_CLOSE(v4.w, 99.0f);
            CHECK_CLOSE(v4.x, 1.0f);
        }
    }
}

template<size_t N, typename T>
void test_vector_set()
{
    SUBCASE("Basic Set")
    {
        Vector<T, N> v = Vector<T, N>::zero();
        Vector<T, N> v2 = v.template set<"x">(static_cast<T>(5));
        CHECK_CLOSE(v2[0], static_cast<T>(5));
        CHECK_CLOSE(v[0], static_cast<T>(0));
    }

    if constexpr (N >= 2)
    {
        SUBCASE("Multiple Components")
        {
            Vector<T, N> v = Vector<T, N>::zero();

            Vector<T, N> v2 = v.template set<"yx">(static_cast<T>(10), static_cast<T>(20));
            CHECK_CLOSE(v2.x, static_cast<T>(20));
            CHECK_CLOSE(v2.y, static_cast<T>(10));

            Vector<T, N> v3 = v.template set<"rg">(static_cast<T>(30), static_cast<T>(40));
            CHECK_CLOSE(v3[0], static_cast<T>(30));
            CHECK_CLOSE(v3[1], static_cast<T>(40));
        }
    }

    if constexpr (N >= 3)
    {
        SUBCASE("Partial Set 3D")
        {
            Vector<T, N> v = Vector<T, N>::one();
            Vector<T, N> v2 = v.template set<"z">(static_cast<T>(99));
            CHECK_CLOSE(v2.x, static_cast<T>(1));
            CHECK_CLOSE(v2.y, static_cast<T>(1));
            CHECK_CLOSE(v2.z, static_cast<T>(99));
        }
    }

    if constexpr (N >= 4)
    {
        SUBCASE("Complex Set 4D")
        {
            Vector<T, N> v = Vector<T, N>::zero();
            Vector<T, N> v2 = v.template set<"xa">(static_cast<T>(7), static_cast<T>(8));
            CHECK_CLOSE(v2.x, static_cast<T>(7));
            CHECK_CLOSE(v2.y, static_cast<T>(0));
            CHECK_CLOSE(v2.z, static_cast<T>(0));
            CHECK_CLOSE(v2.w, static_cast<T>(8));
        }
    }
}

template<size_t N, typename T>
void test_vector_operators()
{
    Vector<T, N> v1 = Vector<T, N>::one();                       
    Vector<T, N> v2 = Vector<T, N>::one() * static_cast<T>(2);    

    SUBCASE("Subscript Operator")
    {
        Vector<T, N> v = Vector<T, N>::zero();
        v[0] = static_cast<T>(42);
        CHECK_CLOSE(v[0], static_cast<T>(42));

        const Vector<T, N> cv = v;
        CHECK_CLOSE(cv[0], static_cast<T>(42));
    }
    SUBCASE("Arithmetic")
    {
        Vector<T, N> resAdd = v1 + v2;
        for (size_t i = 0; i < N; ++i) CHECK_CLOSE(resAdd[i], static_cast<T>(3));

        Vector<T, N> resSub = v2 - v1;
        for (size_t i = 0; i < N; ++i) CHECK_CLOSE(resSub[i], static_cast<T>(1));

        Vector<T, N> resMul = v1 * static_cast<T>(5);
        for (size_t i = 0; i < N; ++i) CHECK_CLOSE(resMul[i], static_cast<T>(5));

        Vector<T, N> resMulLeft = static_cast<T>(10) * v1;
        for (size_t i = 0; i < N; ++i) CHECK_CLOSE(resMulLeft[i], static_cast<T>(10));

        Vector<T, N> resScale = v2 * v2;
        for (size_t i = 0; i < N; ++i) CHECK_CLOSE(resScale[i], static_cast<T>(4));

        Vector<T, N> resDiv = v2 / static_cast<T>(2);
        for (size_t i = 0; i < N; ++i) CHECK_CLOSE(resDiv[i], static_cast<T>(1));
    }
    SUBCASE("Unary Operators")
    {
        Vector<T, N> negated = -v1;
        for (size_t i = 0; i < N; ++i) CHECK_CLOSE(negated[i], static_cast<T>(-1));
    }
    SUBCASE("Assignment Operators")
    {
        Vector<T, N> testV = Vector<T, N>::one();

        testV += v1;
        CHECK_CLOSE_VECTOR(testV, v2);

        testV -= v1;
        CHECK_CLOSE_VECTOR(testV, v1);

        testV *= static_cast<T>(10);
        for (size_t i = 0; i < N; ++i) CHECK_CLOSE(testV[i], static_cast<T>(10));

        testV *= v2;
        for (size_t i = 0; i < N; ++i) CHECK_CLOSE(testV[i], static_cast<T>(20));

        testV /= static_cast<T>(4);
        for (size_t i = 0; i < N; ++i) CHECK_CLOSE(testV[i], static_cast<T>(5));
    }
}

template<size_t N, typename T>
void test_vector_comparison()
{
    SUBCASE("Equality and Inequality")
    {
        Vector<T, N> v1 = Vector<T, N>::one();
        Vector<T, N> v2 = Vector<T, N>::one();
        Vector<T, N> v3 = Vector<T, N>::zero();

        CHECK(v1 == v2);
        CHECK(v1.equals(v2));

        CHECK(v1 != v3);
        CHECK_FALSE(v1 == v3);
    }

    SUBCASE("Epsilon Tolerance")
    {
        if constexpr (std::is_floating_point_v<T>)
        {
            Vector<T, N> v1 = Vector<T, N>::zero();
            Vector<T, N> v2 = Vector<T, N>::zero();

            T smallNudge = static_cast<T>(1e-9);
            v2[0] = smallNudge;

            CHECK(v1 == v2);
            CHECK(v1.equals(v2));

            v2[0] = static_cast<T>(1.0);
            CHECK(v1 != v2);
            CHECK_FALSE(v1.equals(v2));
        }
        else
        {
            Vector<T, N> v1 = Vector<T, N>::zero();
            Vector<T, N> v2 = Vector<T, N>::zero();
            v2[0] = 1;

            CHECK_FALSE(v1 == v2);
            CHECK(v1 != v2);
        }
    }
}

template<size_t N, typename T>
void test_vector_matrix_conversion()
{
    Vector<T, N> v;
    for (size_t i = 0; i < N; ++i) v[i] = static_cast<T>(i + 1);

    SUBCASE("As Column Matrix")
    {
        auto colMat = v.asColumnMatrix();

        static_assert(decltype(colMat)::Rows == N, "Matrix rows must match Vector size!");
        static_assert(decltype(colMat)::Cols == 1, "Matrix must have 1 column!");

        for (size_t i = 0; i < N; ++i)
        {
            CHECK_CLOSE(colMat(i, 0), v[i]);
        }
    }
    SUBCASE("As Row Matrix")
    {
        auto rowMat = v.asRowMatrix();

        static_assert(decltype(rowMat)::Rows == 1, "Matrix must have 1 row!");
        static_assert(decltype(rowMat)::Cols == N, "Matrix columns must match Vector size!");

        for (size_t i = 0; i < N; ++i)
        {
            CHECK_CLOSE(rowMat(0, i), v[i]);
        }
    }
}

template<size_t N, typename T>
void test_vector_math_ops()
{

}

TEST_SUITE("Math")
{
    TEST_SUITE("Vector<T, N>")
    {
        TEST_CASE_TEMPLATE("Constructors and Constants", T, float, double, int)
        {
            SUBCASE("Vector2") { test_vector_constructors<1, T>(); }
            SUBCASE("Vector2") { test_vector_constructors<2, T>(); }
            SUBCASE("Vector3") { test_vector_constructors<3, T>(); }
            SUBCASE("Vector4") { test_vector_constructors<4, T>(); }
        }
        TEST_CASE_TEMPLATE("Vector Swizzling", T, float, double, int)
        {
            SUBCASE("Vector2") { test_vector_swizzling<2, T>(); }
            SUBCASE("Vector3") { test_vector_swizzling<3, T>(); }
            SUBCASE("Vector4") { test_vector_swizzling<4, T>(); }
        }
        TEST_CASE_TEMPLATE("Vector Set", T, float, double, int)
        {
            SUBCASE("Vector2") { test_vector_set<2, T>(); }
            SUBCASE("Vector3") { test_vector_set<3, T>(); }
            SUBCASE("Vector4") { test_vector_set<4, T>(); }
        }
        TEST_CASE_TEMPLATE("Vector Operators Overloads", T, float, double, int)
        {
            SUBCASE("Vector2") { test_vector_operators<2, T>(); }
            SUBCASE("Vector3") { test_vector_operators<3, T>(); }
            SUBCASE("Vector4") { test_vector_operators<4, T>(); }
        }
        TEST_CASE_TEMPLATE("Vector Comparison", T, float, double, int)
        {
            SUBCASE("Vector2") { test_vector_comparison<2, T>(); }
            SUBCASE("Vector3") { test_vector_comparison<3, T>(); }
            SUBCASE("Vector4") { test_vector_comparison<4, T>(); }
        }
        TEST_CASE_TEMPLATE("Matrix Conversions", T, float, double, int)
        {
            SUBCASE("Vector2") { test_vector_matrix_conversion<2, T>(); }
            SUBCASE("Vector3") { test_vector_matrix_conversion<3, T>(); }
            SUBCASE("Vector4") { test_vector_matrix_conversion<4, T>(); }
        }
        TEST_CASE_TEMPLATE("Math Operations", T, float, double, int)
        {
            SUBCASE("Vector2") { test_vector_math_ops<2, T>(); }
            SUBCASE("Vector3") { test_vector_math_ops<3, T>(); }
            SUBCASE("Vector4") { test_vector_math_ops<4, T>(); }
        }
    }
    TEST_SUITE("Vector<T, 2>")
    {
        TEST_CASE_TEMPLATE("Constants", T, float, double, int)
        {
            SUBCASE("Up")
            {
                CHECK_CLOSE_VECTOR(ObsidianEngine::Vector2::up(), ObsidianEngine::Vector2(0, 1));
            }
            SUBCASE("Down")
            {
                CHECK_CLOSE_VECTOR(ObsidianEngine::Vector2::down(), ObsidianEngine::Vector2(0, -1));
            }
            SUBCASE("Left")
            {
                CHECK_CLOSE_VECTOR(ObsidianEngine::Vector2::left(), ObsidianEngine::Vector2(-1, 0));
            }
            SUBCASE("Right")
            {
                CHECK_CLOSE_VECTOR(ObsidianEngine::Vector2::right(), ObsidianEngine::Vector2(1, 0));
            }
        }
    }
    TEST_SUITE("Vector<T, 3>")
    {
        TEST_CASE_TEMPLATE("Constants", T, float, double, int)
        {
            SUBCASE("Up")
            {
                CHECK_CLOSE_VECTOR(ObsidianEngine::Vector3::up(), ObsidianEngine::Vector3(0, 1, 0));
            }
            SUBCASE("Down")
            {
                CHECK_CLOSE_VECTOR(ObsidianEngine::Vector3::down(), ObsidianEngine::Vector3(0, -1, 0));
            }
            SUBCASE("Left")
            {
                CHECK_CLOSE_VECTOR(ObsidianEngine::Vector3::left(), ObsidianEngine::Vector3(-1, 0, 0));
            }
            SUBCASE("Right")
            {
                CHECK_CLOSE_VECTOR(ObsidianEngine::Vector3::right(), ObsidianEngine::Vector3(1, 0, 0));
            }
            SUBCASE("Forward")
            {
                CHECK_CLOSE_VECTOR(ObsidianEngine::Vector3::forward(), ObsidianEngine::Vector3(0, 0, 1));
            }
            SUBCASE("Back")
            {
                CHECK_CLOSE_VECTOR(ObsidianEngine::Vector3::back(), ObsidianEngine::Vector3(0, 0, -1));
            }
        }
        TEST_CASE_TEMPLATE("Vector<T, 3>::cross", T, float, double, int)
        {
            SUBCASE("Standard Basis Vectors")
            {
                ObsidianEngine::Vector3 x = ObsidianEngine::Vector3::right();
                ObsidianEngine::Vector3 y = ObsidianEngine::Vector3::up();
                ObsidianEngine::Vector3 z = ObsidianEngine::Vector3::forward();

                CHECK_CLOSE_VECTOR(ObsidianEngine::Vector3::cross(x, y), z);
                CHECK_CLOSE_VECTOR(y.cross(z), x);
                CHECK_CLOSE_VECTOR(z.cross(x), y);
            }
            SUBCASE("Anti Commutative Property")
            {
                ObsidianEngine::Vector3 a(1, 2, 3);
                ObsidianEngine::Vector3 b(4, 5, 6);

                ObsidianEngine::Vector3 crossAB = ObsidianEngine::Vector3::cross(a, b);
                ObsidianEngine::Vector3 crossBA = ObsidianEngine::Vector3::cross(b, a);

                CHECK_CLOSE_VECTOR(crossAB, -crossBA);
            }
            SUBCASE("Parallel Vectors")
            {
                ObsidianEngine::Vector3 a(1, 2, 3);
                ObsidianEngine::Vector3 b(2, 4, 6);

                ObsidianEngine::Vector3 result = ObsidianEngine::Vector3::cross(a, b);
                CHECK_CLOSE_VECTOR(result, ObsidianEngine::Vector3::zero());
            }
            SUBCASE("Orthogonality Property")
            {
                ObsidianEngine::Vector3 a(0.5f, 2.3f, -1.1f);
                ObsidianEngine::Vector3 b(1.0f, 0.0f, 4.5f);
                ObsidianEngine::Vector3 result = a.cross(b);

                CHECK_CLOSE(ObsidianEngine::Vector3::dot(result, a), static_cast<T>(0));
                CHECK_CLOSE(ObsidianEngine::Vector3::dot(result, b), static_cast<T>(0));
            }
        }
    }
    TEST_SUITE("Vector<T, 4>")
    {
        TEST_CASE_TEMPLATE("Constants", T, float, double, int)
        {
            SUBCASE("Back")
            {
                CHECK_CLOSE_VECTOR(ObsidianEngine::Vector4::identity(), ObsidianEngine::Vector4(0, 0, 0, 1));
            }
        }
    }
}