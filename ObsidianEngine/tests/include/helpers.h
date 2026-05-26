#ifndef  __OBSIDIANENGINE_TESTS_HELPERS_H__
#define  __OBSIDIANENGINE_MATH_QUATERNION_HELPERS_H__

#include "doctest/doctest.h"
#include "ObsidianEngine/Math/Math.hpp"

#define EPSILON 0.0001f

#define GET_6TH_ARG(_1, _2, _3, _4, _5, _6, NAME, ...) NAME
#define GET_3RD_ARG(_1, _2, _3, NAME, ...) NAME
#define EXPAND(x) x

#define CHECK_CLOSE2(a, b, eps) CHECK((a) == doctest::Approx(b).epsilon(eps))
#define CHECK_CLOSE1(a, b) CHECK_CLOSE2(a, b, EPSILON)
#define GET_CHECK_CLOSE(_1, _2, _3, NAME, ...) NAME
#define CHECK_CLOSE_CHOOSER(...) EXPAND(GET_CHECK_CLOSE(__VA_ARGS__, CHECK_CLOSE2, CHECK_CLOSE1))
#define CHECK_CLOSE(...) EXPAND(CHECK_CLOSE_CHOOSER(__VA_ARGS__)(__VA_ARGS__))

#define CHECK_QUATERNION(q, _x, _y, _z, _w) CHECK(q.x == _x); CHECK(q.y == _y); CHECK(q.z == _z); CHECK(q.w == _w)
#define CHECK_QUATERNIONS(q1, q2) CHECK(q1.x == q2.x); CHECK(q1.y == q2.y); CHECK(q1.z == q2.z); CHECK(q1.w == q2.w)

#define CHECK_CLOSE_QUATERNION_6(q, _x, _y, _z, _w, eps) CHECK_CLOSE(q.x, _x, eps); CHECK_CLOSE(q.y, _y, eps); CHECK_CLOSE(q.z, _z, eps); CHECK_CLOSE(q.w, _w, eps) 
#define CHECK_CLOSE_QUATERNION_5(q, _x, _y, _z, _w) CHECK_CLOSE_QUATERNION_6(q, _x, _y, _z, _w,EPSILON)
#define CHECK_CLOSE_QUATERNION(...) EXPAND(GET_6TH_ARG(__VA_ARGS__, CHECK_CLOSE_QUATERNION_6, CHECK_CLOSE_QUATERNION_5)(__VA_ARGS__))

#define CHECK_CLOSE_QUATERNIONS_3(q1, q2, eps) CHECK_CLOSE(q1.x, q2.x, eps); CHECK_CLOSE(q1.y, q2.y, eps); CHECK_CLOSE(q1.z, q2.z, eps); CHECK_CLOSE(q1.w, q2.w, eps); 
#define CHECK_CLOSE_QUATERNIONS_2(q1, q2) CHECK_CLOSE_QUATERNIONS_3(q1, q2, EPSILON)
#define CHECK_CLOSE_QUATERNIONS(...) EXPAND(GET_3RD_ARG(__VA_ARGS__, CHECK_CLOSE_QUATERNIONS_3, CHECK_CLOSE_QUATERNIONS_2)(__VA_ARGS__))

#define CHECK_VECTOR(v1, v2) check_vector_exact(v1, v2)

#define CHECK_CLOSE_VECTOR_3(v1, v2, eps) check_vector_close(v1, v2, eps)
#define CHECK_CLOSE_VECTOR_2(v1, v2) check_vector_close(v1, v2, EPSILON)

#define GET_VECTOR_ARG(_1, _2, _3, NAME, ...) NAME
#define CHECK_CLOSE_VECTOR(...) EXPAND(GET_VECTOR_ARG(__VA_ARGS__, CHECK_CLOSE_VECTOR_3, CHECK_CLOSE_VECTOR_2)(__VA_ARGS__))

#define CHECK_MATRIX(m1, m2) check_matrix_exact(m1, m2)

#define CHECK_CLOSE_MATRIX_3(m1, m2, eps) check_matrix_close(m1, m2, eps)
#define CHECK_CLOSE_MATRIX_2(m1, m2) check_matrix_close(m1, m2, EPSILON)

#define GET_MATRIX_ARG(_1, _2, _3, NAME, ...) NAME
#define CHECK_CLOSE_MATRIX(...) EXPAND(GET_MATRIX_ARG(__VA_ARGS__, CHECK_CLOSE_MATRIX_3, CHECK_CLOSE_MATRIX_2)(__VA_ARGS__))

template<typename T, size_t N>
void check_vector_close(const ObsidianEngine::detail::Vector<T, N>& a,
    const ObsidianEngine::detail::Vector<T, N>& b,
    double eps = EPSILON)
{
    for (size_t i = 0; i < N; ++i)
    {
        CHECK_MESSAGE(a[i] == doctest::Approx(b[i]).epsilon(eps),
            "Value mismatch at index [" << i << "]");
    }
}

template<typename T, size_t N>
void check_vector_exact(const ObsidianEngine::detail::Vector<T, N>& a,
    const ObsidianEngine::detail::Vector<T, N>& b)
{
    for (size_t i = 0; i < N; ++i)
    {
        CHECK_MESSAGE(a[i] == b[i],
            "Exact value mismatch at index [" << i << "]");
    }
}

template<size_t R, size_t C, typename T>
void check_matrix_close(const ObsidianEngine::detail::Matrix<R, C, T>& a, const ObsidianEngine::detail::Matrix<R, C, T>& b,
    double eps = EPSILON) 
{
    for (size_t r = 0; r < R; ++r) 
    {
        for (size_t c = 0; c < C; ++c) 
        {
            CHECK_MESSAGE(a(r, c) == doctest::Approx(b(r, c)).epsilon(eps), "Value mismatch at (" << r << "," << c << ")");
        }
    }
}

template<size_t R, size_t C, typename T>
void check_matrix_exact(const ObsidianEngine::detail::Matrix<R, C, T>& a, const ObsidianEngine::detail::Matrix<R, C, T>& b) {
    for (size_t r = 0; r < R; ++r)
    {
        for (size_t c = 0; c < C; ++c) 
        {
            CHECK_MESSAGE(a(r, c) == b(r, c), "Exact value mismatch at (" << r << "," << c << ")");
        }
    }
}

#endif