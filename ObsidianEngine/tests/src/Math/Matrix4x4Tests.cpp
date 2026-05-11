#include "doctest/doctest.h"

#include "helpers.h"
#include "ObsidianEngine/Math/Math.h"
#include <string>

using namespace ObsidianEngine::detail;

TEST_SUITE("Math")
{
    TEST_SUITE("Matrix4x4<T>")
    {
        TEST_CASE_TEMPLATE("Matrix4x4<T>::determinant and Matrix4x4<T>::inverse", T, float, double)
        {
            SUBCASE("Identity Matrix")
            {
                Matrix4x4<T> m = Matrix4x4<T>::identity();
                CHECK_CLOSE(m.determinant(), 1);
                CHECK_CLOSE_MATRIX(m.inverse(), Matrix4x4<T>::identity());
            }

            SUBCASE("Singular Matrix")
            {
                Matrix4x4<T> m = Matrix4x4<T>::zero();
                CHECK_CLOSE(m.determinant(), 0);
                CHECK_CLOSE_MATRIX(m.inverse(), Matrix4x4<T>::identity());
            }

            SUBCASE("Complex Matrix")
            {
                Matrix4x4<T> m = Matrix4x4<T>::fromRows(
                    T(1), T(1), T(1), T(1),
                    T(0), T(2), T(1), T(1),
                    T(0), T(0), T(3), T(1),
                    T(0), T(0), T(0), T(4)
                );

                CHECK_CLOSE(m.determinant(), 24);

                Matrix4x4<T> inv = m.inverse();
                Matrix4x4<T> result = m * inv;

                CHECK_CLOSE_MATRIX(result, Matrix4x4<T>::identity());
            }

            SUBCASE("Negative Determinant")
            {
                Matrix4x4<T> m = Matrix4x4<T>::fromRows(
                    T(0), T(1), T(0), T(0),
                    T(1), T(0), T(0), T(0),
                    T(0), T(0), T(1), T(0),
                    T(0), T(0), T(0), T(1)
                );

                CHECK_CLOSE(m.determinant(), -1);

                Matrix4x4<T> inv = m.inverse();
                CHECK_CLOSE_MATRIX(m * inv, Matrix4x4<T>::identity());
            }

            SUBCASE("Inverse")
            {
                Matrix4x4<T> m = Matrix4x4<T>::fromRows(
                    T(1), T(1), T(1), T(1),
                    T(0), T(2), T(1), T(1),
                    T(0), T(0), T(3), T(1),
                    T(0), T(0), T(0), T(4)
                );

                Matrix4x4<T> inv = m.inverse();
                CHECK_CLOSE_MATRIX(m * inv, Matrix4x4<T>::identity());
            }
        }
        TEST_CASE_TEMPLATE("Matrix4x4<T>::multiplyPoint and Matrix4x4<T>::multiplyDirection", T, float, double)
        {
            Matrix4x4<T> translation = Matrix4x4<T>::identity();
            translation(0, 3) = T(10);
            translation(1, 3) = T(20);
            translation(2, 3) = T(30);

            SUBCASE("Matrix4x4<T>::multiplyPoint")
            {
                Vector<T, 3> point(1.0f, 2.0f, 3.0f);

                Vector<T, 3> result = translation.multiplyPoint(point);

                Vector<T, 3> expected(T(11), T(22), T(33));
                CHECK_CLOSE_VECTOR(result, expected);
            }

            SUBCASE("Matrix4x4<T>::multiplyDirection")
            {
                Vector<T, 3> direction(1.0f, 2.0f, 3.0f);

                Vector<T, 3> result = translation.multiplyDirection(direction);

                CHECK_CLOSE_VECTOR(result, direction);
            }

            SUBCASE("Scaling Point and Direction")
            {
                Matrix4x4<T> scale = Matrix4x4<T>::identity();
                scale(0, 0) = T(2);
                scale(1, 1) = T(2);
                scale(2, 2) = T(2);

                Vector<T, 3> v(1.0f, 2.0f, 3.0f);
                Vector<T, 3> expected(2.0f, 4.0f, 6.0f);

                CHECK_CLOSE_VECTOR(scale.multiplyPoint(v), expected);
                CHECK_CLOSE_VECTOR(scale.multiplyDirection(v), expected);
            }
        }
        TEST_CASE_TEMPLATE("Matrix4x4<T> TRS Logic", T, float, double)
        {
            SUBCASE("Translate and Scale")
            {
                Vector<T, 3> pos(10, 20, 30);
                Matrix4x4<T> t = Matrix4x4<T>::translate(pos);
                CHECK_CLOSE_VECTOR(t.position(), pos);

                Vector<T, 3> s(2, 3, 4);
                Matrix4x4<T> sc = Matrix4x4<T>::scale(s);
                CHECK_CLOSE_VECTOR(sc.lossyScale(), s);
            }
            SUBCASE("TRS Composition and Decomposition")
            {
                Vector<T, 3> pos(5.0f, -2.0f, 10.0f);
                Quaternion<T> rot = Quaternion<T>::fromEuler(Vector<T, 3>(0.0f, 1.5708f, 0.0f));
                Vector<T, 3> sc(2.0f, 2.0f, 2.0f);

                Matrix4x4<T> m = Matrix4x4<T>::trs(pos, rot, sc);

                CHECK(m.validTRS() == true);

                CHECK_CLOSE_VECTOR(m.position(), pos);
                CHECK_CLOSE_VECTOR(m.lossyScale(), sc);

                Quaternion<T> extractedRot = m.rotation();
                CHECK_CLOSE_QUATERNIONS(extractedRot, rot);
            }
            SUBCASE("Euler Rotation")
            {
                T angle = static_cast<T>(1.5708);

                Matrix4x4<T> rotX = Matrix4x4<T>::rotateX(angle);
                Matrix4x4<T> rotY = Matrix4x4<T>::rotateY(angle);
                Matrix4x4<T> rotZ = Matrix4x4<T>::rotateZ(angle);

                Vector<T, 3> up(0, 1, 0);
                CHECK_CLOSE_VECTOR(rotX.multiplyDirection(up), Vector<T, 3>(0, 0, 1));

                Vector<T, 3> right(1, 0, 0);
                CHECK_CLOSE_VECTOR(rotY.multiplyDirection(right), Vector<T, 3>(0, 0, -1));
                CHECK_CLOSE_VECTOR(rotZ.multiplyDirection(right), Vector<T, 3>(0, 1, 0));
            }
            SUBCASE("Invalid TRS Detection")
            {
                Matrix4x4<T> m = Matrix4x4<T>::zero();
                CHECK(m.validTRS() == false);

                Matrix4x4<T> scaleZero = Matrix4x4<T>::scale(Vector<T, 3>(0, 1, 1));
                CHECK(scaleZero.validTRS() == false);
            }
            SUBCASE("Negative Scaling (Mirroring)")
            {
                Vector<T, 3> scale(-1.0f, 2.0f, 3.0f);
                Matrix4x4<T> m = Matrix4x4<T>::scale(scale);

                CHECK_CLOSE_VECTOR(m.lossyScale(), Vector<T, 3>(1.0f, 2.0f, 3.0f));
                CHECK(m.determinant() < 0);
            }
            SUBCASE("Non Uniform Scale and Rotation")
            {
                Vector<T, 3> pos(0, 0, 0);
                Quaternion<T> rot = Quaternion<T>::fromEuler(Vector<T, 3>(0.4f, 0.5f, 0.6f));
                Vector<T, 3> sc(1.0f, 5.0f, 0.2f);

                Matrix4x4<T> m = Matrix4x4<T>::trs(pos, rot, sc);

                CHECK_CLOSE_VECTOR(m.lossyScale(), sc);
                CHECK_CLOSE_QUATERNIONS(m.rotation(), rot, 0.001f);
            }
            SUBCASE("Identity")
            {
                Matrix4x4<T> m = Matrix4x4<T>::identity();

                CHECK_CLOSE_VECTOR(m.position(), Vector<T, 3>(0, 0, 0));
                CHECK_CLOSE_VECTOR(m.lossyScale(), Vector<T, 3>(1, 1, 1));
                CHECK_CLOSE_QUATERNIONS(m.rotation(), Quaternion<T>::identity());
            }
            SUBCASE("Near Singular Matrix")
            {
                Matrix4x4<T> m = Matrix4x4<T>::scale(Vector<T, 3>(1e-10f, 1.0f, 1.0f));
                CHECK(m.validTRS() == false);
            }
            SUBCASE("Inversion Integrity")
            {
                Matrix4x4<T> original = Matrix4x4<T>::trs({ 10, -5, 2 }, Quaternion<T>::fromEuler({ 0.5, 0.2, 0.1 }), { 2, 2, 2 });

                Matrix4x4<T> inv = original.inverse();
                Matrix4x4<T> result = original * inv;

                CHECK_CLOSE_MATRIX(result, Matrix4x4<T>::identity());
            }
        }
        TEST_CASE_TEMPLATE("Camera Matrices", T, float, double)
        {
            SUBCASE("View Matrix")
            {
                Vector<T, 3> eye(0, 0, 10);
                Vector<T, 3> target(0, 0, 0);
                Vector<T, 3> up(0, 1, 0);

                Matrix4x4<T> view = Matrix4x4<T>::lookAt(eye, target, up);

                Vector<T, 3> viewSpaceOrigin = view.multiplyPoint(Vector<T, 3>(0, 0, 0));
                CHECK_CLOSE_VECTOR(viewSpaceOrigin, Vector<T, 3>(0, 0, -10));

                Vector<T, 3> viewSpaceEye = view.multiplyPoint(eye);
                CHECK_CLOSE_VECTOR(viewSpaceEye, Vector<T, 3>(0, 0, 0));

                Vector<T, 3> worldPoint(0, 11, 10);
                Vector<T, 3> viewPoint = view.multiplyPoint(worldPoint);
                CHECK_CLOSE_VECTOR(viewPoint, Vector<T, 3>(0, 11, 0));

                Vector<T, 3> worldDirection(0, 1, 0);
                Vector<T, 3> viewDirection = view.multiplyDirection(worldDirection);
                CHECK_CLOSE_VECTOR(viewDirection, Vector<T, 3>(0, 1, 0));

                Vector<T, 3> worldRight(1, 0, 0);
                Vector<T, 3> viewRight = view.multiplyDirection(worldRight);
                CHECK_CLOSE_VECTOR(viewRight, Vector<T, 3>(1, 0, 0));
            }
            SUBCASE("Perspective Projection")
            {
                T fov = Math<T>::val(90.0);
                T aspect = Math<T>::val(1.0);
                T nearZ = Math<T>::val(1.0);
                T farZ = Math<T>::val(100.0);

                Matrix4x4<T> proj = Matrix4x4<T>::perspective(fov, aspect, nearZ, farZ);

                Vector<T, 4> pNear(0, 0, -nearZ, 1);
                Vector<T, 4> resNear = proj * pNear;
                CHECK_CLOSE(resNear.z / resNear.w, Math<T>::val(-1));

                Vector<T, 4> pFar(0, 0, -farZ, 1);
                Vector<T, 4> resFar = proj * pFar;
                CHECK_CLOSE(resFar.z / resFar.w, Math<T>::val(1));

                Vector<T, 4> pEdge(1, 1, -1, 1);
                Vector<T, 4> resEdge = proj * pEdge;
                CHECK_CLOSE(resEdge.x / resEdge.w, Math<T>::val(1));
                CHECK_CLOSE(resEdge.y / resEdge.w, Math<T>::val(1));

                Matrix4x4<T> projWide = Matrix4x4<T>::perspective(fov, Math<T>::val(2), nearZ, farZ);
                Vector<T, 4> resWide = projWide * pEdge;
                CHECK_CLOSE(resWide.x / resWide.w, Math<T>::val(0.5));
            }
            SUBCASE("Orthographic Projection")
            {
                T left = 0, right = 100;
                T bottom = 0, top = 100;
                T nearZ = 0, farZ = 10;

                Matrix4x4<T> projection = Matrix4x4<T>::ortho(left, right, bottom, top, nearZ, farZ);

                Vector<T, 3> center(50, 50, 5);
                CHECK_CLOSE_VECTOR(projection.multiplyPoint(center), Vector<T, 3>(0, 0, -2));

                Vector<T, 3> bottomLeftNear(0, 0, 0);
                CHECK_CLOSE_VECTOR(projection.multiplyPoint(bottomLeftNear), Vector<T, 3>(-1, -1, -1));

                Vector<T, 3> topRightFar(100, 100, 10);
                CHECK_CLOSE_VECTOR(projection.multiplyPoint(topRightFar), Vector<T, 3>(1, 1, -3));

                Matrix4x4<T> sym = Matrix4x4<T>::ortho(-50, 50, -50, 50, 0, 10);
                CHECK_CLOSE_VECTOR(sym.multiplyPoint(Vector<T, 3>(0, 0, 5)), Vector<T, 3>(0, 0, -2));
            }
            SUBCASE("Frustum Projection")
            {
                T left = -1, right = 1;
                T bottom = -1, top = 1;
                T nearZ = 1, farZ = 10;

                Matrix4x4<T> frust = Matrix4x4<T>::frustum(left, right, bottom, top, nearZ, farZ);

                Vector<T, 4> pNear(0, 0, -1, 1);
                Vector<T, 4> resNear = frust * pNear;
                CHECK_CLOSE(resNear.z / resNear.w, T(-1));

                Vector<T, 4> pFar(0, 0, -10, 1);
                Vector<T, 4> resFar = frust * pFar;
                CHECK_CLOSE(resFar.z / resFar.w, T(1));

                Vector<T, 4> pCorner(-1, -1, -1, 1);
                Vector<T, 4> resCorner = frust * pCorner;
                CHECK_CLOSE(resCorner.x / resCorner.w, T(-1));
                CHECK_CLOSE(resCorner.y / resCorner.w, T(-1));
                CHECK_CLOSE(resCorner.z / resCorner.w, T(-1));


                Matrix4x4<T> offCenter = Matrix4x4<T>::frustum(-1, 3, -1, 1, 1, 10);
                Vector<T, 4> pCenter(0, 0, -1, 1);
                Vector<T, 4> resOff = offCenter * pCenter;

                CHECK(resOff.x / resOff.w < 0);
            }
            SUBCASE("Perspective and Frustum Equivalence")
            {
                T fov = Math<T>::val(90.0);
                T aspect = Math<T>::val(1.0);
                T nearZ = Math<T>::val(1.0);
                T farZ = Math<T>::val(10.0);

                Matrix4x4<T> persp = Matrix4x4<T>::perspective(fov, aspect, nearZ, farZ);

                T top = nearZ * std::tan(fov * Math<T>::Pi / Math<T>::val(360.0));
                T bottom = -top;
                T right = top * aspect;
                T left = -right;

                Matrix4x4<T> frust = Matrix4x4<T>::frustum(left, right, bottom, top, nearZ, farZ);

                CHECK_CLOSE_MATRIX(persp, frust);
            }
        }
    }
}