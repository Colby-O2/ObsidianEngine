#include "doctest/doctest.h"

#include "helpers.h"
#include "ObsidianEngine/Math/Math.h"
#include <string>

using namespace ObsidianEngine::detail;

TEST_SUITE("Math")
{
    TEST_SUITE("Quaternion<T>")
    {
        TEST_CASE_TEMPLATE("Constructors", T, float, double)
        {
            SUBCASE("Default Constructor")
            {
                Quaternion<T> q;
                CHECK_QUATERNION(q, 0.0f, 0.0f, 0.0f, 1.0f);

            }
            SUBCASE("Element-Wise Constructor")
            {
                Quaternion<T> q(1.0f, 2.0f, 3.0f, 4.0f);
                CHECK_QUATERNION(q, 1.0f, 2.0f, 3.0f, 4.0f);
            }
            SUBCASE("Vector4 Constructor")
            {
                Vector<T, 4> v(5.0f, 6.0f, 7.0f, 8.0f);
                Quaternion<T> q(v);
                CHECK_QUATERNION(q, 5.0f, 6.0f, 7.0f, 8.0f);
            }
        }
        TEST_CASE_TEMPLATE("Identity", T, float, double)
        {
            SUBCASE("Identity Constructor")
            {
                Quaternion<T> q = Quaternion<T>::identity();
                CHECK_QUATERNION(q, 0.0f, 0.0f, 0.0f, 1.0f);
            }
            SUBCASE("Identity Length")
            {
                Quaternion<T> q = Quaternion<T>::identity();
                CHECK_CLOSE(q.length(), 1.0f);
            }
            SUBCASE("Identity Conjugate")
            {
                Quaternion<T> q = Quaternion<T>::identity();
                Quaternion<T> conj = q.conjugate();
                CHECK_QUATERNION(conj, 0.0f, 0.0f, 0.0f, 1.0f);
            }
            SUBCASE("Identity Multplication")
            {
                Quaternion<T> q(0.3f, 0.5f, 0.6f, 0.1f);
                Quaternion<T> id = Quaternion<T>::identity();
                Quaternion<T> result = q * id;
                CHECK_QUATERNIONS(result, q);
            }
        }
        TEST_CASE_TEMPLATE("Quaternion<T>::fromAxisAngle", T, float, double)
        {
            SUBCASE("Standard Rotation")
            {
                T angle = 90.0f;
                Vector<T, 3> axis(0, 1, 0);
                Quaternion<T> q = Quaternion<T>::fromAxisAngle(axis, angle);
                CHECK_CLOSE_QUATERNION(q, 0.0f, 0.707106f, 0.0f, 0.707106f);
                CHECK_CLOSE(q.length(), 1.0f);
            }
            SUBCASE("Zero Angle")
            {
                T angle = 0.0f;
                Vector<T, 3> axis(0, 1, 0);
                Quaternion<T> q = Quaternion<T>::fromAxisAngle(axis, angle);
                CHECK_QUATERNIONS(q, Quaternion<T>::identity());
                CHECK_CLOSE(q.length(), 1.0f);
            }
            SUBCASE("Full Rotation")
            {
                T angle = 360.0f;
                Vector<T, 3> axis(0, 1, 0);
                Quaternion<T> q = Quaternion<T>::fromAxisAngle(axis, angle);
                CHECK_CLOSE(q.x, 0.0f);
                CHECK_CLOSE(q.y, 0.0f);
                CHECK_CLOSE(q.z, 0.0f);
                CHECK_CLOSE(std::abs(q.w), 1.0f);
                CHECK_CLOSE(q.length(), 1.0f);
            }
            SUBCASE("Non Unit Axis")
            {
                T angle = 90.0f;
                Vector<T, 3> axis(0, 2, 0);
                Quaternion<T> q = Quaternion<T>::fromAxisAngle(axis, angle);
                CHECK_CLOSE_QUATERNION(q, 0.0f, 0.707106f, 0.0f, 0.707106f);
                CHECK_CLOSE(q.length(), 1.0f);
            }
            SUBCASE("Negative Angle")
            {
                T angle = -90.0f;
                Vector<T, 3> axis(0, 1, 0);
                Quaternion<T> q = Quaternion<T>::fromAxisAngle(axis, angle);
                CHECK_CLOSE_QUATERNION(q, 0.0f, -0.707106f, 0.0f, 0.707106f);
                CHECK_CLOSE(q.length(), 1.0f);
            }
            SUBCASE("Large Angle Wraparound")
            {
                T angle = 720.0f;
                Vector<T, 3> axis(0, 1, 0);
                Quaternion<T> q = Quaternion<T>::fromAxisAngle(axis, angle);
                CHECK_CLOSE_QUATERNION(q, 0.0f, 0.0f, 0.0f, 1.0f);
                CHECK_CLOSE(q.length(), 1.0f);
            }
            SUBCASE("180 Degree Precision")
            {
                T angle = 180.0f;
                Vector<T, 3> axis(0, 1, 0);
                Quaternion<T> q = Quaternion<T>::fromAxisAngle(axis, angle);
                CHECK_CLOSE_QUATERNION(q, 0.0f, 1.0f, 0.0f, 0.0f);
                CHECK_CLOSE(q.length(), 1.0f);
            }
            SUBCASE("Opposite Directions")
            {
                Quaternion<T> qPos = Quaternion<T>::fromAxisAngle(Vector<T, 3>(1, 0, 0), 90.0f);
                Quaternion<T> qNeg = Quaternion<T>::fromAxisAngle(Vector<T, 3>(1, 0, 0), -90.0f);

                CHECK_CLOSE(qPos.x, -qNeg.x);
                CHECK_CLOSE(qPos.y, qNeg.y);
                CHECK_CLOSE(qPos.z, qNeg.z);
                CHECK_CLOSE(qPos.w, qNeg.w);

                CHECK_CLOSE(qPos.length(), 1.0f);
                CHECK_CLOSE(qNeg.length(), 1.0f);
            }
            SUBCASE("Zero Axis")
            {
                Vector<T, 3> zeroAxis(0.0f, 0.0f, 0.0f);
                Quaternion<T> q = Quaternion<T>::fromAxisAngle(zeroAxis, 90.0f);
                CHECK_CLOSE_QUATERNIONS(q, Quaternion<T>::identity());
                CHECK_CLOSE(q.length(), 1.0f);
            }
            SUBCASE("Diagonal Axis Rotation")
            {
                Vector<T, 3> axis(1.0f, 1.0f, 1.0f);
                T angle = 120.0f;
                Quaternion<T> q = Quaternion<T>::fromAxisAngle(axis, angle);
                CHECK_CLOSE_QUATERNION(q, 0.5f, 0.5f, 0.5f, 0.5f);
                CHECK_CLOSE(q.length(), 1.0f);
            }
        }
        TEST_CASE_TEMPLATE("Quaternion<T>::toAxisAngle", T, float, double)
        {
            SUBCASE("Standard Rotation")
            {
                Vector<T, 3> axis(0, 1, 0);
                T angle = 90.0f;
                Quaternion<T> q = Quaternion<T>::fromAxisAngle(axis, angle);

                Vector<T, 3> outAxis;
                T outAngle;
                q.toAxisAngle(outAxis, outAngle);

                CHECK_CLOSE(outAngle, angle);
                CHECK_CLOSE_VECTOR(outAxis, axis);
            }
            SUBCASE("Identity Quaternion<T>")
            {
                Quaternion<T> q = Quaternion<T>::identity();
                Vector<T, 3> outAxis;
                T outAngle;
                q.toAxisAngle(outAxis, outAngle);

                CHECK_CLOSE(outAngle, 0.0f);
                CHECK_CLOSE_VECTOR(outAxis, Vector<T, 3>(1, 0, 0));
                CHECK(!std::isnan(outAngle));
            }
            SUBCASE("Non Unit Quaternion<T>")
            {
                Quaternion<T> q(0.0f, 0.0f, 0.0f, 2.0f);

                Vector<T, 3> outAxis;
                T outAngle;
                q.toAxisAngle(outAxis, outAngle);

                CHECK(!std::isnan(outAngle));
                CHECK(!std::isnan(outAxis.x));
                CHECK_CLOSE(outAngle, 0.0f);
            }
            SUBCASE("180 Degree Flip")
            {
                Vector<T, 3> axis(1, 0, 0);
                T angle = 180.0f;
                Quaternion<T> q = Quaternion<T>::fromAxisAngle(axis, angle);

                Vector<T, 3> outAxis;
                T outAngle;
                q.toAxisAngle(outAxis, outAngle);

                CHECK_CLOSE(outAngle, angle);
                CHECK_CLOSE_VECTOR(outAxis, axis);
            }
            SUBCASE("Round Trip Consistency")
            {
                Vector<T, 3> originalAxis = Vector<T, 3>(1, 2, 3).normalized();
                T originalAngle = 45.0f;

                Quaternion<T> q = Quaternion<T>::fromAxisAngle(originalAxis, originalAngle);

                Vector<T, 3> resultAxis;
                T resultAngle;
                q.toAxisAngle(resultAxis, resultAngle);

                CHECK_CLOSE(resultAngle, originalAngle);
                CHECK_CLOSE_VECTOR(resultAxis, originalAxis);
            }
        }
        TEST_CASE_TEMPLATE("Quaternion<T>::angle", T, float, double)
        {
            SUBCASE("Zero Difference")
            {
                Quaternion<T> q1 = Quaternion<T>::fromEuler(10, 20, 30);
                Quaternion<T> q2 = q1;
                T angle = Quaternion<T>::angle(q1, q2);
                CHECK_CLOSE(angle, 0.0f);
            }
            SUBCASE("Opposite Sign")
            {
                Quaternion<T> q1(0.182574f, 0.365148f, 0.547723f, 0.730297f);
                Quaternion<T> q2(-q1.x, -q1.y, -q1.z, -q1.w);

                CHECK(Quaternion<T>::angle(q1, q2) == doctest::Approx(0.0f));
            }
            SUBCASE("Safety Clamp")
            {
                Quaternion<T> q1(0, 0, 0, 1.0f);
                Quaternion<T> q2(0, 0, 0, 1.0000001f);

                T result = Quaternion<T>::angle(q1, q2);
                CHECK_FALSE(std::isnan(result));
                CHECK_CLOSE(result, 0.0f);
            }
            SUBCASE("Known Angles")
            {
                Quaternion<T> qId = Quaternion<T>::identity();

                Quaternion<T> q90 = Quaternion<T>::fromAxisAngle(Vector<T, 3>(1, 0, 0), 90.0f);
                CHECK_CLOSE(Quaternion<T>::angle(qId, q90), 90.0f);

                Quaternion<T> q180 = Quaternion<T>::fromAxisAngle(Vector<T, 3>(0, 0, 1), 180.0f);
                CHECK_CLOSE(Quaternion<T>::angle(qId, q180), 180.0f);
            }
            SUBCASE("Near 180 Degrees")
            {
                Quaternion<T> q1 = Quaternion<T>::fromAxisAngle(Vector<T, 3>(1, 0, 0), 0.0f);
                Quaternion<T> q2 = Quaternion<T>::fromAxisAngle(Vector<T, 3>(1, 0, 0), 179.99f);

                T result = Quaternion<T>::angle(q1, q2);
                CHECK_CLOSE(result, 179.99f);
            }
            SUBCASE("Exactly 180 Degrees")
            {
                Quaternion<T> q1 = Quaternion<T>::identity();
                Quaternion<T> q2 = Quaternion<T>::fromAxisAngle(Vector<T, 3>(0, 1, 0), 180.0f);
                T result = Quaternion<T>::angle(q1, q2);
                CHECK_CLOSE(result, 180.0f);
            }
            SUBCASE("Small Angle Difference")
            {
                T smallAngle = 0.1f;
                Quaternion<T> q1 = Quaternion<T>::identity();
                Quaternion<T> q2 = Quaternion<T>::fromAxisAngle(Vector<T, 3>(0, 1, 0), smallAngle);

                T result = Quaternion<T>::angle(q1, q2);

                CHECK_CLOSE(result, smallAngle, 0.01);
            }
        }
        TEST_CASE_TEMPLATE("Quaternion<T>::rotateTowards", T, float, double)
        {
            Quaternion<T> from = Quaternion<T>::identity();
            Quaternion<T> to = Quaternion<T>::fromAxisAngle(Vector<T, 3>(0, 1, 0), 90.0f);

            SUBCASE("Partial Rotation")
            {
                T maxDelta = 30.0f;
                Quaternion<T> result = Quaternion<T>::rotateTowards(from, to, maxDelta);
                T angleMoved = Quaternion<T>::angle(from, result);

                CHECK_CLOSE(angleMoved, 30.0f);
                CHECK_CLOSE(Quaternion<T>::angle(result, to), 60.0f);
            }
            SUBCASE("Overshooting (Clamping to target)")
            {
                T maxDelta = 120.0f;
                Quaternion<T> result = Quaternion<T>::rotateTowards(from, to, maxDelta);

                CHECK_QUATERNIONS(result, to);
                CHECK_CLOSE(Quaternion<T>::angle(result, to), 0.0f);
            }
            SUBCASE("Already at Target")
            {
                Quaternion<T> result = Quaternion<T>::rotateTowards(to, to, 10.0f);

                CHECK_QUATERNIONS(result, to);
                CHECK_CLOSE(Quaternion<T>::angle(result, to), 0.0f);
            }
            SUBCASE("Small Delta Accumulation")
            {
                T smallDelta = 0.00001f;
                Quaternion<T> result = from;
                int iterations = 500000;

                for (int i = 0; i < iterations; i++)
                {
                    result = Quaternion<T>::rotateTowards(result, to, smallDelta);
                }

                T totalMoved = Quaternion<T>::angle(from, result);

                CHECK(totalMoved > 0.1f);
                CHECK(result.w < 1.0f);
            }
        }
        TEST_CASE_TEMPLATE("Quaternion<T>::fromToRotation", T, float, double)
        {
            SUBCASE("Identical Directions")
            {
                Vector<T, 3> v1(1.0f, 0.0f, 0.0f);
                Vector<T, 3> v2(1.0f, 0.0f, 0.0f);
                Quaternion<T> q = Quaternion<T>::fromToRotation(v1, v2);

                CHECK_QUATERNIONS(q, Quaternion<T>::identity());
                CHECK_CLOSE(q.length(), 1.0f);
            }
            SUBCASE("Opposite Directions")
            {
                Vector<T, 3> v1(1.0f, 0.0f, 0.0f);
                Vector<T, 3> v2(-1.0f, 0.0f, 0.0f);

                Quaternion<T> q = Quaternion<T>::fromToRotation(v1, v2);
                CHECK_CLOSE(q.w, 0.0f);
                CHECK_CLOSE(q.length(), 1.0f);

                Vector<T, 3> result = q * v1;
                CHECK_CLOSE_VECTOR(result, v2);

                Vector<T, 3> v3(0.0f, 1.0f, 0.0f);
                Vector<T, 3> v4(0.0f, -1.0f, 0.0f);
                Quaternion<T> q2 = Quaternion<T>::fromToRotation(v3, v4);
                CHECK_CLOSE(q2.w, 0.0f);

                Vector<T, 3> result2 = q2 * v3;
                CHECK_CLOSE_VECTOR(result2, v4);
            }
            SUBCASE("90 Degree Rotation")
            {
                Vector<T, 3> v1(1.0f, 0.0f, 0.0f);
                Vector<T, 3> v2(0.0f, 1.0f, 0.0f);
                Quaternion<T> q = Quaternion<T>::fromToRotation(v1, v2);

                CHECK_CLOSE_QUATERNION(q, 0.0f, 0.0f, 0.707106f, 0.707106f);
                CHECK_CLOSE(q.length(), 1.0f);

                Vector<T, 3> result = q * v1;
                CHECK_CLOSE_VECTOR(result, v2);
            }
            SUBCASE("Non Unit Input Vectors")
            {
                Vector<T, 3> v1(2.0f, 0.0f, 0.0f);
                Vector<T, 3> v2(0.0f, 5.0f, 0.0f);
                Quaternion<T> q = Quaternion<T>::fromToRotation(v1, v2);

                CHECK_CLOSE_QUATERNION(q, 0.0f, 0.0f, 0.707106f, 0.707106f);
                CHECK_CLOSE(q.length(), 1.0f);
            }
            SUBCASE("Small Rotation")
            {
                Vector<T, 3> v1(1.0f, 1.0f, 0.0f);
                Vector<T, 3> v2(1.0f, 1.1f, 0.0f);
                v1.normalize();
                v2.normalize();

                Quaternion<T> q = Quaternion<T>::fromToRotation(v1, v2);
                CHECK_CLOSE(q.length(), 1.0f);

                Vector<T, 3> result = q * v1;
                CHECK_CLOSE_VECTOR(result, v2);
            }
        }
        TEST_CASE_TEMPLATE("Quaternion<T>::fromEuler", T, float, double)
        {
            SUBCASE("Individual Axis Rotations")
            {
                Quaternion<T> qX = Quaternion<T>::fromEuler(90.0f, 0.0f, 0.0f);
                CHECK_CLOSE_QUATERNION(qX, 0.707106f, 0.0f, 0.0f, 0.707106f);

                Quaternion<T> qY = Quaternion<T>::fromEuler(0.0f, 90.0f, 0.0f);
                CHECK_CLOSE_QUATERNION(qY, 0.0f, 0.707106f, 0.0f, 0.707106f);

                Quaternion<T> qZ = Quaternion<T>::fromEuler(0.0f, 0.0f, 90.0f);
                CHECK_CLOSE_QUATERNION(qZ, 0.0f, 0.0f, 0.707106f, 0.707106f);
            }
            SUBCASE("Zero Rotation")
            {
                Quaternion<T> q = Quaternion<T>::fromEuler(0.0f, 0.0f, 0.0f);
                CHECK_QUATERNIONS(q, Quaternion<T>::identity());
            }
            SUBCASE("Combined Rotations")
            {
                Quaternion<T> q = Quaternion<T>::fromEuler(30.0f, 45.0f, 60.0f);
                CHECK_CLOSE_QUATERNION(q, 0.022260f, 0.4396797f, 0.3604234f, 0.8223632f);
            }
            SUBCASE("Vector<T, 3> Overload")
            {
                Vector<T, 3> angles(45.0f, 0.0f, 0.0f);
                Quaternion<T> q1 = Quaternion<T>::fromEuler(angles);
                Quaternion<T> q2 = Quaternion<T>::fromEuler(45.0f, 0.0f, 0.0f);
                CHECK_QUATERNIONS(q1, q2);
            }

            SUBCASE("Gimbal Lock (90 deg Pitch)")
            {
                Quaternion<T> q = Quaternion<T>::fromEuler(0.0f, 90.0f, 0.0f);
                Vector<T, 3> forward = q * Vector<T, 3>(0, 0, 1);
                CHECK_CLOSE(forward.x, 1.0f);
                CHECK_CLOSE(forward.y, 0.0f);
                CHECK_CLOSE(forward.z, 0.0f);
            }

            SUBCASE("Negative Angles")
            {
                Quaternion<T> qPos = Quaternion<T>::fromEuler(0.0f, 45.0f, 0.0f);
                Quaternion<T> qNeg = Quaternion<T>::fromEuler(0.0f, -45.0f, 0.0f);
                Quaternion<T> conj = qPos.conjugate();
                CHECK_CLOSE_QUATERNIONS(qNeg, conj);
            }
        }
        TEST_CASE_TEMPLATE("Quaternion<T>::fromEulerEx", T, float, double)
        {
            T x = 30.0f, y = 45.0f, z = 60.0f;

            SUBCASE("XYZ Order") 
            {
                Quaternion<T> q = Quaternion<T>::fromEulerEx(x, y, z, ObsidianEngine::EulerOrder::XYZ);
                CHECK_CLOSE_QUATERNION(q, 0.3919038f, 0.2005621f, 0.5319757f, 0.7233174f);
            }

            SUBCASE("XZY Order") 
            {
                Quaternion<T> q = Quaternion<T>::fromEulerEx(x, y, z, ObsidianEngine::EulerOrder::XZY);
                CHECK_CLOSE_QUATERNION(q, 0.02226f, 0.2005621f, 0.5319757f, 0.8223632f);
            }

            SUBCASE("YXZ Order") 
            {
                Quaternion<T> q = Quaternion<T>::fromEulerEx(x, y, z, ObsidianEngine::EulerOrder::YXZ);
                CHECK_CLOSE_QUATERNION(q, 0.3919038f, 0.2005621f, 0.3604234f, 0.8223632f);
            }

            SUBCASE("YZX Order") 
            {
                Quaternion<T> q = Quaternion<T>::fromEulerEx(x, y, z, ObsidianEngine::EulerOrder::YZX);
                CHECK_CLOSE_QUATERNION(q, 0.3919038f, 0.4396797f, 0.3604234f, 0.7233174f);
            }

            SUBCASE("ZXY Order") 
            {
                Quaternion<T> q = Quaternion<T>::fromEulerEx(x, y, z, ObsidianEngine::EulerOrder::ZXY);
                CHECK_CLOSE_QUATERNION(q, 0.02226f, 0.4396797f, 0.5319757f, 0.7233174f);
            }
            SUBCASE("ZYX Order") 
            {
                Quaternion<T> q1 = Quaternion<T>::fromEulerEx(x, y, z, ObsidianEngine::EulerOrder::ZYX);
                Quaternion<T> q2 = Quaternion<T>::fromEuler(x, y, z);
                CHECK_QUATERNIONS(q1, q2);
                CHECK_CLOSE_QUATERNION(q1, 0.02226f, 0.4396797f, 0.3604234f, 0.8223632f);
            }
        }
        TEST_CASE_TEMPLATE("Quaternion<T>::toEulerEx", T, float, double)
        {
            SUBCASE("Round-Trip")
            {
                struct OrderMapping
                {
                    ObsidianEngine::EulerOrder order;
                    const char* name;
                };

                OrderMapping mappings[] =
                {
                     { ObsidianEngine::EulerOrder::XYZ, "XYZ" },
                     { ObsidianEngine::EulerOrder::XZY, "XZY" },
                     { ObsidianEngine::EulerOrder::YXZ, "YXZ" },
                     { ObsidianEngine::EulerOrder::YZX, "YZX" },
                     { ObsidianEngine::EulerOrder::ZXY, "ZXY" },
                     { ObsidianEngine::EulerOrder::ZYX, "ZYX" }
                };

                Vector<T, 3> testAngles(20.0f, 35.0f, 50.0f);

                for (const auto& item : mappings)
                {
                    std::string subcaseName = std::string("Physical Orientation Integrity (") + item.name + ")";

                    Quaternion<T> q = Quaternion<T>::fromEulerEx(testAngles.x, testAngles.y, testAngles.z, item.order);

                    Vector<T, 3> resultAngles = q.toEulerEx(item.order);

                    Quaternion<T> qVerify = Quaternion<T>::fromEulerEx(resultAngles.x, resultAngles.y, resultAngles.z, item.order);

                    SUBCASE(subcaseName.c_str())
                    {
                        CAPTURE(item.name);
                        CHECK_CLOSE_QUATERNIONS(q, qVerify);
                    }
                }
            }
            SUBCASE("Gimbal Lock")
            {
                struct OrderMapping {
                    ObsidianEngine::EulerOrder order;
                    const char* name;
                };

                OrderMapping mappings[] = {
                    { ObsidianEngine::EulerOrder::XYZ, "XYZ" },
                    { ObsidianEngine::EulerOrder::XZY, "XZY" },
                    { ObsidianEngine::EulerOrder::YXZ, "YXZ" },
                    { ObsidianEngine::EulerOrder::YZX, "YZX" },
                    { ObsidianEngine::EulerOrder::ZXY, "ZXY" },
                    { ObsidianEngine::EulerOrder::ZYX, "ZYX" }
                };

                float lockAngles[] = { 90.0f, -90.0f };

                for (const auto& item : mappings)
                {
                    for (float lockAngle : lockAngles)
                    {
                        std::string subcaseName = std::string("Gimbal Lock ") + item.name + " at " + std::to_string(lockAngle);

                        SUBCASE(subcaseName.c_str())
                        {
                            CAPTURE(item.name);
                            CAPTURE(lockAngle);

                            Vector<T, 3> inputAngles;
                            if (item.order == ObsidianEngine::EulerOrder::XYZ || item.order == ObsidianEngine::EulerOrder::ZYX)
                            {
                                inputAngles = Vector<T, 3>(15.0f, lockAngle, 30.0f);
                            }
                            else if (item.order == ObsidianEngine::EulerOrder::XZY || item.order == ObsidianEngine::EulerOrder::YZX)
                            {
                                inputAngles = Vector<T, 3>(15.0f, 30.0f, lockAngle);
                            }
                            else
                            {
                                inputAngles = Vector<T, 3>(lockAngle, 15.0f, 30.0f);
                            }

                            Quaternion<T> q = Quaternion<T>::fromEulerEx(inputAngles.x, inputAngles.y, inputAngles.z, item.order);
                            Vector<T, 3> extracted = q.toEulerEx(item.order);
                            Quaternion<T> qVerify = Quaternion<T>::fromEulerEx(extracted.x, extracted.y, extracted.z, item.order);
                            CHECK_CLOSE_QUATERNIONS(q, qVerify);
                        }
                    }
                }
            }
            SUBCASE("Cross Order Consistency")
            {
                struct OrderMapping {
                    ObsidianEngine::EulerOrder order;
                    const char* name;
                };

                OrderMapping mappings[] = {
                    { ObsidianEngine::EulerOrder::XYZ, "XYZ" },
                    { ObsidianEngine::EulerOrder::XZY, "XZY" },
                    { ObsidianEngine::EulerOrder::YXZ, "YXZ" },
                    { ObsidianEngine::EulerOrder::YZX, "YZX" },
                    { ObsidianEngine::EulerOrder::ZXY, "ZXY" },
                    { ObsidianEngine::EulerOrder::ZYX, "ZYX" }
                };

                Vector<T, 3> initialAngles(25.0f, -40.0f, 15.0f);

                for (const auto& source : mappings)
                {
                    Quaternion<T> qSource = Quaternion<T>::fromEulerEx(initialAngles.x, initialAngles.y, initialAngles.z, source.order);

                    for (const auto& target : mappings)
                    {
                        std::string subcaseName = std::string("Convert ") + source.name + " -> " + target.name;

                        SUBCASE(subcaseName.c_str())
                        {
                            Vector<T, 3> extracted = qSource.toEulerEx(target.order);
                            Quaternion<T> qVerify = Quaternion<T>::fromEulerEx(extracted.x, extracted.y, extracted.z, target.order);
                            CAPTURE(source.name);
                            CAPTURE(target.name);
                            CHECK_CLOSE_QUATERNIONS(qSource, qVerify);
                        }
                    }
                }
            }
        }
        TEST_CASE_TEMPLATE("Quaternion<T>::toEuler", T, float, double)
        {
            SUBCASE("Same As Quaternion<T>::toEulerEx")
            {
                Quaternion<T> q = Quaternion<T>::fromEuler(20, 30, -45);
                Vector<T, 3> ref = q.toEulerEx(ObsidianEngine::EulerOrder::ZXY);
                Vector<T, 3> res = q.toEuler();

                CHECK_CLOSE(ref.x, res.x);
                CHECK_CLOSE(ref.y, res.y);
                CHECK_CLOSE(ref.z, res.z);
            }
        }
        TEST_CASE_TEMPLATE("Quaternion<T>::lookAt", T, float, double)
        {
            Vector<T, 3> worldUp(0, 1, 0);

            SUBCASE("Looking Forward")
            {
                Vector<T, 3> target(0, 0, 1);
                Quaternion<T> q = Quaternion<T>::lookRotation(target, worldUp);
                CHECK_CLOSE(q.length(), 1.0f);

                Vector<T, 3> result = q * Vector<T, 3>(0, 0, 1);
                CHECK_CLOSE_VECTOR(result, Vector<T, 3>(0, 0, 1));
            }
            SUBCASE("Looking Right")
            {
                Vector<T, 3> target(1, 0, 0);
                Quaternion<T> q = Quaternion<T>::lookRotation(target, worldUp);
                CHECK_CLOSE(q.length(), 1.0f);

                Vector<T, 3> result = q * Vector<T, 3>(0, 0, 1);
                CHECK_CLOSE_VECTOR(result, Vector<T, 3>(1, 0, 0));
            }
            SUBCASE("Looking Backwards")
            {
                Vector<T, 3> target(0, 0, -1);
                Quaternion<T> q = Quaternion<T>::lookRotation(target, worldUp);
                CHECK_CLOSE(q.length(), 1.0f);

                Vector<T, 3> result = q * Vector<T, 3>(0, 0, 1);
                CHECK_CLOSE_VECTOR(result, Vector<T, 3>(0, 0, -1));
            }
            SUBCASE("Looking Slightly Off Vertical")
            {
                Vector<T, 3> target(0.001f, 1.0f, 0.0f);
                Quaternion<T> q = Quaternion<T>::lookRotation(target, worldUp);
                CHECK_CLOSE(q.length(), 1.0f);

                Vector<T, 3> result = q * Vector<T, 3>(0, 0, 1);
                CHECK_CLOSE_VECTOR(result, Vector<T, 3>(0.001f, 1.0f, 0.0f));
            }
            SUBCASE("Zero Direction")
            {
                Quaternion<T> q = Quaternion<T>::lookRotation(Vector<T, 3>(0, 0, 0), worldUp);
                CHECK_CLOSE(q.length(), 1.0f);

                CHECK_CLOSE_QUATERNIONS(q, Quaternion<T>::identity());
            }
            SUBCASE("Looking Straight Up")
            {
                Vector<T, 3> worldUp(0, 1, 0);
                Vector<T, 3> target(0, 1, 0);

                Quaternion<T> q = Quaternion<T>::lookRotation(target, worldUp);
                CHECK_CLOSE(q.length(), 1.0f);

                Vector<T, 3> result = q * Vector<T, 3>(0, 0, 1);
                CHECK_CLOSE_VECTOR(result, Vector<T, 3>(0, 1, 0));

                CHECK(!std::isnan(q.x));
                CHECK(!std::isnan(q.y));
                CHECK(!std::isnan(q.z));
                CHECK(!std::isnan(q.w));
            }
            SUBCASE("Looking Straight Down")
            {
                Vector<T, 3> target(0, -1, 0);
                Quaternion<T> q = Quaternion<T>::lookRotation(target, worldUp);
                CHECK_CLOSE(q.length(), 1.0f);

                Vector<T, 3> result = q * Vector<T, 3>(0, 0, 1);
                CHECK_CLOSE_VECTOR(result, Vector<T, 3>(0, -1, 0));

                CHECK(!std::isnan(q.x));
                CHECK(!std::isnan(q.y));
                CHECK(!std::isnan(q.z));
                CHECK(!std::isnan(q.w));;
            }
            SUBCASE("Tilted up Vector")
            {
                Vector<T, 3> tiltedUp = Vector<T, 3>(1, 1, 0).normalized();
                Vector<T, 3> target(0, 0, 1);

                Quaternion<T> q = Quaternion<T>::lookRotation(target, tiltedUp);
                CHECK_CLOSE(q.length(), 1.0f);

                Vector<T, 3> forwardResult = q * Vector<T, 3>(0, 0, 1);
                Vector<T, 3> upResult = q * Vector<T, 3>(0, 1, 0);

                CHECK_CLOSE(forwardResult.z, 1.0f);
                CHECK_CLOSE((Vector<T, 3>::dot(upResult, forwardResult)), 0.0f);
            }
            SUBCASE("Non Unit Inputs")
            {
                Vector<T, 3> target(10.0f, 10.0f, 10.0f);
                Vector<T, 3> up(0.0f, 50.0f, 0.0f);

                Quaternion<T> q = Quaternion<T>::lookRotation(target, up);
                CHECK_CLOSE(q.length(), 1.0f);

                Vector<T, 3> result = q * Vector<T, 3>(0, 0, 1);
                Vector<T, 3> expected = target.normalized();
                CHECK_CLOSE_VECTOR(result, expected);
            }
        }
        TEST_CASE_TEMPLATE("Quaternion<T> Normalization", T, float, double)
        {
            SUBCASE("Length and LengthSquared")
            {
                Quaternion<T> q(1.0f, 1.0f, 1.0f, 1.0f);
                CHECK_CLOSE(q.lengthSquared(), 4.0f);
                CHECK_CLOSE(q.length(), 2.0f);
            }
            SUBCASE("Normalize A Quaternion<T>")
            {
                Quaternion<T> q(1.0f, 1.0f, 1.0f, 1.0f);
                q.normalize();

                CHECK(q.length() == doctest::Approx(1.0f));

                CHECK_CLOSE(q.x, 0.5f);
                CHECK_CLOSE(q.y, 0.5f);
                CHECK_CLOSE(q.z, 0.5f);
                CHECK_CLOSE(q.w, 0.5f);
            }
            SUBCASE("Normalized (Const version)")
            {
                const Quaternion<T> q(0.0f, 10.0f, 0.0f, 0.0f);
                Quaternion<T> unit = q.normalized();

                CHECK_CLOSE(q.length(), 10.0f);
                CHECK_CLOSE(unit.length(), 1.0f);
                CHECK_CLOSE(unit.y, 1.0f);
            }
            SUBCASE("Zero Magnitude Safety")
            {
                Quaternion<T> q(0, 0, 0, 0);
                q.normalize();

                CHECK_QUATERNIONS(q, Quaternion<T>::identity());
            }
            SUBCASE("Already Normalized")
            {
                Quaternion<T> q = Quaternion<T>::identity();
                q.normalize();

                CHECK_CLOSE(q.w , 1.0f);
                CHECK_CLOSE(q.length(), 1.0f);
            }
        }
        TEST_CASE_TEMPLATE("Quaternion<T>::conjugate", T, float, double)
        {
            SUBCASE("Conjugate")
            {
                Quaternion<T> q(1.0f, 2.0f, 3.0f, 4.0f);
                Quaternion<T> c = q.conjugate();

                CHECK(c.x == -1.0f);
                CHECK(c.y == -2.0f);
                CHECK(c.z == -3.0f);
                CHECK(c.w == 4.0f);
            }
        }
        TEST_CASE_TEMPLATE("Quaternion<T>::inverse", T, float, double)
        {
            SUBCASE("Inverse Unit Quaternion<T>")
            {
                Quaternion<T> q = Quaternion<T>::fromAxisAngle(Vector<T, 3>(0, 1, 0), 90.0f);
                Quaternion<T> inv = q.inverse();

                Vector<T, 3> v(1, 0, 0);
                Vector<T, 3> result = (q * v);
                result = (inv * result);

                CHECK_CLOSE(result.x, 1.0f);
                CHECK_CLOSE(result.y, 0.0f);
                CHECK_CLOSE(result.z, 0.0f);
            }

            SUBCASE("Inverse Non Unit Quaternion<T>")
            {
                Quaternion<T> q(1.0f, 1.0f, 1.0f, 1.0f);
                Quaternion<T> inv = q.inverse();

                CHECK_CLOSE(inv.x, -0.25f);
                CHECK_CLOSE(inv.y, -0.25f);
                CHECK_CLOSE(inv.z, -0.25f);
                CHECK_CLOSE(inv.w, 0.25f);
            }

            SUBCASE("Inverse Zero Quaternion<T>")
            {
                Quaternion<T> q(0, 0, 0, 0);
                Quaternion<T> inv = q.inverse();

                CHECK_QUATERNIONS(inv, Quaternion<T>::identity());
            }

            SUBCASE("Inverse of Inverse")
            {
                Quaternion<T> q = Quaternion<T>::fromAxisAngle(Vector<T, 3>(1, 2, 3).normalized(), 45.0f);
                Quaternion<T> inv = q.inverse();

                Quaternion<T> finalQ = inv.inverse();

                CHECK_CLOSE_QUATERNIONS(finalQ, q);
            }
            SUBCASE("Identity Property Unit Quaternion<T>")
            {
                Quaternion<T> q1 = Quaternion<T>::fromAxisAngle(Vector<T, 3>(1, 0.5f, 0.2f).normalized(), 42.0f);
                Quaternion<T> inv1 = q1.inverse();

                Quaternion<T> result1 = q1 * inv1;

                SUBCASE("Unit Quaternion<T> is Identity")
                {
                    CHECK_CLOSE_QUATERNIONS(result1, Quaternion<T>::identity());
                }
            }
            SUBCASE("Identity Property Non Unit Quaternion<T>")
            {
                Quaternion<T> q2(2.0f, 2.0f, 2.0f, 2.0f);
                Quaternion<T> inv2 = q2.inverse();

                Quaternion<T> result2 = q2 * inv2;

                SUBCASE("Non Unit Quaternion<T> is Identity")
                {
                    CHECK_CLOSE_QUATERNIONS(result2, Quaternion<T>::identity());
                }
            }
            SUBCASE("Relative Rotation")
            {
                Quaternion<T> start = Quaternion<T>::fromAxisAngle(Vector<T, 3>(0, 1, 0), 10.0f);
                Quaternion<T> end = Quaternion<T>::fromAxisAngle(Vector<T, 3>(0, 1, 0), 30.0f);

                Quaternion<T> delta = end * start.inverse();

                T angle;
                Vector<T, 3> axis;
                delta.toAxisAngle(axis, angle);

                CHECK_CLOSE(angle, 20.0f);
            }
        }
        TEST_CASE_TEMPLATE("Quaternion<T>::VectorTransform", T, float, double)
        {
            SUBCASE("Rotate Identity")
            {
                Quaternion<T> q = Quaternion<T>::identity();
                Vector<T, 3> v(1, 2, 3);
                Vector<T, 3> result = q.rotate(v);

                CHECK_CLOSE_VECTOR(result, v);
            }
            SUBCASE("Rotate 90 Degrees Around Y")
            {
                Quaternion<T> q = Quaternion<T>::fromAxisAngle(Vector<T, 3>(0, 1, 0), 90.0f);

                Vector<T, 3> f = q.forward();
                Vector<T, 3> u = q.up();
                Vector<T, 3> r = q.right();

                CHECK_CLOSE_VECTOR(f, Vector<T, 3>(1, 0, 0));
                CHECK_CLOSE_VECTOR(u, Vector<T, 3>(0, 1, 0));
                CHECK_CLOSE_VECTOR(r, Vector<T, 3>(0, 0, -1));
            }
            SUBCASE("Rotate 180 Degrees Around X")
            {
                Quaternion<T> q = Quaternion<T>::fromAxisAngle(Vector<T, 3>(1, 0, 0), 180.0f);

                Vector<T, 3> f = q.forward();
                Vector<T, 3> u = q.up();
                Vector<T, 3> r = q.right();

                CHECK_CLOSE_VECTOR(f, Vector<T, 3>(0, 0, -1));
                CHECK_CLOSE_VECTOR(u, Vector<T, 3>(0, -1, 0));
                CHECK_CLOSE_VECTOR(r, Vector<T, 3>(1, 0, 0));
            }
            SUBCASE("Rotate Non Unity Vector")
            {
                Quaternion<T> q = Quaternion<T>::fromAxisAngle(Vector<T, 3>(0, 0, 1), 90.0f);
                Vector<T, 3> v(0, 10, 0);
                Vector<T, 3> result = q.rotate(v);

                CHECK_CLOSE_VECTOR(result, Vector<T, 3>(-10, 0, 0));
            }
        }
        TEST_CASE_TEMPLATE("Quaternion<T>::Dot", T, float, double)
        {
            SUBCASE("Dot with Identity")
            {
                Quaternion<T> q1 = Quaternion<T>::identity();
                Quaternion<T> q2 = Quaternion<T>::identity();

                CHECK_CLOSE(Quaternion<T>::dot(q1, q2), 1.0f);
                CHECK_CLOSE(q1.dot(q2), 1.0f);
            }
            SUBCASE("Orthogonal Quaternion<T>s")
            {
                Quaternion<T> q1(1, 0, 0, 0);
                Quaternion<T> q2(0, 1, 0, 0);

                CHECK_CLOSE(Quaternion<T>::dot(q1, q2), 0.0f);
                CHECK_CLOSE(q1.dot(q2), 0.0f);
            }
            SUBCASE("Opposite Orientations (Double Cover)")
            {
                Quaternion<T> q1(0, 0, 0, 1);
                Quaternion<T> q2(0, 0, 0, -1);

                CHECK_CLOSE(Quaternion<T>::dot(q1, q2), -1.0f);
                CHECK_CLOSE(q1.dot(q2), -1.0f);
            }
            SUBCASE("General Values")
            {
                Quaternion<T> q1(1, 2, 3, 4);
                Quaternion<T> q2(5, 6, 7, 8);

                CHECK_CLOSE(Quaternion<T>::dot(q1, q2), 70.0f);
                CHECK_CLOSE(q1.dot(q2), 70.0f);
            }
        }
        TEST_CASE_TEMPLATE("Quaternion<T>::NLerp", T, float, double)
        {
            Quaternion<T> q1 = Quaternion<T>::identity();
            Quaternion<T> q2 = Quaternion<T>::fromAxisAngle(Vector<T, 3>(0, 1, 0), 90.0f);

            SUBCASE("Midpoint")
            {
                Quaternion<T> result = Quaternion<T>::nlerp(q1, q2, 0.5f);
                CHECK_CLOSE(result.lengthSquared(), 1.0f);

                T angle;
                Vector<T, 3> axis;
                result.toAxisAngle(axis, angle);

                CHECK_CLOSE(angle, 45.0f);
                CHECK_CLOSE_VECTOR(axis, Vector<T, 3>(0, 1, 0));
            }
            SUBCASE("Short Path Logic")
            {
                Quaternion<T> q2Neg(-q2.x, -q2.y, -q2.z, -q2.w);
                Quaternion<T> result = Quaternion<T>::nlerp(q1, q2Neg, 0.5f);

                T angle;
                Vector<T, 3> axis;
                result.toAxisAngle(axis, angle);

                CHECK_CLOSE(angle, 45.0f);
            }

            SUBCASE("Clamping")
            {
                Quaternion<T> result = Quaternion<T>::nlerp(q1, q2, 1.5f);
                CHECK_CLOSE_QUATERNIONS(result, q2);

                Quaternion<T> resultUnclamped = Quaternion<T>::nlerpUnclamped(q1, q2, 2.0f);
                CHECK(resultUnclamped.w != doctest::Approx(q2.w));
            }
        }
        TEST_CASE_TEMPLATE("Quaternion<T>::slerp", T, float, double)
        {
            Quaternion<T> identity = Quaternion<T>::identity();
            Quaternion<T> rotY90 = Quaternion<T>::fromAxisAngle(Vector<T, 3>(0, 1, 0), 90.0f);

            SUBCASE("Endpoints")
            {
                Quaternion<T> identity = Quaternion<T>::identity();
                Quaternion<T> rotY90 = Quaternion<T>::fromAxisAngle(Vector<T, 3>(0, 1, 0), 90.0f);

                Quaternion<T> start = Quaternion<T>::slerp(identity, rotY90, 0.0f);
                CHECK_CLOSE_QUATERNIONS(start, Quaternion<T>::identity());

                Quaternion<T> end = Quaternion<T>::slerp(identity, rotY90, 1.0f);
                CHECK_CLOSE_QUATERNIONS(end, rotY90);
            }
            SUBCASE("Midpoint")
            {
                Quaternion<T> identity = Quaternion<T>::identity();
                Quaternion<T> rotY90 = Quaternion<T>::fromAxisAngle(Vector<T, 3>(0, 1, 0), 90.0f);

                Quaternion<T> mid = Quaternion<T>::slerp(identity, rotY90, 0.5f);

                T rad225 = Math<T>::val(22.5f * (3.1415926535f / 180.0f));
                T expectedY = std::sin(rad225);
                T expectedW = std::cos(rad225);

                CHECK_CLOSE(mid.x, 0.0f);
                CHECK_CLOSE(mid.y, expectedY);
                CHECK_CLOSE(mid.z, 0.0f);
                CHECK_CLOSE(mid.w, expectedW);
            }
            SUBCASE("Shortest Path")
            {
                Quaternion<T> q1 = Quaternion<T>::identity();
                Quaternion<T> q2 = -rotY90;

                Quaternion<T> mid = Quaternion<T>::slerp(q1, q2, 0.5f);

                T expectedW = Math<T>::val(std::cos(22.5f * (3.14159f / 180.0f)));
                CHECK_CLOSE(std::abs(mid.w), expectedW);
            }
            SUBCASE("Extremely Close Rotations")
            {
                Quaternion<T> q1 = Quaternion<T>::identity();
                Quaternion<T> q2 = Quaternion<T>(0.00001f, 0, 0, 1.0f).normalized();

                Quaternion<T> result = Quaternion<T>::slerp(q1, q2, 0.5f);

                CHECK_FALSE(std::isnan(result.w));
                CHECK_CLOSE(result.w, 1.0f);
            }
            SUBCASE("Slerp Clamping and Extrapolation")
            {
                Quaternion<T> q1 = Quaternion<T>::identity();
                Quaternion<T> q2 = Quaternion<T>::fromAxisAngle(Vector<T, 3>(0, 1, 0), 90.0f);

                Quaternion<T> resultClamped = Quaternion<T>::slerp(q1, q2, 1.5f);

                CHECK_CLOSE_QUATERNIONS(resultClamped, q2);

                Quaternion<T> resultUnclamped = Quaternion<T>::slerpUnclamped(q1, q2, 2.0f);

                CHECK_CLOSE(resultUnclamped.x, 0.0f);
                CHECK_CLOSE(resultUnclamped.y, 1.0f);
                CHECK_CLOSE(resultUnclamped.z, 0.0f);
                CHECK_CLOSE(resultUnclamped.w, 0.0f);

                CHECK(resultUnclamped.w != doctest::Approx(q2.w));
                CHECK(resultUnclamped.y != doctest::Approx(q2.y));
            }
        }
        TEST_CASE_TEMPLATE("Quaternion<T>::toMatrix", T, float, double)
        {
            SUBCASE("Identity Mapping")
            {
                Quaternion<T> q = Quaternion<T>::identity();
                Matrix4x4<T> m = q.toMatrix();
                CHECK_CLOSE_MATRIX(m, (Matrix4x4<T>::identity()));
            }
            SUBCASE("90 Degree Rotation Along X-Axis")
            {
                Quaternion<T> q = Quaternion<T>::fromEuler(90.0f, 0.0f, 0.0f);
                Matrix4x4<T> m = q.toMatrix();

                Matrix4x4<T> expected = Matrix4x4<T>::identity();
                expected(1, 1) = 0.0f; 
                expected(1, 2) = -1.0f;
                expected(2, 1) = 1.0f;  
                expected(2, 2) = 0.0f;

                CHECK_CLOSE_MATRIX(m, expected);
            }
            SUBCASE("90 Degree Rotation Along Y-Axis")
            {
                Quaternion<T> q = Quaternion<T>::fromEuler(0.0f, 90.0f, 0.0f);
                Matrix4x4<T> m = q.toMatrix();

                Matrix4x4<T> expected = Matrix4x4<T>::identity();
                expected(0, 0) = 0.0f;  
                expected(0, 2) = 1.0f;
                expected(2, 0) = -1.0f; 
                expected(2, 2) = 0.0f;

                CHECK_CLOSE_MATRIX(m, expected);
            }
            SUBCASE("90 Degree Rotation Along Z-Axis")
            {
                Quaternion<T> q = Quaternion<T>::fromEuler(0.0f, 0.0f, 90.0f);
                Matrix4x4<T> m = q.toMatrix();

                Matrix4x4<T> expected = Matrix4x4<T>::identity();
                expected(0, 0) = 0.0f; 
                expected(0, 1) = -1.0f;
                expected(1, 0) = 1.0f;  
                expected(1, 1) = 0.0f;
                expected(2, 2) = 1.0f;

                CHECK_CLOSE_MATRIX(m, expected);
            }
        }
        TEST_CASE_TEMPLATE("Quaternion<T>::fromMatrix", T, float, double)
        {
            SUBCASE("Branch 1: Positive Trace (qw is largest)")
            {
                Matrix4x4<T> m = Matrix4x4<T>::rotateX(0.1f);
                Quaternion<T> q = Quaternion<T>::fromMatrix(m);

                CHECK_CLOSE_MATRIX(q.toMatrix(), m);
            }

            SUBCASE("Branch 2: Row 0 is largest")
            {
                Matrix4x4<T> m = Matrix4x4<T>::rotateX(Math<T>::Pi);
                Quaternion<T> q = Quaternion<T>::fromMatrix(m);

                CHECK_CLOSE_MATRIX(q.toMatrix(), m);
            }

            SUBCASE("Branch 3: Row 1 is largest")
            {
                Matrix4x4<T> m = Matrix4x4<T>::rotateY(Math<T>::Pi);
                Quaternion<T> q = Quaternion<T>::fromMatrix(m);

                CHECK_CLOSE_MATRIX(q.toMatrix(), m);
            }
            SUBCASE("Branch 4: Row 2 is largest")
            {
                Matrix4x4<T> m = Matrix4x4<T>::rotateZ(Math<T>::Pi);
                Quaternion<T> q = Quaternion<T>::fromMatrix(m);

                CHECK_CLOSE_MATRIX(q.toMatrix(), m);
            }
            SUBCASE("Ignoring Translation")
            {
                Matrix4x4<T> m = Matrix4x4<T>::translate({ 10.0f, -5.0f, 20.0f }) * Matrix4x4<T>::rotateX(0.5f);
                Quaternion<T> q = Quaternion<T>::fromMatrix(m);
                Quaternion<T> expected = Quaternion<T>::fromEulerEx(0.5f * Math<T>::Rad2Deg, 0.0f, 0.0f, ObsidianEngine::EulerOrder::XYZ);

                T dot = q.x * expected.x + q.y * expected.y + q.z * expected.z + q.w * expected.w;
                CHECK(std::abs(dot) > 0.999f);
            }
            SUBCASE("Random Rotation Integrity")
            {
                Vector<T, 3> testOrientations[] = {
                    { 10.0f, 20.0f, 30.0f },
                    { 90.0f, 45.0f, 0.0f },
                    { 180.0f, 0.0f, 90.0f },
                    { -45.0f, 120.0f, -30.0f }
                };

                for (const auto& angles : testOrientations)
                {
                    Quaternion<T> qOriginal = Quaternion<T>::fromEuler(angles.x, angles.y, angles.z);
                    Matrix4x4<T> m = qOriginal.toMatrix();
                    Quaternion<T> qReconstructed = Quaternion<T>::fromMatrix(m);
                    T dot = qOriginal.x * qReconstructed.x + qOriginal.y * qReconstructed.y + qOriginal.z * qReconstructed.z + qOriginal.w * qReconstructed.w;

                    CAPTURE(angles);
                    CAPTURE(qOriginal);
                    CAPTURE(qReconstructed);

                    CHECK_CLOSE(std::abs(dot), 0.9999f);
                }
            }
            SUBCASE("Numerical Stability near 180 degrees")
            {
                Matrix4x4<T> m = Matrix4x4<T>::rotateY(Math<T>::Pi -0.001f);

                Quaternion<T> q = Quaternion<T>::fromMatrix(m);
                Matrix4x4<T> mBack = q.toMatrix();

                CHECK_CLOSE_MATRIX(m, mBack, 0.001f);
            }
        }
        TEST_CASE_TEMPLATE("Quaternion<T> Arithmetic", T, float, double)
        {
            Quaternion<T> q1(1, 2, 3, 4);
            Quaternion<T> q2(5, 6, 7, 8);

            SUBCASE("Binary Addition and Subtraction")
            {
                Quaternion<T> resAdd = q1 + q2;
                CHECK_CLOSE(resAdd.x, 6); 
                CHECK_CLOSE(resAdd.y, 8);
                CHECK_CLOSE(resAdd.z, 10); 
                CHECK_CLOSE(resAdd.w, 12);

                Quaternion<T> resSub = q2 - q1;
                CHECK_CLOSE(resSub.x, 4); 
                CHECK_CLOSE(resSub.y, 4);
                CHECK_CLOSE(resSub.z, 4); 
                CHECK_CLOSE(resSub.w, 4);
            }

            SUBCASE("Assignment Addition and Subtraction")
            {
                Quaternion<T> q = q1;
                q += q2;
                CHECK_CLOSE_QUATERNIONS(q, (q1 + q2));

                q -= q2;
                CHECK_CLOSE_QUATERNIONS(q, q1);
            }

            SUBCASE("Negation")
            {
                Quaternion<T> neg = -q1;
                CHECK_CLOSE(neg.x, -1); 
                CHECK_CLOSE(neg.y, -2);
                CHECK_CLOSE(neg.z, -3); 
                CHECK_CLOSE(neg.w, -4);
            }
        }
        TEST_CASE_TEMPLATE("Quaternion<T> Multiplication", T, float, double)
        {
            Quaternion<T> qX = Quaternion<T>::fromAxisAngle(Vector<T, 3>(1, 0, 0), 90.0f);
            Quaternion<T> qY = Quaternion<T>::fromAxisAngle(Vector<T, 3>(0, 1, 0), 90.0f);

            SUBCASE("Quaternion<T> * Quaternion<T>")
            {
                Quaternion<T> combined = qX * qY;
                CHECK_CLOSE(combined.x, 0.5f);
                CHECK_CLOSE(combined.y, 0.5f);
                CHECK_CLOSE(combined.z, 0.5f);
                CHECK_CLOSE(combined.w, 0.5f);
            }

            SUBCASE("Scalar Multiplication")
            {
                Quaternion<T> q(1, 1, 1, 1);
                Quaternion<T> res = q.mulScalar(2.0f);
                CHECK_CLOSE(res.x, 2.0f);

                Quaternion<T> resLeft = res.mulScalar(0.5f);
                CHECK_CLOSE(resLeft.x, 1.0f);

                q.mulScalarAssign(3.0f);
                CHECK_CLOSE(q.x, 3.0f);
            }
            SUBCASE("Quaternion<T> * Vector<T, 3> (Rotate)")
            {
                Quaternion<T> q = Quaternion<T>::fromAxisAngle(Vector<T, 3>(0, 1, 0), 90.0f);
                Vector<T, 3> v(0, 0, 1);
                Vector<T, 3> rotated = q * v;

                CHECK_CLOSE(rotated.x, 1.0f);
                CHECK_CLOSE(rotated.y, 0.0f);
                CHECK_CLOSE(rotated.z, 0.0f);
            }
            SUBCASE("Composition Validation")
            {
                Quaternion<T> qX = Quaternion<T>::fromAxisAngle(Vector<T, 3>(1, 0, 0), 90.0f);
                Quaternion<T> qY = Quaternion<T>::fromAxisAngle(Vector<T, 3>(0, 1, 0), 90.0f);
                Vector<T, 3> v(0, 0, 1); 

                Vector<T, 3> step1 = qX * v;
                Vector<T, 3> finalA = qY * step1;

                Quaternion<T> combined = qY * qX;
                Vector<T, 3> finalB = combined * v;

                CHECK_CLOSE(finalB.x, finalA.x);
                CHECK_CLOSE(finalB.y, finalA.y);
                CHECK_CLOSE(finalB.z, finalA.z);
            }
        }
        TEST_CASE_TEMPLATE("Quaternion<T> Utility", T, float, double)
        {
            Quaternion<T> q(1, 2, 3, 4);

            SUBCASE("Subscript Operator")
            {
                CHECK_CLOSE(q[0], q.x);
                CHECK_CLOSE(q[1], q.y);
                CHECK_CLOSE(q[2], q.z);
                CHECK_CLOSE(q[3], q.w);

                q[0] = 10;
                CHECK_CLOSE(q.x, 10);

                q[1] = 10;
                CHECK_CLOSE(q.y, 10);

                q[2] = 10;
                CHECK_CLOSE(q.z, 10);

                q[3] = 10;
                CHECK_CLOSE(q.w, 10);
            }
            SUBCASE("Equality")
            {
                Quaternion<T> same(1, 2, 3, 4);
                Quaternion<T> different(0, 2, 3, 4);

                CHECK_CLOSE(q == same);
                CHECK_CLOSE(q != different);
                CHECK_FALSE(q == different);
            }
        }
    }
}