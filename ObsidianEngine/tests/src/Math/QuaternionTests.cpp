#include "doctest/doctest.h"

#include <string>

#include "helpers.h"

#include "ObsidianEngine/Math/Math.h"

using namespace ObsidianEngine::detail;

TEST_SUITE("Math")
{
    TEST_SUITE("Quaternion")
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
                Quaternion q = Quaternion::identity();
                Quaternion conj = q.conjugate();
                CHECK_QUATERNION(conj, 0.0f, 0.0f, 0.0f, 1.0f);
            }
            SUBCASE("Identity Multplication")
            {
                Quaternion q(0.3f, 0.5f, 0.6f, 0.1f);
                Quaternion id = Quaternion::identity();
                Quaternion result = q * id;
                CHECK_QUATERNIONS(result, q);
            }
        }
        TEST_CASE("Quaternion::fromAxisAngle")
        {
            SUBCASE("Standard Rotation")
            {
                float angle = 90.0f;
                Vector3 axis(0, 1, 0);
                Quaternion q = Quaternion::fromAxisAngle(axis, angle);
                CHECK_CLOSE_QUATERNION(q, 0.0f, 0.707106f, 0.0f, 0.707106f);
                CHECK_CLOSE(q.length(), 1.0f);
            }
            SUBCASE("Zero Angle")
            {
                float angle = 0.0f;
                Vector3 axis(0, 1, 0);
                Quaternion q = Quaternion::fromAxisAngle(axis, angle);
                CHECK_QUATERNIONS(q, Quaternion::identity());
                CHECK_CLOSE(q.length(), 1.0f);
            }
            SUBCASE("Full Rotation")
            {
                float angle = 360.0f;
                Vector3 axis(0, 1, 0);
                Quaternion q = Quaternion::fromAxisAngle(axis, angle);
                CHECK_CLOSE(q.x, 0.0f);
                CHECK_CLOSE(q.y, 0.0f);
                CHECK_CLOSE(q.z, 0.0f);
                CHECK_CLOSE(std::abs(q.w), 1.0f);
                CHECK_CLOSE(q.length(), 1.0f);
            }
            SUBCASE("Non Unit Axis")
            {
                float angle = 90.0f;
                Vector3 axis(0, 2, 0);
                Quaternion q = Quaternion::fromAxisAngle(axis, angle);
                CHECK_CLOSE_QUATERNION(q, 0.0f, 0.707106f, 0.0f, 0.707106f);
                CHECK_CLOSE(q.length(), 1.0f);
            }
            SUBCASE("Negative Angle")
            {
                float angle = -90.0f;
                Vector3 axis(0, 1, 0);
                Quaternion q = Quaternion::fromAxisAngle(axis, angle);
                CHECK_CLOSE_QUATERNION(q, 0.0f, -0.707106f, 0.0f, 0.707106f);
                CHECK_CLOSE(q.length(), 1.0f);
            }
            SUBCASE("Large Angle Wraparound")
            {
                float angle = 720.0f;
                Vector3 axis(0, 1, 0);
                Quaternion q = Quaternion::fromAxisAngle(axis, angle);
                CHECK_CLOSE_QUATERNION(q, 0.0f, 0.0f, 0.0f, 1.0f);
                CHECK_CLOSE(q.length(), 1.0f);
            }
            SUBCASE("180 Degree Precision")
            {
                float angle = 180.0f;
                Vector3 axis(0, 1, 0);
                Quaternion q = Quaternion::fromAxisAngle(axis, angle);
                CHECK_CLOSE_QUATERNION(q, 0.0f, 1.0f, 0.0f, 0.0f);
                CHECK_CLOSE(q.length(), 1.0f);
            }
            SUBCASE("Opposite Directions")
            {
                using Vec3 = detail::Vector<float, 3>;

                Quaternion qPos = Quaternion::fromAxisAngle(Vec3(1, 0, 0), 90.0f);
                Quaternion qNeg = Quaternion::fromAxisAngle(Vec3(1, 0, 0), -90.0f);

                CHECK_CLOSE(qPos.x, -qNeg.x);
                CHECK_CLOSE(qPos.y, qNeg.y);
                CHECK_CLOSE(qPos.z, qNeg.z);
                CHECK_CLOSE(qPos.w, qNeg.w);

                CHECK_CLOSE(qPos.length(), 1.0f);
                CHECK_CLOSE(qNeg.length(), 1.0f);
            }
            SUBCASE("Zero Axis")
            {
                Vector3 zeroAxis(0.0f, 0.0f, 0.0f);
                Quaternion q = Quaternion::fromAxisAngle(zeroAxis, 90.0f);
                CHECK_CLOSE_QUATERNIONS(q, Quaternion::identity());
                CHECK_CLOSE(q.length(), 1.0f);
            }
            SUBCASE("Diagonal Axis Rotation")
            {
                Vector3 axis(1.0f, 1.0f, 1.0f);
                float angle = 120.0f;
                Quaternion q = Quaternion::fromAxisAngle(axis, angle);
                CHECK_CLOSE_QUATERNION(q, 0.5f, 0.5f, 0.5f, 0.5f);
                CHECK_CLOSE(q.length(), 1.0f);
            }
        }
        TEST_CASE("Quaternion::toAxisAngle")
        {
            SUBCASE("Standard Rotation")
            {
                Vector3 axis(0, 1, 0);
                float angle = 90.0f;
                Quaternion q = Quaternion::fromAxisAngle(axis, angle);

                Vector3 outAxis;
                float outAngle;
                q.toAxisAngle(outAxis, outAngle);

                CHECK_CLOSE(outAngle, angle);
                CHECK_CLOSE(outAxis.x, axis.x);
                CHECK_CLOSE(outAxis.y, axis.y);
                CHECK_CLOSE(outAxis.z, axis.z);
            }
            SUBCASE("Identity Quaternion")
            {
                Quaternion q = Quaternion::identity();
                Vector3 outAxis;
                float outAngle;
                q.toAxisAngle(outAxis, outAngle);

                CHECK_CLOSE(outAngle, 0.0f);
                CHECK_CLOSE(outAxis.x, 1.0f);
                CHECK_CLOSE(outAxis.y, 0.0f);
                CHECK_CLOSE(outAxis.z, 0.0f);
                CHECK(!std::isnan(outAngle));
            }
            SUBCASE("Non Unit Quaternion")
            {
                Quaternion q(0.0f, 0.0f, 0.0f, 2.0f);

                Vector3 outAxis;
                float outAngle;
                q.toAxisAngle(outAxis, outAngle);

                CHECK(!std::isnan(outAngle));
                CHECK(!std::isnan(outAxis.x));
                CHECK_CLOSE(outAngle, 0.0f);
            }
            SUBCASE("180 Degree Flip")
            {
                Vector3 axis(1, 0, 0);
                float angle = 180.0f;
                Quaternion q = Quaternion::fromAxisAngle(axis, angle);

                Vector3 outAxis;
                float outAngle;
                q.toAxisAngle(outAxis, outAngle);

                CHECK_CLOSE(outAngle, angle);
                CHECK_CLOSE(outAxis.x, axis.x);
                CHECK_CLOSE(outAxis.y, axis.y);
                CHECK_CLOSE(outAxis.z, axis.z);
            }
            SUBCASE("Round Trip Consistency")
            {
                Vector3 originalAxis = Vector3(1, 2, 3).normalized();
                float originalAngle = 45.0f;

                Quaternion q = Quaternion::fromAxisAngle(originalAxis, originalAngle);

                Vector3 resultAxis;
                float resultAngle;
                q.toAxisAngle(resultAxis, resultAngle);

                CHECK_CLOSE(resultAngle, originalAngle);
                CHECK_CLOSE(resultAxis.x, originalAxis.x);
                CHECK_CLOSE(resultAxis.y, originalAxis.y);
                CHECK_CLOSE(resultAxis.z, originalAxis.z);
            }
        }
        TEST_CASE("Quaternion::angle")
        {
            SUBCASE("Zero Difference")
            {
                Quaternion q1 = Quaternion::fromEuler(10, 20, 30);
                Quaternion q2 = q1;
                float angle = Quaternion::angle(q1, q2);
                CHECK_CLOSE(angle, 0.0f);
            }
            SUBCASE("Opposite Sign")
            {
                Quaternion q1(0.182574f, 0.365148f, 0.547723f, 0.730297f);
                Quaternion q2(-q1.x, -q1.y, -q1.z, -q1.w);

                CHECK(Quaternion::angle(q1, q2) == doctest::Approx(0.0f));
            }
            SUBCASE("Safety Clamp")
            {
                Quaternion q1(0, 0, 0, 1.0f);
                Quaternion q2(0, 0, 0, 1.0000001f);

                float result = Quaternion::angle(q1, q2);
                CHECK_FALSE(std::isnan(result));
                CHECK_CLOSE(result, 0.0f);
            }
            SUBCASE("Known Angles")
            {
                Quaternion qId = Quaternion::identity();

                Quaternion q90 = Quaternion::fromAxisAngle(Vector3(1, 0, 0), 90.0f);
                CHECK_CLOSE(Quaternion::angle(qId, q90), 90.0f);

                Quaternion q180 = Quaternion::fromAxisAngle(Vector3(0, 0, 1), 180.0f);
                CHECK_CLOSE(Quaternion::angle(qId, q180), 180.0f);
            }
            SUBCASE("Near 180 Degrees")
            {
                Quaternion q1 = Quaternion::fromAxisAngle(Vector3(1, 0, 0), 0.0f);
                Quaternion q2 = Quaternion::fromAxisAngle(Vector3(1, 0, 0), 179.99f);

                float result = Quaternion::angle(q1, q2);
                CHECK_CLOSE(result, 179.99f);
            }
            SUBCASE("Exactly 180 Degrees")
            {
                Quaternion q1 = Quaternion::identity();
                Quaternion q2 = Quaternion::fromAxisAngle(Vector3(0, 1, 0), 180.0f);
                float result = Quaternion::angle(q1, q2);
                CHECK_CLOSE(result, 180.0f);
            }
            SUBCASE("Small Angle Difference")
            {
                float smallAngle = 0.1f;
                Quaternion q1 = Quaternion::identity();
                Quaternion q2 = Quaternion::fromAxisAngle(Vector3(0, 1, 0), smallAngle);

                float result = Quaternion::angle(q1, q2);

                CHECK_CLOSE(result, smallAngle, 0.01);
            }
        }
        TEST_CASE("Quaternion::rotateTowards")
        {
            Quaternion from = Quaternion::identity();
            Quaternion to = Quaternion::fromAxisAngle(Vector3(0, 1, 0), 90.0f);

            SUBCASE("Partial Rotation")
            {
                float maxDelta = 30.0f;
                Quaternion result = Quaternion::rotateTowards(from, to, maxDelta);
                float angleMoved = Quaternion::angle(from, result);

                CHECK_CLOSE(angleMoved, 30.0f);
                CHECK_CLOSE(Quaternion::angle(result, to), 60.0f);
            }
            SUBCASE("Overshooting (Clamping to target)")
            {
                float maxDelta = 120.0f;
                Quaternion result = Quaternion::rotateTowards(from, to, maxDelta);

                CHECK_QUATERNIONS(result, to);
                CHECK_CLOSE(Quaternion::angle(result, to), 0.0f);
            }
            SUBCASE("Already at Target")
            {
                Quaternion result = Quaternion::rotateTowards(to, to, 10.0f);

                CHECK_QUATERNIONS(result, to);
                CHECK_CLOSE(Quaternion::angle(result, to), 0.0f);
            }
            SUBCASE("Small Delta Accumulation")
            {
                float smallDelta = 0.00001f;
                Quaternion result = from;
                int iterations = 500000;

                for (int i = 0; i < iterations; i++)
                {
                    result = Quaternion::rotateTowards(result, to, smallDelta);
                }

                float totalMoved = Quaternion::angle(from, result);

                CHECK(totalMoved > 0.1f);
                CHECK(result.w < 1.0f);
            }
        }
        TEST_CASE("Quaternion::fromToRotation")
        {
            SUBCASE("Identical Directions")
            {
                Vector3 v1(1.0f, 0.0f, 0.0f);
                Vector3 v2(1.0f, 0.0f, 0.0f);
                Quaternion q = Quaternion::fromToRotation(v1, v2);

                CHECK_QUATERNIONS(q, Quaternion::identity());
                CHECK_CLOSE(q.length(), 1.0f);
            }
            SUBCASE("Opposite Directions")
            {
                Vector3 v1(1.0f, 0.0f, 0.0f);
                Vector3 v2(-1.0f, 0.0f, 0.0f);

                Quaternion q = Quaternion::fromToRotation(v1, v2);
                CHECK_CLOSE(q.w, 0.0f);
                CHECK_CLOSE(q.length(), 1.0f);

                Vector3 result = q * v1;
                CHECK_CLOSE(result.x, v2.x);
                CHECK_CLOSE(result.y, v2.y);
                CHECK_CLOSE(result.z, v2.z);

                Vector3 v3(0.0f, 1.0f, 0.0f);
                Vector3 v4(0.0f, -1.0f, 0.0f);
                Quaternion q2 = Quaternion::fromToRotation(v3, v4);
                CHECK_CLOSE(q2.w, 0.0f);

                Vector3 result2 = q2 * v3;
                CHECK_CLOSE(result2.y, v4.y);
            }
            SUBCASE("90 Degree Rotation")
            {
                Vector3 v1(1.0f, 0.0f, 0.0f);
                Vector3 v2(0.0f, 1.0f, 0.0f);
                Quaternion q = Quaternion::fromToRotation(v1, v2);

                CHECK_CLOSE_QUATERNION(q, 0.0f, 0.0f, 0.707106f, 0.707106f);
                CHECK_CLOSE(q.length(), 1.0f);

                Vector3 result = q * v1;
                CHECK_CLOSE(result.x, v2.x);
                CHECK_CLOSE(result.y, v2.y);
                CHECK_CLOSE(result.z, v2.z);
            }
            SUBCASE("Non Unit Input Vectors")
            {
                Vector3 v1(2.0f, 0.0f, 0.0f);
                Vector3 v2(0.0f, 5.0f, 0.0f);
                Quaternion q = Quaternion::fromToRotation(v1, v2);

                CHECK_CLOSE_QUATERNION(q, 0.0f, 0.0f, 0.707106f, 0.707106f);
                CHECK_CLOSE(q.length(), 1.0f);
            }
            SUBCASE("Small Rotation")
            {
                Vector3 v1(1.0f, 1.0f, 0.0f);
                Vector3 v2(1.0f, 1.1f, 0.0f);
                v1.normalize();
                v2.normalize();

                Quaternion q = Quaternion::fromToRotation(v1, v2);
                Vector3 result = q * v1;

                CHECK_CLOSE(result.x, v2.x);
                CHECK_CLOSE(result.y, v2.y);
                CHECK_CLOSE(result.z, v2.z);
                CHECK_CLOSE(q.length(), 1.0f);
            }
        }
        TEST_CASE("Quaternion::fromEuler")
        {
            SUBCASE("Individual Axis Rotations")
            {
                Quaternion qX = Quaternion::fromEuler(90.0f, 0.0f, 0.0f);
                CHECK_CLOSE_QUATERNION(qX, 0.707106f, 0.0f, 0.0f, 0.707106f);

                Quaternion qY = Quaternion::fromEuler(0.0f, 90.0f, 0.0f);
                CHECK_CLOSE_QUATERNION(qY, 0.0f, 0.707106f, 0.0f, 0.707106f);

                Quaternion qZ = Quaternion::fromEuler(0.0f, 0.0f, 90.0f);
                CHECK_CLOSE_QUATERNION(qZ, 0.0f, 0.0f, 0.707106f, 0.707106f);
            }
            SUBCASE("Zero Rotation")
            {
                Quaternion q = Quaternion::fromEuler(0.0f, 0.0f, 0.0f);
                CHECK_QUATERNIONS(q, Quaternion::identity());
            }
            SUBCASE("Combined Rotations")
            {
                Quaternion q = Quaternion::fromEuler(30.0f, 45.0f, 60.0f);
                CHECK_CLOSE_QUATERNION(q, 0.022260f, 0.4396797f, 0.3604234f, 0.8223632f);
            }
            SUBCASE("Vector3 Overload")
            {
                Vector3 angles(45.0f, 0.0f, 0.0f);
                Quaternion q1 = Quaternion::fromEuler(angles);
                Quaternion q2 = Quaternion::fromEuler(45.0f, 0.0f, 0.0f);
                CHECK_QUATERNIONS(q1, q2);
            }

            SUBCASE("Gimbal Lock (90 deg Pitch)")
            {
                Quaternion q = Quaternion::fromEuler(0.0f, 90.0f, 0.0f);
                Vector3 forward = q * Vector3(0, 0, 1);
                CHECK_CLOSE(forward.x, 1.0f);
                CHECK_CLOSE(forward.y, 0.0f);
                CHECK_CLOSE(forward.z, 0.0f);
            }

            SUBCASE("Negative Angles")
            {
                Quaternion qPos = Quaternion::fromEuler(0.0f, 45.0f, 0.0f);
                Quaternion qNeg = Quaternion::fromEuler(0.0f, -45.0f, 0.0f);
                Quaternion conj = qPos.conjugate();
                CHECK_CLOSE_QUATERNIONS(qNeg, conj);
            }
        }
        TEST_CASE("Quaternion::fromEulerEx")
        {
            float x = 30.0f, y = 45.0f, z = 60.0f;

            SUBCASE("XYZ Order") 
            {
                Quaternion q = Quaternion::fromEulerEx(x, y, z, EulerOrder::XYZ);
                CHECK_CLOSE_QUATERNION(q, 0.3919038f, 0.2005621f, 0.5319757f, 0.7233174f);
            }

            SUBCASE("XZY Order") 
            {
                Quaternion q = Quaternion::fromEulerEx(x, y, z, EulerOrder::XZY);
                CHECK_CLOSE_QUATERNION(q, 0.02226f, 0.2005621f, 0.5319757f, 0.8223632f);
            }

            SUBCASE("YXZ Order") 
            {
                Quaternion q = Quaternion::fromEulerEx(x, y, z, EulerOrder::YXZ);
                CHECK_CLOSE_QUATERNION(q, 0.3919038f, 0.2005621f, 0.3604234f, 0.8223632f);
            }

            SUBCASE("YZX Order") 
            {
                Quaternion q = Quaternion::fromEulerEx(x, y, z, EulerOrder::YZX);
                CHECK_CLOSE_QUATERNION(q, 0.3919038f, 0.4396797f, 0.3604234f, 0.7233174f);
            }

            SUBCASE("ZXY Order") 
            {
                Quaternion q = Quaternion::fromEulerEx(x, y, z, EulerOrder::ZXY);
                CHECK_CLOSE_QUATERNION(q, 0.02226f, 0.4396797f, 0.5319757f, 0.7233174f);
            }
            SUBCASE("ZYX Order") 
            {
                Quaternion q1 = Quaternion::fromEulerEx(x, y, z, EulerOrder::ZYX);
                Quaternion q2 = Quaternion::fromEuler(x, y, z);
                CHECK_QUATERNIONS(q1, q2);
                CHECK_CLOSE_QUATERNION(q1, 0.02226f, 0.4396797f, 0.3604234f, 0.8223632f);
            }
        }
        TEST_CASE("Quaternion::toEulerEx")
        {
            SUBCASE("Round-Trip")
            {
                struct OrderMapping
                {
                    EulerOrder order;
                    const char* name;
                };

                OrderMapping mappings[] =
                {
                     { EulerOrder::XYZ, "XYZ" },
                     { EulerOrder::XZY, "XZY" },
                     { EulerOrder::YXZ, "YXZ" },
                     { EulerOrder::YZX, "YZX" },
                     { EulerOrder::ZXY, "ZXY" },
                     { EulerOrder::ZYX, "ZYX" }
                };

                Vector3 testAngles(20.0f, 35.0f, 50.0f);

                for (const auto& item : mappings)
                {
                    std::string subcaseName = std::string("Physical Orientation Integrity (") + item.name + ")";

                    Quaternion q = Quaternion::fromEulerEx(testAngles.x, testAngles.y, testAngles.z, item.order);

                    Vector3 resultAngles = q.toEulerEx(item.order);

                    Quaternion qVerify = Quaternion::fromEulerEx(resultAngles.x, resultAngles.y, resultAngles.z, item.order);

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
                    EulerOrder order;
                    const char* name;
                };

                OrderMapping mappings[] = {
                    { EulerOrder::XYZ, "XYZ" },
                    { EulerOrder::XZY, "XZY" },
                    { EulerOrder::YXZ, "YXZ" },
                    { EulerOrder::YZX, "YZX" },
                    { EulerOrder::ZXY, "ZXY" },
                    { EulerOrder::ZYX, "ZYX" }
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

                            Vector3 inputAngles;
                            if (item.order == EulerOrder::XYZ || item.order == EulerOrder::ZYX)
                            {
                                inputAngles = Vector3(15.0f, lockAngle, 30.0f);
                            }
                            else if (item.order == EulerOrder::XZY || item.order == EulerOrder::YZX)
                            {
                                inputAngles = Vector3(15.0f, 30.0f, lockAngle);
                            }
                            else
                            {
                                inputAngles = Vector3(lockAngle, 15.0f, 30.0f);
                            }

                            Quaternion q = Quaternion::fromEulerEx(inputAngles.x, inputAngles.y, inputAngles.z, item.order);
                            Vector3 extracted = q.toEulerEx(item.order);
                            Quaternion qVerify = Quaternion::fromEulerEx(extracted.x, extracted.y, extracted.z, item.order);
                            CHECK_CLOSE_QUATERNIONS(q, qVerify);
                        }
                    }
                }
            }
            SUBCASE("Cross Order Consistency")
            {
                struct OrderMapping {
                    EulerOrder order;
                    const char* name;
                };

                OrderMapping mappings[] = {
                    { EulerOrder::XYZ, "XYZ" },
                    { EulerOrder::XZY, "XZY" },
                    { EulerOrder::YXZ, "YXZ" },
                    { EulerOrder::YZX, "YZX" },
                    { EulerOrder::ZXY, "ZXY" },
                    { EulerOrder::ZYX, "ZYX" }
                };

                Vector3 initialAngles(25.0f, -40.0f, 15.0f);

                for (const auto& source : mappings)
                {
                    Quaternion qSource = Quaternion::fromEulerEx(initialAngles.x, initialAngles.y, initialAngles.z, source.order);

                    for (const auto& target : mappings)
                    {
                        std::string subcaseName = std::string("Convert ") + source.name + " -> " + target.name;

                        SUBCASE(subcaseName.c_str())
                        {
                            Vector3 extracted = qSource.toEulerEx(target.order);
                            Quaternion qVerify = Quaternion::fromEulerEx(extracted.x, extracted.y, extracted.z, target.order);
                            CAPTURE(source.name);
                            CAPTURE(target.name);
                            CHECK_CLOSE_QUATERNIONS(qSource, qVerify);
                        }
                    }
                }
            }
        }
        TEST_CASE("Quaternion::toEuler")
        {
            SUBCASE("Same As Quaternion::toEulerEx")
            {
                Quaternion q = Quaternion::fromEuler(20, 30, -45);
                Vector3 ref = q.toEulerEx(EulerOrder::ZXY);
                Vector3 res = q.toEuler();

                CHECK_CLOSE(ref.x, res.x);
                CHECK_CLOSE(ref.y, res.y);
                CHECK_CLOSE(ref.z, res.z);
            }
        }
        TEST_CASE("Quaternion::lookAt")
        {
            Vector3 worldUp(0, 1, 0);

            SUBCASE("Looking Forward")
            {
                Vector3 target(0, 0, 1);
                Quaternion q = Quaternion::lookAt(target, worldUp);
                CHECK_CLOSE(q.length(), 1.0f);

                Vector3 result = q * Vector3(0, 0, 1);
                CHECK_CLOSE(result.x, 0.0f);
                CHECK_CLOSE(result.y, 0.0f);
                CHECK_CLOSE(result.z, 1.0f);
            }
            SUBCASE("Looking Right")
            {
                Vector3 target(1, 0, 0);
                Quaternion q = Quaternion::lookAt(target, worldUp);
                CHECK_CLOSE(q.length(), 1.0f);

                Vector3 result = q * Vector3(0, 0, 1);
                CHECK_CLOSE(result.x, 1.0f);
                CHECK_CLOSE(result.y, 0.0f);
                CHECK_CLOSE(result.z, 0.0f);
            }
            SUBCASE("Looking Backwards")
            {
                Vector3 target(0, 0, -1);
                Quaternion q = Quaternion::lookAt(target, worldUp);
                CHECK_CLOSE(q.length(), 1.0f);

                Vector3 result = q * Vector3(0, 0, 1);
                CHECK_CLOSE(result.x, 0.0f);
                CHECK_CLOSE(result.y, 0.0f);
                CHECK_CLOSE(result.z, -1.0f);
            }
            SUBCASE("Looking Slightly Off Vertical")
            {
                Vector3 target(0.001f, 1.0f, 0.0f);
                Quaternion q = Quaternion::lookAt(target, worldUp);
                CHECK_CLOSE(q.length(), 1.0f);

                Vector3 result = q * Vector3(0, 0, 1);
                CHECK_CLOSE(result.x, 0.001f);
                CHECK_CLOSE(result.y, 1.0f);
                CHECK_CLOSE(result.z, 0.0f);
            }
            SUBCASE("Zero Direction")
            {
                Quaternion q = Quaternion::lookAt(Vector3(0, 0, 0), worldUp);
                CHECK_CLOSE(q.length(), 1.0f);

                CHECK_CLOSE(q.x, 0.0f);
                CHECK_CLOSE(q.y, 0.0f);
                CHECK_CLOSE(q.z, 0.0f);
                CHECK_CLOSE(q.w, 1.0f);
            }
            SUBCASE("Looking Straight Up")
            {
                Vector3 worldUp(0, 1, 0);
                Vector3 target(0, 1, 0);

                Quaternion q = Quaternion::lookAt(target, worldUp);
                CHECK_CLOSE(q.length(), 1.0f);

                Vector3 result = q * Vector3(0, 0, 1);
                CHECK_CLOSE(result.x, 0.0f);
                CHECK_CLOSE(result.y, 1.0f);
                CHECK_CLOSE(result.z, 0.0f);

                CHECK(!std::isnan(q.x));
                CHECK(!std::isnan(q.y));
                CHECK(!std::isnan(q.z));
                CHECK(!std::isnan(q.w));
            }
            SUBCASE("Looking Straight Down")
            {
                Vector3 target(0, -1, 0);
                Quaternion q = Quaternion::lookAt(target, worldUp);
                CHECK_CLOSE(q.length(), 1.0f);

                Vector3 result = q * Vector3(0, 0, 1);
                CHECK_CLOSE(result.x, 0.0f);
                CHECK_CLOSE(result.y, -1.0f);
                CHECK_CLOSE(result.z, 0.0f);

                CHECK(!std::isnan(q.x));
                CHECK(!std::isnan(q.y));
                CHECK(!std::isnan(q.z));
                CHECK(!std::isnan(q.w));;
            }
            SUBCASE("Tilted up Vector")
            {
                Vector3 tiltedUp = Vector3(1, 1, 0).normalized();
                Vector3 target(0, 0, 1);

                Quaternion q = Quaternion::lookAt(target, tiltedUp);
                CHECK_CLOSE(q.length(), 1.0f);

                Vector3 forwardResult = q * Vector3(0, 0, 1);
                Vector3 upResult = q * Vector3(0, 1, 0);

                CHECK_CLOSE(forwardResult.z, 1.0f);
                CHECK_CLOSE(Vector3::dot(upResult, forwardResult), 0.0f);
            }
            SUBCASE("Non Unit Inputs")
            {
                Vector3 target(10.0f, 10.0f, 10.0f);
                Vector3 up(0.0f, 50.0f, 0.0f);

                Quaternion q = Quaternion::lookAt(target, up);
                CHECK_CLOSE(q.length(), 1.0f);

                Vector3 result = q * Vector3(0, 0, 1);
                Vector3 expected = target.normalized();
                CHECK_CLOSE(result.x, expected.x);
                CHECK_CLOSE(result.y, expected.y);
                CHECK_CLOSE(result.z, expected.z);
            }
        }
        TEST_CASE("Quaternion Normalization")
        {
            SUBCASE("Length and LengthSquared")
            {
                Quaternion q(1.0f, 1.0f, 1.0f, 1.0f);
                CHECK_CLOSE(q.lengthSquared(), 4.0f);
                CHECK_CLOSE(q.length(), 2.0f);
            }
            SUBCASE("Normalize A Quaternion")
            {
                Quaternion q(1.0f, 1.0f, 1.0f, 1.0f);
                q.normalize();

                CHECK(q.length() == doctest::Approx(1.0f));

                CHECK_CLOSE(q.x, 0.5f);
                CHECK_CLOSE(q.y, 0.5f);
                CHECK_CLOSE(q.z, 0.5f);
                CHECK_CLOSE(q.w, 0.5f);
            }
            SUBCASE("Normalized (Const version)")
            {
                const Quaternion q(0.0f, 10.0f, 0.0f, 0.0f);
                Quaternion unit = q.normalized();

                CHECK_CLOSE(q.length(), 10.0f);
                CHECK_CLOSE(unit.length(), 1.0f);
                CHECK_CLOSE(unit.y, 1.0f);
            }
            SUBCASE("Zero Magnitude Safety")
            {
                Quaternion q(0, 0, 0, 0);
                q.normalize();

                CHECK_QUATERNIONS(q, Quaternion::identity());
            }
            SUBCASE("Already Normalized")
            {
                Quaternion q = Quaternion::identity();
                q.normalize();

                CHECK_CLOSE(q.w , 1.0f);
                CHECK_CLOSE(q.length(), 1.0f);
            }
        }
        TEST_CASE("Quaternion::conjugate")
        {
            SUBCASE("Conjugate")
            {
                Quaternion q(1.0f, 2.0f, 3.0f, 4.0f);
                Quaternion c = q.conjugate();

                CHECK(c.x == -1.0f);
                CHECK(c.y == -2.0f);
                CHECK(c.z == -3.0f);
                CHECK(c.w == 4.0f);
            }
        }
        TEST_CASE("Quaternion::inverse")
        {
            SUBCASE("Inverse Unit Quaternion")
            {
                Quaternion q = Quaternion::fromAxisAngle(Vector3(0, 1, 0), 90.0f);
                Quaternion inv = q.inverse();

                Vector3 v(1, 0, 0);
                Vector3 result = (q * v);
                result = (inv * result);

                CHECK_CLOSE(result.x, 1.0f);
                CHECK_CLOSE(result.y, 0.0f);
                CHECK_CLOSE(result.z, 0.0f);
            }

            SUBCASE("Inverse Non Unit Quaternion")
            {
                Quaternion q(1.0f, 1.0f, 1.0f, 1.0f);
                Quaternion inv = q.inverse();

                CHECK_CLOSE(inv.x, -0.25f);
                CHECK_CLOSE(inv.y, -0.25f);
                CHECK_CLOSE(inv.z, -0.25f);
                CHECK_CLOSE(inv.w, 0.25f);
            }

            SUBCASE("Inverse Zero Quaternion")
            {
                Quaternion q(0, 0, 0, 0);
                Quaternion inv = q.inverse();

                CHECK_QUATERNIONS(inv, Quaternion::identity());
            }

            SUBCASE("Inverse of Inverse")
            {
                Quaternion q = Quaternion::fromAxisAngle(Vector3(1, 2, 3).normalized(), 45.0f);
                Quaternion inv = q.inverse();

                Quaternion finalQ = inv.inverse();

                CHECK_CLOSE_QUATERNIONS(finalQ, q);
            }
            SUBCASE("Identity Property Unit Quaternion")
            {
                Quaternion q1 = Quaternion::fromAxisAngle(Vector3(1, 0.5f, 0.2f).normalized(), 42.0f);
                Quaternion inv1 = q1.inverse();

                Quaternion result1 = q1 * inv1;

                SUBCASE("Unit Quaternion is Identity")
                {
                    CHECK_CLOSE(result1.x, 0.0f);
                    CHECK_CLOSE(result1.y, 0.0f);
                    CHECK_CLOSE(result1.z, 0.0f);
                    CHECK_CLOSE(result1.w, 1.0f);
                }
            }
            SUBCASE("Identity Property Non Unit Quaternion")
            {
                Quaternion q2(2.0f, 2.0f, 2.0f, 2.0f);
                Quaternion inv2 = q2.inverse();

                Quaternion result2 = q2 * inv2;

                SUBCASE("Non Unit Quaternion is Identity")
                {
                    CHECK_CLOSE(result2.x, 0.0f);
                    CHECK_CLOSE(result2.y, 0.0f);
                    CHECK_CLOSE(result2.z, 0.0f);
                    CHECK_CLOSE(result2.w, 1.0f);
                }
            }
            SUBCASE("Relative Rotation")
            {
                Quaternion start = Quaternion::fromAxisAngle(Vector3(0, 1, 0), 10.0f);
                Quaternion end = Quaternion::fromAxisAngle(Vector3(0, 1, 0), 30.0f);

                Quaternion delta = end * start.inverse();

                float angle;
                Vector3 axis;
                delta.toAxisAngle(axis, angle);

                CHECK_CLOSE(angle, 20.0f);
            }
        }
        TEST_CASE("Quaternion::VectorTransform")
        {
            SUBCASE("Rotate Identity")
            {
                Quaternion q = Quaternion::identity();
                Vector3 v(1, 2, 3);
                Vector3 result = q.rotate(v);

                CHECK_CLOSE(result.x, v.x);
                CHECK_CLOSE(result.y, v.y);
                CHECK_CLOSE(result.z, v.z);
            }
            SUBCASE("Rotate 90 Degrees Around Y")
            {
                Quaternion q = Quaternion::fromAxisAngle(Vector3(0, 1, 0), 90.0f);

                Vector3 f = q.forward();
                Vector3 u = q.up();
                Vector3 r = q.right();

                CHECK_CLOSE(f.x, 1.0f);
                CHECK_CLOSE(f.y, 0.0f);
                CHECK_CLOSE(f.z, 0.0f);

                CHECK_CLOSE(u.x, 0.0f);
                CHECK_CLOSE(u.y, 1.0f);
                CHECK_CLOSE(u.z, 0.0f);

                CHECK_CLOSE(r.x, 0.0f);
                CHECK_CLOSE(r.y, 0.0f);
                CHECK_CLOSE(r.z, -1.0f);
            }
            SUBCASE("Rotate 180 Degrees Around X")
            {
                Quaternion q = Quaternion::fromAxisAngle(Vector3(1, 0, 0), 180.0f);

                Vector3 f = q.forward();
                Vector3 u = q.up();
                Vector3 r = q.right();

                CHECK_CLOSE(f.x, 0.0f);
                CHECK_CLOSE(f.y, 0.0f);
                CHECK_CLOSE(f.z, -1.0f);

                CHECK_CLOSE(u.x, 0.0f);
                CHECK_CLOSE(u.y, -1.0f);
                CHECK_CLOSE(u.z, 0.0f);

                CHECK_CLOSE(r.x, 1.0f);
                CHECK_CLOSE(r.y, 0.0f);
                CHECK_CLOSE(r.z, 0.0f);
            }
            SUBCASE("Rotate Non Unity Vector")
            {
                Quaternion q = Quaternion::fromAxisAngle(Vector3(0, 0, 1), 90.0f);
                Vector3 v(0, 10, 0);
                Vector3 result = q.rotate(v);

                CHECK_CLOSE(result.x, -10.0f);
                CHECK_CLOSE(result.y, 0.0f);
                CHECK_CLOSE(result.z, 0.0f);
            }
        }
        TEST_CASE("Quaternion::Dot")
        {
            SUBCASE("Dot with Identity")
            {
                Quaternion q1 = Quaternion::identity();
                Quaternion q2 = Quaternion::identity();

                CHECK_CLOSE(Quaternion::dot(q1, q2), 1.0f);
                CHECK_CLOSE(q1.dot(q2), 1.0f);
            }
            SUBCASE("Orthogonal Quaternions")
            {
                Quaternion q1(1, 0, 0, 0);
                Quaternion q2(0, 1, 0, 0);

                CHECK_CLOSE(Quaternion::dot(q1, q2), 0.0f);
                CHECK_CLOSE(q1.dot(q2), 0.0f);
            }
            SUBCASE("Opposite Orientations (Double Cover)")
            {
                Quaternion q1(0, 0, 0, 1);
                Quaternion q2(0, 0, 0, -1);

                CHECK_CLOSE(Quaternion::dot(q1, q2), -1.0f);
                CHECK_CLOSE(q1.dot(q2), -1.0f);
            }
            SUBCASE("General Values")
            {
                Quaternion q1(1, 2, 3, 4);
                Quaternion q2(5, 6, 7, 8);

                CHECK_CLOSE(Quaternion::dot(q1, q2), 70.0f);
                CHECK_CLOSE(q1.dot(q2), 70.0f);
            }
        }
        TEST_CASE("Quaternion::NLerp")
        {
            Quaternion q1 = Quaternion::identity();
            Quaternion q2 = Quaternion::fromAxisAngle(Vector3(0, 1, 0), 90.0f);

            SUBCASE("Midpoint")
            {
                Quaternion result = Quaternion::nlerp(q1, q2, 0.5f);
                CHECK_CLOSE(result.lengthSquared(), 1.0f);

                float angle;
                Vector3 axis;
                result.toAxisAngle(axis, angle);

                CHECK_CLOSE(angle, 45.0f);
                CHECK_CLOSE(axis.x, 0.0f);
                CHECK_CLOSE(axis.y, 1.0f);
                CHECK_CLOSE(axis.z, 0.0f);
            }

            SUBCASE("Short Path Logic")
            {
                Quaternion q2Neg(-q2.x, -q2.y, -q2.z, -q2.w);
                Quaternion result = Quaternion::nlerp(q1, q2Neg, 0.5f);

                float angle;
                Vector3 axis;
                result.toAxisAngle(axis, angle);

                CHECK_CLOSE(angle, 45.0f);
            }

            SUBCASE("Clamping")
            {
                Quaternion result = Quaternion::nlerp(q1, q2, 1.5f);
                CHECK_CLOSE_QUATERNIONS(result, q2);

                Quaternion resultUnclamped = Quaternion::nlerpUnclamped(q1, q2, 2.0f);
                CHECK(resultUnclamped.w != doctest::Approx(q2.w));
            }
        }
        TEST_CASE("Quaternion::slerp")
        {
            Quaternion identity = Quaternion::identity();
            Quaternion rotY90 = Quaternion::fromAxisAngle(Vector3(0, 1, 0), 90.0f);

            SUBCASE("Endpoints")
            {
                Quaternion identity = Quaternion::identity();
                Quaternion rotY90 = Quaternion::fromAxisAngle(Vector3(0, 1, 0), 90.0f);

                Quaternion start = Quaternion::slerp(identity, rotY90, 0.0f);
                CHECK_CLOSE(start.x, 0.0f);
                CHECK_CLOSE(start.y, 0.0f);
                CHECK_CLOSE(start.z, 0.0f);
                CHECK_CLOSE(start.w, 1.0f);

                Quaternion end = Quaternion::slerp(identity, rotY90, 1.0f);
                CHECK_CLOSE(end.x, rotY90.x);
                CHECK_CLOSE(end.y, rotY90.y);
                CHECK_CLOSE(end.z, rotY90.z);
                CHECK_CLOSE(end.w, rotY90.w);
            }
            SUBCASE("Midpoint")
            {
                Quaternion identity = Quaternion::identity();
                Quaternion rotY90 = Quaternion::fromAxisAngle(Vector3(0, 1, 0), 90.0f);

                Quaternion mid = Quaternion::slerp(identity, rotY90, 0.5f);

                float rad225 = 22.5f * (3.1415926535f / 180.0f);
                float expectedY = std::sin(rad225);
                float expectedW = std::cos(rad225);

                CHECK_CLOSE(mid.x, 0.0f);
                CHECK_CLOSE(mid.y, expectedY);
                CHECK_CLOSE(mid.z, 0.0f);
                CHECK_CLOSE(mid.w, expectedW);
            }
            SUBCASE("Shortest Path")
            {
                Quaternion q1 = Quaternion::identity();
                Quaternion q2 = -rotY90;

                Quaternion mid = Quaternion::slerp(q1, q2, 0.5f);

                float expectedW = std::cos(22.5f * (3.14159f / 180.0f));
                CHECK_CLOSE(std::abs(mid.w), expectedW);
            }
            SUBCASE("Extremely Close Rotations")
            {
                Quaternion q1 = Quaternion::identity();
                Quaternion q2 = Quaternion(0.00001f, 0, 0, 1.0f).normalized();

                Quaternion result = Quaternion::slerp(q1, q2, 0.5f);

                CHECK_FALSE(std::isnan(result.w));
                CHECK_CLOSE(result.w, 1.0f);
            }
            SUBCASE("Slerp Clamping and Extrapolation")
            {
                Quaternion q1 = Quaternion::identity();
                Quaternion q2 = Quaternion::fromAxisAngle(Vector3(0, 1, 0), 90.0f);

                Quaternion resultClamped = Quaternion::slerp(q1, q2, 1.5f);

                CHECK_CLOSE(resultClamped.x, q2.x);
                CHECK_CLOSE(resultClamped.y, q2.y);
                CHECK_CLOSE(resultClamped.z, q2.z);
                CHECK_CLOSE(resultClamped.w, q2.w);

                Quaternion resultUnclamped = Quaternion::slerpUnclamped(q1, q2, 2.0f);

                CHECK_CLOSE(resultUnclamped.x, 0.0f);
                CHECK_CLOSE(resultUnclamped.y, 1.0f);
                CHECK_CLOSE(resultUnclamped.z, 0.0f);
                CHECK_CLOSE(resultUnclamped.w, 0.0f);

                CHECK(resultUnclamped.w != doctest::Approx(q2.w));
                CHECK(resultUnclamped.y != doctest::Approx(q2.y));
            }
        }
        TEST_CASE("Quaternion::toMatrix")
        {
            SUBCASE("Identity Mapping")
            {
                Quaternion q = Quaternion::identity();
                Matrix4x4 m = q.toMatrix();
                CHECK_CLOSE_MATRIX(m, Matrix4x4::identity());
            }
            SUBCASE("90 Degree Rotation Along X-Axis")
            {
                Quaternion q = Quaternion::fromEuler(90.0f, 0.0f, 0.0f);
                Matrix4x4 m = q.toMatrix();

                Matrix4x4 expected = Matrix4x4::identity();
                expected(1, 1) = 0.0f; 
                expected(1, 2) = -1.0f;
                expected(2, 1) = 1.0f;  
                expected(2, 2) = 0.0f;

                CHECK_CLOSE_MATRIX(m, expected);
            }
            SUBCASE("90 Degree Rotation Along Y-Axis")
            {
                Quaternion q = Quaternion::fromEuler(0.0f, 90.0f, 0.0f);
                Matrix4x4 m = q.toMatrix();

                Matrix4x4 expected = Matrix4x4::identity();
                expected(0, 0) = 0.0f;  
                expected(0, 2) = 1.0f;
                expected(2, 0) = -1.0f; 
                expected(2, 2) = 0.0f;

                CHECK_CLOSE_MATRIX(m, expected);
            }
            SUBCASE("90 Degree Rotation Along Z-Axis")
            {
                Quaternion q = Quaternion::fromEuler(0.0f, 0.0f, 90.0f);
                Matrix4x4 m = q.toMatrix();

                Matrix4x4 expected = Matrix4x4::identity();
                expected(0, 0) = 0.0f; 
                expected(0, 1) = -1.0f;
                expected(1, 0) = 1.0f;  
                expected(1, 1) = 0.0f;
                expected(2, 2) = 1.0f;

                CHECK_CLOSE_MATRIX(m, expected);
            }
        }
        TEST_CASE("Quaternion::fromMatrix")
        {
            SUBCASE("Branch 1: Positive Trace (qw is largest)")
            {
                Matrix4x4 m = Matrix4x4::rotateX(0.1f);
                Quaternion q = Quaternion::fromMatrix(m);

                CHECK_CLOSE_MATRIX(q.toMatrix(), m);
            }

            SUBCASE("Branch 2: Row 0 is largest")
            {
                Matrix4x4 m = Matrix4x4::rotateX(Mathf::Pi);
                Quaternion q = Quaternion::fromMatrix(m);

                CHECK_CLOSE_MATRIX(q.toMatrix(), m);
            }

            SUBCASE("Branch 3: Row 1 is largest")
            {
                Matrix4x4 m = Matrix4x4::rotateY(Mathf::Pi);
                Quaternion q = Quaternion::fromMatrix(m);

                CHECK_CLOSE_MATRIX(q.toMatrix(), m);
            }
            SUBCASE("Branch 4: Row 2 is largest")
            {
                Matrix4x4 m = Matrix4x4::rotateZ(Mathf::Pi);
                Quaternion q = Quaternion::fromMatrix(m);

                CHECK_CLOSE_MATRIX(q.toMatrix(), m);
            }
            SUBCASE("Ignoring Translation")
            {
                Matrix4x4 m = Matrix4x4::translate({ 10.0f, -5.0f, 20.0f }) * Matrix4x4::rotateX(0.5f);
                Quaternion q = Quaternion::fromMatrix(m);
                Quaternion expected = Quaternion::fromEulerEx(0.5f * Mathf::Rad2Deg, 0.0f, 0.0f, EulerOrder::XYZ);

                float dot = q.x * expected.x + q.y * expected.y + q.z * expected.z + q.w * expected.w;
                CHECK(std::abs(dot) > 0.999f);
            }
            SUBCASE("Random Rotation Integrity")
            {
                Vector3 testOrientations[] = {
                    { 10.0f, 20.0f, 30.0f },
                    { 90.0f, 45.0f, 0.0f },
                    { 180.0f, 0.0f, 90.0f },
                    { -45.0f, 120.0f, -30.0f }
                };

                for (const auto& angles : testOrientations)
                {
                    Quaternion qOriginal = Quaternion::fromEuler(angles.x, angles.y, angles.z);
                    Matrix4x4 m = qOriginal.toMatrix();
                    Quaternion qReconstructed = Quaternion::fromMatrix(m);
                    float dot = qOriginal.x * qReconstructed.x + qOriginal.y * qReconstructed.y + qOriginal.z * qReconstructed.z + qOriginal.w * qReconstructed.w;

                    CAPTURE(angles);
                    CAPTURE(qOriginal);
                    CAPTURE(qReconstructed);

                    CHECK_CLOSE(std::abs(dot), 0.9999f);
                }
            }
            SUBCASE("Numerical Stability near 180 degrees")
            {
                Matrix4x4 m = Matrix4x4::rotateY(Mathf::Pi -0.001f);

                Quaternion q = Quaternion::fromMatrix(m);
                Matrix4x4 mBack = q.toMatrix();

                CHECK_CLOSE_MATRIX(m, mBack, 0.001f);
            }
        }
        TEST_CASE("Quaternion Arithmetic")
        {
            Quaternion q1(1, 2, 3, 4);
            Quaternion q2(5, 6, 7, 8);

            SUBCASE("Binary Addition and Subtraction")
            {
                Quaternion resAdd = q1 + q2;
                CHECK_CLOSE(resAdd.x, 6); 
                CHECK_CLOSE(resAdd.y, 8);
                CHECK_CLOSE(resAdd.z, 10); 
                CHECK_CLOSE(resAdd.w, 12);

                Quaternion resSub = q2 - q1;
                CHECK_CLOSE(resSub.x, 4); 
                CHECK_CLOSE(resSub.y, 4);
                CHECK_CLOSE(resSub.z, 4); 
                CHECK_CLOSE(resSub.w, 4);
            }

            SUBCASE("Assignment Addition and Subtraction")
            {
                Quaternion q = q1;
                q += q2;
                CHECK_CLOSE_QUATERNIONS(q, (q1 + q2));

                q -= q2;
                CHECK_CLOSE_QUATERNIONS(q, q1);
            }

            SUBCASE("Negation")
            {
                Quaternion neg = -q1;
                CHECK_CLOSE(neg.x, -1); 
                CHECK_CLOSE(neg.y, -2);
                CHECK_CLOSE(neg.z, -3); 
                CHECK_CLOSE(neg.w, -4);
            }
        }
        TEST_CASE("Quaternion Multiplication")
        {
            Quaternion qX = Quaternion::fromAxisAngle(Vector3(1, 0, 0), 90.0f);
            Quaternion qY = Quaternion::fromAxisAngle(Vector3(0, 1, 0), 90.0f);

            SUBCASE("Quaternion * Quaternion")
            {
                Quaternion combined = qX * qY;
                CHECK_CLOSE(combined.x, 0.5f);
                CHECK_CLOSE(combined.y, 0.5f);
                CHECK_CLOSE(combined.z, 0.5f);
                CHECK_CLOSE(combined.w, 0.5f);
            }

            SUBCASE("Scalar Multiplication")
            {
                Quaternion q(1, 1, 1, 1);
                Quaternion res = q * 2.0f;
                CHECK_CLOSE(res.x, 2.0f);

                Quaternion resLeft = 0.5f * res;
                CHECK_CLOSE(resLeft.x, 1.0f);

                q *= 3.0f;
                CHECK_CLOSE(q.x, 3.0f);
            }
            SUBCASE("Quaternion * Vector3 (Rotate)")
            {
                Quaternion q = Quaternion::fromAxisAngle(Vector3(0, 1, 0), 90.0f);
                Vector3 v(0, 0, 1);
                Vector3 rotated = q * v;

                CHECK_CLOSE(rotated.x, 1.0f);
                CHECK_CLOSE(rotated.y, 0.0f);
                CHECK_CLOSE(rotated.z, 0.0f);
            }
            SUBCASE("Composition Validation")
            {
                Quaternion qX = Quaternion::fromAxisAngle(Vector3(1, 0, 0), 90.0f);
                Quaternion qY = Quaternion::fromAxisAngle(Vector3(0, 1, 0), 90.0f);
                Vector3 v(0, 0, 1); 

                Vector3 step1 = qX * v;
                Vector3 finalA = qY * step1;

                Quaternion combined = qY * qX;
                Vector3 finalB = combined * v;

                CHECK_CLOSE(finalB.x, finalA.x);
                CHECK_CLOSE(finalB.y, finalA.y);
                CHECK_CLOSE(finalB.z, finalA.z);
            }
        }
        TEST_CASE("Quaternion Utility")
        {
            Quaternion q(1, 2, 3, 4);

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
                Quaternion same(1, 2, 3, 4);
                Quaternion different(0, 2, 3, 4);

                CHECK_CLOSE(q == same);
                CHECK_CLOSE(q != different);
                CHECK_FALSE(q == different);
            }
        }
    }
}