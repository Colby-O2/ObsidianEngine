#ifndef QUATERNION_H_
#define QUATERNION_H_

#include "Vector.h"
#include "Matrix.h"

#include <iostream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <numbers>

namespace ObsidianEngine 
{
	namespace detail
	{
		template<typename T>
		struct Quaternion;
	}

	using Quaternion = detail::Quaternion<float>;

	namespace detail
	{
		template<typename T>
		struct Quaternion
		{
			union
			{
				struct
				{
					T x, y, z, w;
				};

				T data[4];
			};

			constexpr Quaternion() : x(0), y(0), z(0), w(static_cast<T>(1)) {}
			constexpr Quaternion(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}
			constexpr Quaternion(const Vector<T, 4>& v) : x(v.x), y(v.y), z(v.z), w(v.w) {}

			static constexpr Quaternion identity()
			{
				return Quaternion(0, 0, 0, static_cast<T>(1));
			}

			static Quaternion fromAxisAngle(const Vector<T, 3>& axis, T angleDegrees)
			{
				T rad = angleDegrees * (std::numbers::pi_v<T> / static_cast<T>(180));
				T halfAngle = rad * static_cast<T>(0.5);
				T s = std::sin(halfAngle);
				Vector<T, 3> normAxis = axis.normalized();
				return Quaternion(normAxis.x * s, normAxis.y * s, normAxis.z * s, std::cos(halfAngle));
			}

			static T angle(const Quaternion& a, const Quaternion& b)
			{
				T d = dot(a, b);

				d = std::abs(d);

				d = std::clamp(d, static_cast<T>(0), static_cast<T>(1));

				T angleRad = static_cast<T>(2) * std::acos(d);

				return angleRad * (static_cast<T>(180) / std::numbers::pi_v<T>);
			}

			static Quaternion rotateTowards(const Quaternion& from, const Quaternion& to, T maxDegreesDelta)
			{
				T totalAngle = angle(from, to);

				if (totalAngle < static_cast<T>(1e-6))
				{
					return to;
				}

				T t = std::min(static_cast<T>(1), maxDegreesDelta / totalAngle);

				return slerp(from, to, t);
			}

			static Quaternion fromToRotation(const Vector<T, 3>& fromDirection, const Vector<T, 3>& toDirection)
			{
				Vector<T, 3> v0 = fromDirection.normalized();
				Vector<T, 3> v1 = toDirection.normalized();

				T dot = Vector<T, 3>::dot(v0, v1);

				if (dot >= static_cast<T>(0.99999))
				{
					return identity();
				}

				if (dot <= static_cast<T>(-0.99999))
				{
					Vector<T, 3> axis = Vector<T, 3>::cross(Vector<T, 3>(1, 0, 0), v0);
					if (axis.sqrMagnitude() < static_cast<T>(0.001))
					{
						axis = Vector<T, 3>::cross(Vector<T, 3>(0, 1, 0), v0);
					}
					return fromAxisAngle(axis.normalized(), static_cast<T>(180));
				}

				Vector<T, 3> cross = Vector<T, 3>::cross(v0, v1);
				Quaternion q(cross.x, cross.y, cross.z, dot + static_cast<T>(1));

				return q.normalized();
			}

			static Quaternion fromEuler(T x, T y, T z)
			{
				T factor = std::numbers::pi_v<T> / static_cast<T>(360);
				T cx = std::cos(x * factor); T sx = std::sin(x * factor);
				T cy = std::cos(y * factor); T sy = std::sin(y * factor);
				T cz = std::cos(z * factor); T sz = std::sin(z * factor);

				return Quaternion(
					sx * cy * cz - cx * sy * sz,
					cx * sy * cz + sx * cy * sz,
					cx * cy * sz - sx * sy * cz,
					cx * cy * cz + sx * sy * sz
				);
			}

			static Quaternion fromEuler(const Vector<T, 3>& eulerAngles)
			{
				return fromEuler(eulerAngles.x, eulerAngles.y, eulerAngles.z);
			}

			Vector<T, 3> toEuler() const
			{
				Vector<T, 3> euler;

				T sinp = std::clamp(static_cast<T>(2) * (w * y - z * x), static_cast<T>(-1), static_cast<T>(1));

				if (std::abs(sinp) >= static_cast<T>(0.99999))
				{
					euler.y = std::copysign(std::numbers::pi_v<T> / 2, sinp);
					euler.x = static_cast<T>(2) * std::atan2(x, w);
					euler.z = 0;
				}
				else
				{
					euler.y = std::asin(sinp);

					T sinr_cosp = static_cast<T>(2) * (w * x + y * z);
					T cosr_cosp = static_cast<T>(1) - static_cast<T>(2) * (x * x + y * y);
					euler.x = std::atan2(sinr_cosp, cosr_cosp);

					T siny_cosp = static_cast<T>(2) * (w * z + x * y);
					T cosy_cosp = static_cast<T>(1) - static_cast<T>(2) * (y * y + z * z);
					euler.z = std::atan2(siny_cosp, cosy_cosp);
				}

				return euler * (static_cast<T>(180) / std::numbers::pi_v<T>);
			}

			static Quaternion lookAt(const Vector<T, 3>& direction, const Vector<T, 3>& up)
			{
				if (direction.sqrMagnitude() < static_cast<T>(0.0001))
				{
					return identity();
				}

				Vector<T, 3> forward = direction.normalized();
				Vector<T, 3> right = Vector<T, 3>::cross(up, forward).normalized();
				Vector<T, 3> actualUp = Vector<T, 3>::cross(forward, right);

				Matrix4x4<T> rot;

				rot(0, 0) = right.x;    rot(1, 0) = right.y;    rot(2, 0) = right.z;
				rot(0, 1) = actualUp.x; rot(1, 1) = actualUp.y; rot(2, 1) = actualUp.z;
				rot(0, 2) = forward.x;  rot(1, 2) = forward.y;  rot(2, 2) = forward.z;

				return fromMatrix(rot);
			}

			T lengthSquared() const
			{
				return this->dot(*this);
			}

			T length() const
			{
				return std::sqrt(lengthSquared());
			}

			void normalize()
			{
				T len = length();
				if (len > 0) 
				{
					T invLen = static_cast<T>(1) / len;
					x *= invLen; y *= invLen; z *= invLen; w *= invLen;
				}
			}

			Quaternion normalized() const
			{
				Quaternion q = *this;
				q.normalize();
				return q;
			}

			constexpr Quaternion conjugate() const
			{
				return Quaternion(-x, -y, -z, w);
			}

			constexpr Quaternion inverse() const
			{
				T norm = lengthSquared();
				if (norm > 0) 
				{
					T invNorm = static_cast<T>(1) / norm;
					return Quaternion(-x * invNorm, -y * invNorm, -z * invNorm, w * invNorm);
				}
				return identity();
			}

			Vector<T, 3> rotate(const Vector<T, 3>& v) const
			{
				Vector<T, 3> q_vec(x, y, z);
				Vector<T, 3> t = Vector<T, 3>::cross(q_vec, v)* static_cast<T>(2);
				return v + (t * w) + Vector<T, 3>::cross(q_vec, t);
			}

			Vector<T, 3> forward() const { return rotate(Vector<T, 3>(0, 0, 1)); }
			Vector<T, 3> up()      const { return rotate(Vector<T, 3>(0, 1, 0)); }
			Vector<T, 3> right()   const { return rotate(Vector<T, 3>(1, 0, 0)); }

			static T dot(const Quaternion& a, const Quaternion& b)
			{
				return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
			}

			T dot(const Quaternion& other) const
			{
				return x * other.x + y * other.y + z * other.z + w * other.w;
			}

			Matrix4x4<T> toMatrix() const
			{
				T xx = x * x; T yy = y * y; T zz = z * z;
				T xy = x * y; T xz = x * z; T yz = y * z;
				T xw = x * w; T yw = y * w; T zw = z * w;

				Matrix4x4<T> res = Matrix4x4<T>::identity();
				res(0, 0) = static_cast<T>(1) - static_cast<T>(2) * (yy + zz);
				res(1, 0) = static_cast<T>(2) * (xy + zw);
				res(2, 0) = static_cast<T>(2) * (xz - yw);

				res(0, 1) = static_cast<T>(2) * (xy - zw);
				res(1, 1) = static_cast<T>(1) - static_cast<T>(2) * (xx + zz);
				res(2, 1) = static_cast<T>(2) * (yz + xw);

				res(0, 2) = static_cast<T>(2) * (xz + yw);
				res(1, 2) = static_cast<T>(2) * (yz - xw);
				res(2, 2) = static_cast<T>(1) - static_cast<T>(2) * (xx + yy);
				return res;
			}

			static Quaternion fromMatrix(const Matrix4x4<T>& m)
			{
				auto rot = m.template slice<3, 3>(0, 0);

				T tr = rot(0, 0) + rot(1, 1) + rot(2, 2);
				T qx, qy, qz, qw;

				if (tr > 0) {
					T s = std::sqrt(tr + static_cast<T>(1.0)) * static_cast<T>(2.0);
					qw = static_cast<T>(0.25) * s;
					qx = (rot(2, 1) - rot(1, 2)) / s;
					qy = (rot(0, 2) - rot(2, 0)) / s;
					qz = (rot(1, 0) - rot(0, 1)) / s;
				}
				else if ((rot(0, 0) > rot(1, 1)) && (rot(0, 0) > rot(2, 2))) {
					T s = std::sqrt(static_cast<T>(1.0) + rot(0, 0) - rot(1, 1) - rot(2, 2)) * static_cast<T>(2.0);
					qw = (rot(2, 1) - rot(1, 2)) / s;
					qx = static_cast<T>(0.25) * s;
					qy = (rot(0, 1) + rot(1, 0)) / s;
					qz = (rot(0, 2) + rot(2, 0)) / s;
				}
				else if (rot(1, 1) > rot(2, 2)) {
					T s = std::sqrt(static_cast<T>(1.0) + rot(1, 1) - rot(0, 0) - rot(2, 2)) * static_cast<T>(2.0);
					qw = (rot(0, 2) - rot(2, 0)) / s;
					qx = (rot(0, 1) + rot(1, 0)) / s;
					qy = static_cast<T>(0.25) * s;
					qz = (rot(1, 2) + rot(2, 1)) / s;
				}
				else {
					T s = std::sqrt(static_cast<T>(1.0) + rot(2, 2) - rot(0, 0) - rot(1, 1)) * static_cast<T>(2.0);
					qw = (rot(1, 0) - rot(0, 1)) / s;
					qx = (rot(0, 2) + rot(2, 0)) / s;
					qy = (rot(1, 2) + rot(2, 1)) / s;
					qz = static_cast<T>(0.25) * s;
				}

				return Quaternion(qx, qy, qz, qw).normalized();
			}

			static Quaternion nlerpUnclamped(const Quaternion& a, const Quaternion& b, T t) 
			{
				T dot = Quaternion::dot(a, b);
				T t1 = static_cast<T>(1) - t;
				Quaternion res;
				if (dot < 0) res = Quaternion(t1 * a.x - t * b.x, t1 * a.y - t * b.y, t1 * a.z - t * b.z, t1 * a.w - t * b.w);
				else res = Quaternion(t1 * a.x + t * b.x, t1 * a.y + t * b.y, t1 * a.z + t * b.z, t1 * a.w + t * b.w);
				return res.normalized();
			}

			static Quaternion nlerp(const Quaternion& a, const Quaternion& b, T t)
			{
				return nlerpUnclamped(a, b, std::clamp(t, static_cast<T>(0), static_cast<T>(1)));
			}

			static Quaternion slerpUnclamped(const Quaternion& a, const Quaternion& b, T t) 
			{
				T dot = Quaternion::dot(a, b);

				Quaternion end = b;
				if (dot < static_cast<T>(0)) {
					dot = -dot;
					end = Quaternion(-b.x, -b.y, -b.z, -b.w);
				}

				if (dot > static_cast<T>(0.9995)) 
				{
					return nlerpUnclamped(a, end, t);
				}

				T theta_0 = std::acos(dot);
				T theta = theta_0 * t;
				T sin_theta = std::sin(theta);
				T sin_theta_0 = std::sin(theta_0);

				T s0 = std::cos(theta) - dot * sin_theta / sin_theta_0;
				T s1 = sin_theta / sin_theta_0;

				return Quaternion(
					(s0 * a.x) + (s1 * end.x),
					(s0 * a.y) + (s1 * end.y),
					(s0 * a.z) + (s1 * end.z),
					(s0 * a.w) + (s1 * end.w)
				);
			}

			static Quaternion slerp(const Quaternion& a, const Quaternion& b, T t) 
			{
				return slerpUnclamped(a, b, std::clamp(t, static_cast<T>(0), static_cast<T>(1)));
			}

			Quaternion operator*(const Quaternion& other) const 
			{
				return Quaternion(
					w * other.x + x * other.w + y * other.z - z * other.y,
					w * other.y + y * other.w + z * other.x - x * other.z,
					w * other.z + z * other.w + x * other.y - y * other.x,
					w * other.w - x * other.x - y * other.y - z * other.z
				);
			}

			Vector<T, 3> operator*(const Vector<T, 3>& vec) const 
			{ 
				return rotate(vec); 
			}

			T operator[](int i) const 
			{ 
				return data[i]; 
			}

			T& operator[](int i) 
			{ 
				return data[i]; 
			}

			bool operator==(const Quaternion& o) const 
			{ 
				return x == o.x && y == o.y && z == o.z && w == o.w; 
			}

			bool operator!=(const Quaternion& o) const 
			{
				return !(*this == o);
			}
			template<typename T>
			friend std::ostream& operator<<(std::ostream& os, const detail::Quaternion<T>& q) 
			{
				os << "(" << q.x << ", " << q.y << ", " << q.z << ", " << q.w << ")";
				return os;
			}
		};
	}
}

#endif