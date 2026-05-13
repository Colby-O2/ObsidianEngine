#ifndef  __OBSIDIANENGINE_MATH_QUATERNION_H__
#define  __OBSIDIANENGINE_MATH_QUATERNION_H__

#include "ObsidianEngine/Math/MathUtils.h"
#include "ObsidianEngine/Math/Vector/VectorTypes.h"
#include "ObsidianEngine/Math/Matrix/MatrixTypes.h"

#include <iostream>
#include <iomanip>
#include <numbers>
#include <limits>
#include <string>
#include <sstream>

namespace ObsidianEngine
{
	enum class EulerOrder
	{
		XYZ, YXZ, ZXY, ZYX, YZX, XZY
	};
}

namespace ObsidianEngine::detail
{
	template<std::floating_point T>
	struct Matrix4x4;

	template<std::floating_point T>
	struct Quaternion
	{
		using ValueType = T;

		union
		{
			struct
			{
				T x, y, z, w;
			};

			T data[4];
		};

		constexpr Quaternion() noexcept : x(Math<T>::val(Math<T>::val(0))), y(Math<T>::val(0)), z(Math<T>::val(0)), w(Math<T>::val(Math<T>::val(1))) {}
		constexpr Quaternion(T x, T y, T z, T w) noexcept : x(x), y(y), z(z), w(w) {}
		constexpr Quaternion(const Vector<T, 4>& v) noexcept : x(v.x), y(v.y), z(v.z), w(v.w) {}
		template<typename U>
		constexpr explicit Quaternion(const Quaternion<U>& other) noexcept : x(static_cast<T>(other.x)), y(static_cast<T>(other.y)), z(static_cast<T>(other.z)), w(static_cast<T>(other.w)) {}

		template<typename U>
		requires std::is_floating_point_v<U>
		auto cast() const noexcept
		{
			Quaternion<U> result;

			for (size_t i = 0; i < 4; ++i)
			{
				result[i] = static_cast<U>(data[i]);
			}

			return result;
		}

		static constexpr Quaternion identity() noexcept
		{
			return Quaternion(0, 0, 0, 1);
		}

		static Quaternion fromAxisAngle(const Vector<T, 3>& axis, T angleDegrees) noexcept
		{
			T sqrMag = axis.sqrMagnitude();
			if (sqrMag < Math<T>::Epsilon)
			{
				return identity();
			}

			T rad = angleDegrees * Math<T>::Deg2Rad;
			T halfAngle = rad * Math<T>::val(0.5);
			T s = Math<T>::sin(halfAngle);

			Vector<T, 3> normAxis = axis.normalized();
			return Quaternion(normAxis.x * s, normAxis.y * s, normAxis.z * s, Math<T>::cos(halfAngle));
		}

		void toAxisAngle(Vector<T, 3>& outAxis, T& outAngleDegrees) const noexcept
		{
			Quaternion q = *this;

			T lenSq = q.lengthSquared();
			if (Math<T>::abs(lenSq - Math<T>::val(1)) > Math<T>::Epsilon)
			{
				q.normalize();
			}

			T cosHalfAngle = q.w;
			if (cosHalfAngle > Math<T>::val(1)) cosHalfAngle = Math<T>::val(1);
			else if (cosHalfAngle < Math<T>::val(-1)) cosHalfAngle = Math<T>::val(-1);

			T rad = Math<T>::acos(cosHalfAngle) * Math<T>::val(2);
			outAngleDegrees = rad * Math<T>::Rad2Deg;

			T s = Math<T>::sqrt(Math<T>::val(1) - cosHalfAngle * cosHalfAngle);

			if (s < Math<T>::Epsilon)
			{
				outAxis = Vector<T, 3>(1, 0, 0);
			}
			else
			{
				T invS = Math<T>::val(1) / s;
				outAxis.x = x * invS;
				outAxis.y = y * invS;
				outAxis.z = z * invS;
			}
		}

		static T angle(const Quaternion& a, const Quaternion& b) noexcept
		{
			Quaternion q1 = a;
			Quaternion q2 = b;

			T d = Math<T>::abs(dot(q1, q2));

			if (d >= Math<T>::AlmostOne)
			{
				return Math<T>::val(0);
			}

			d = Math<T>::clamp(d, 0, 1);

			return Math<T>::val(2) * Math<T>::acos(d) * Math<T>::Rad2Deg;
		}

		static Quaternion rotateTowards(const Quaternion& from, const Quaternion& to, T maxDegreesDelta) noexcept
		{
			T totalAngle = angle(from, to);

			if (totalAngle < Math<T>::Epsilon || maxDegreesDelta >= totalAngle)
			{
				return to;
			}

			T t = Math<T>::min(1, maxDegreesDelta / totalAngle);

			return slerp(from, to, t);
		}

		Quaternion& setFromToRotation(const Vector<T, 3>& fromDirection, const Vector<T, 3>& toDirection) noexcept
		{
			Vector<T, 3> v0 = fromDirection.normalized();
			Vector<T, 3> v1 = toDirection.normalized();

			T dot = Vector<T, 3>::dot(v0, v1);

			if (dot >= Math<T>::AlmostOne)
			{
				*this = identity();
				return *this;
			}

			if (dot <= Math<T>::AlmostNegativeOne)
			{
				Vector<T, 3> axis = Vector<T, 3>::cross(Vector<T, 3>(1, 0, 0), v0);
				if (axis.sqrMagnitude() < Math<T>::Epsilon)
				{
					axis = Vector<T, 3>::cross(Vector<T, 3>(0, 1, 0), v0);
				}
				*this = fromAxisAngle(axis.normalized(), Math<T>::val(180));
				this->normalize();

				return *this;
			}

			Vector<T, 3> cross = Vector<T, 3>::cross(v0, v1);

			this->x = cross.x;
			this->y = cross.y;
			this->z = cross.z;
			this->w = dot + Math<T>::val(1);
			this->normalize();

			return *this;
		}

		static Quaternion fromToRotation(const Vector<T, 3>& fromDirection, const Vector<T, 3>& toDirection) noexcept
		{
			Quaternion result;
			result.setFromToRotation(fromDirection, toDirection);
			return result;
		}

		static Quaternion fromEulerEx(T x, T y, T z, EulerOrder order) noexcept
		{
			T factor = Math<T>::Deg2Rad * Math<T>::val(0.5);
			T cx = Math<T>::cos(x * factor); T sx = Math<T>::sin(x * factor);
			T cy = Math<T>::cos(y * factor); T sy = Math<T>::sin(y * factor);
			T cz = Math<T>::cos(z * factor); T sz = Math<T>::sin(z * factor);

			switch (order)
			{
			case EulerOrder::XYZ:
				return Quaternion(
					sx * cy * cz + cx * sy * sz,
					cx * sy * cz - sx * cy * sz,
					cx * cy * sz + sx * sy * cz,
					cx * cy * cz - sx * sy * sz
				);
			case EulerOrder::XZY:
				return Quaternion(
					sx * cy * cz - cx * sy * sz,
					cx * sy * cz - sx * cy * sz,
					cx * cy * sz + sx * sy * cz,
					cx * cy * cz + sx * sy * sz
				);
			case EulerOrder::YXZ:
				return Quaternion(
					sx * cy * cz + cx * sy * sz,
					cx * sy * cz - sx * cy * sz,
					cx * cy * sz - sx * sy * cz,
					cx * cy * cz + sx * sy * sz
				);
			case EulerOrder::YZX:
				return Quaternion(
					sx * cy * cz + cx * sy * sz,
					cx * sy * cz + sx * cy * sz,
					cx * cy * sz - sx * sy * cz,
					cx * cy * cz - sx * sy * sz
				);
			case EulerOrder::ZXY:
				return Quaternion(
					sx * cy * cz - cx * sy * sz,
					cx * sy * cz + sx * cy * sz,
					cx * cy * sz + sx * sy * cz,
					cx * cy * cz - sx * sy * sz
				);
			case EulerOrder::ZYX:
				return Quaternion(
					sx * cy * cz - cx * sy * sz,
					cx * sy * cz + sx * cy * sz,
					cx * cy * sz - sx * sy * cz,
					cx * cy * cz + sx * sy * sz
				);
			default:
				return identity();
			}
		}

		static Quaternion fromEuler(T x, T y, T z) noexcept
		{
			return fromEulerEx(x, y, z, EulerOrder::ZYX);
		}

		static Quaternion fromEuler(const Vector<T, 3>& eulerAngles) noexcept
		{
			return fromEuler(eulerAngles.x, eulerAngles.y, eulerAngles.z);
		}

		Vector<T, 3> toEulerEx(EulerOrder order) const noexcept
		{
			Vector<T, 3> euler;
			T sqw = w * w; T sqx = x * x; T sqy = y * y; T sqz = z * z;
			T unit = sqx + sqy + sqz + sqw;

			switch (order)
			{
			case EulerOrder::XYZ:
			{
				T siny = Math<T>::val(2) * (w * y + x * z);
				siny = Math<T>::clamp(siny / unit, -1, 1);

				euler.y = Math<T>::asin(siny);

				if (Math<T>::abs(siny) >= Math<T>::AlmostOne)
				{
					euler.x = Math<T>::val(2) * Math<T>::atan2(x, w);
					euler.z = 0;
				}
				else
				{
					euler.x = Math<T>::atan2(Math<T>::val(-2) * (y * z - w * x), sqw - sqx - sqy + sqz);
					euler.z = Math<T>::atan2(Math<T>::val(-2) * (x * y - w * z), sqw + sqx - sqy - sqz);
				}
				break;
			}
			case EulerOrder::XZY:
			{
				T sinz = Math<T>::val(-2) * (x * y - w * z);
				sinz = Math<T>::clamp(sinz / unit, -1, 1);

				euler.z = Math<T>::asin(sinz);

				if (Math<T>::abs(sinz) >= Math<T>::AlmostOne)
				{
					euler.x = Math<T>::val(2) * Math<T>::atan2(x, w);
					euler.y = 0;
				}
				else
				{
					euler.x = Math<T>::atan2(Math<T>::val(2) * (y * z + w * x), sqw - sqx + sqy - sqz);
					euler.y = Math<T>::atan2(Math<T>::val(2) * (x * z + w * y), sqw + sqx - sqy - sqz);
				}
				break;
			}
			case EulerOrder::YXZ:
			{
				T sinx = Math<T>::val(2) * (w * x - y * z);
				sinx = Math<T>::clamp(sinx / unit, -1, 1);

				euler.x = Math<T>::asin(sinx);

				if (Math<T>::abs(sinx) >= Math<T>::AlmostOne)
				{
					euler.y = Math<T>::val(2) * Math<T>::atan2(y, w);
					euler.z = 0;
				}
				else
				{
					euler.y = Math<T>::atan2(Math<T>::val(2) * (x * z + w * y), sqw - sqx - sqy + sqz);
					euler.z = Math<T>::atan2(Math<T>::val(2) * (x * y + w * z), sqw - sqx + sqy - sqz);
				}

				break;
			}
			case EulerOrder::YZX:
			{
				T sinz = Math<T>::val(2) * (w * z + x * y);
				sinz = Math<T>::clamp(sinz / unit, -1, 1);

				euler.z = Math<T>::asin(sinz);

				if (Math<T>::abs(sinz) >= Math<T>::AlmostOne)
				{
					euler.y = Math<T>::val(2) * Math<T>::atan2(y, w);
					euler.x = 0;
				}
				else
				{
					euler.y = Math<T>::atan2(Math<T>::val(-2) * (x * z - w * y), sqw + sqx - sqy - sqz);
					euler.x = Math<T>::atan2(Math<T>::val(-2) * (y * z - w * x), sqw - sqx + sqy - sqz);
				}
				break;
			}
			case EulerOrder::ZXY:
			{
				T sinx = Math<T>::val(2) * (w * x + y * z);
				sinx = Math<T>::clamp(sinx / unit, -1, 1);

				euler.x = Math<T>::asin(sinx);

				if (Math<T>::abs(sinx) >= Math<T>::AlmostOne)
				{
					euler.z = Math<T>::val(2) * Math<T>::atan2(z, w);
					euler.y = 0;
				}
				else
				{
					euler.z = Math<T>::atan2(Math<T>::val(-2) * (x * y - w * z), sqw - sqx + sqy - sqz);
					euler.y = Math<T>::atan2(Math<T>::val(-2) * (x * z - w * y), sqw - sqx - sqy + sqz);
				}
				break;
			}
			case EulerOrder::ZYX:
			{
				T siny = Math<T>::val(2) * (w * y - z * x);
				siny = Math<T>::clamp(siny / unit, -1, 1);

				euler.y = Math<T>::asin(siny);

				if (Math<T>::abs(siny) >= Math<T>::AlmostOne)
				{
					euler.z = Math<T>::val(2) * Math<T>::atan2(z, w);
					euler.x = 0;
				}
				else
				{
					euler.z = Math<T>::atan2(Math<T>::val(2) * (x * y + w * z), sqw + sqx - sqy - sqz);
					euler.x = Math<T>::atan2(Math<T>::val(2) * (y * z + w * x), sqw - sqx - sqy + sqz);
				}
				break;
			}
			}
			return euler * Math<T>::Rad2Deg;
		}

		Vector<T, 3> toEuler() const noexcept
		{
			return toEulerEx(EulerOrder::ZXY);
		}

		Quaternion& setLookRotation(const Vector<T, 3>& direction, const Vector<T, 3>& up) noexcept
		{
			if (direction.sqrMagnitude() < Math<T>::Epsilon)
			{
				*this = identity();
				return *this;
			}

			Vector<T, 3> forward = direction.normalized();
			Vector<T, 3> right = Vector<T, 3>::cross(up, forward);

			if (right.sqrMagnitude() < Math<T>::Epsilon)
			{
				Vector<T, 3> fallback = (Math<T>::abs(forward.y) > Math<T>::val(0.9)) ? Vector<T, 3>(Math<T>::val(0), Math<T>::val(0), Math<T>::val(1)) : Vector<T, 3>(Math<T>::val(0), Math<T>::val(1), Math<T>::val(0));
				right = Vector<T, 3>::cross(fallback, forward).normalized();
			}
			else
			{
				right = right.normalized();
			}

			Vector<T, 3> actualUp = Vector<T, 3>::cross(forward, right);

			Matrix4x4<T> rot;
			rot(0, 0) = right.x;    rot(1, 0) = right.y;    rot(2, 0) = right.z;
			rot(0, 1) = actualUp.x; rot(1, 1) = actualUp.y; rot(2, 1) = actualUp.z;
			rot(0, 2) = forward.x;  rot(1, 2) = forward.y;  rot(2, 2) = forward.z;

			*this = fromMatrix(rot);
			return *this;
		}

		static Quaternion lookRotation(const Vector<T, 3>& direction, const Vector<T, 3>& up) noexcept
		{
			Quaternion q;
			q.setLookRotation(direction, up);
			return q;
		}

		T lengthSquared() const noexcept
		{
			return this->dot(*this);
		}

		T length() const noexcept
		{
			return Math<T>::sqrt(lengthSquared());
		}

		void normalize() noexcept
		{
			T len = length();
			if (len > Math<T>::Epsilon)
			{
				T invLen = Math<T>::val(1) / len;
				x *= invLen; y *= invLen; z *= invLen; w *= invLen;
			}
			else
			{
				*this = identity();
			}
		}

		Quaternion normalized() const noexcept
		{
			Quaternion q = *this;
			q.normalize();
			return q;
		}

		constexpr Quaternion conjugate() const noexcept
		{
			return Quaternion(-x, -y, -z, w);
		}

		constexpr Quaternion inverse() const noexcept
		{
			T norm = lengthSquared();
			if (norm > 0)
			{
				T invNorm = Math<T>::val(1) / norm;
				return Quaternion(-x * invNorm, -y * invNorm, -z * invNorm, w * invNorm);
			}
			return identity();
		}

		Vector<T, 3> rotate(const Vector<T, 3>& v) const noexcept
		{
			assert(Math<T>::abs(lengthSquared() - 1.0f) < 0.01f && "Quaternion must be normalized to rotate a vector!");

			Vector<T, 3> q_vec(x, y, z);
			Vector<T, 3> t = Vector<T, 3>::cross(q_vec, v) * Math<T>::val(2);
			return v + (t * w) + Vector<T, 3>::cross(q_vec, t);
		}

		Vector<T, 3> forward() const noexcept { return rotate(Vector<T, 3>(0, 0, 1)); }
		Vector<T, 3> up()      const noexcept { return rotate(Vector<T, 3>(0, 1, 0)); }
		Vector<T, 3> right()   const noexcept { return rotate(Vector<T, 3>(1, 0, 0)); }

		static T dot(const Quaternion& a, const Quaternion& b) noexcept
		{
			return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
		}

		T dot(const Quaternion& other) const noexcept
		{
			return x * other.x + y * other.y + z * other.z + w * other.w;
		}

		Matrix4x4<T> toMatrix() const noexcept
		{
			T xx = x * x; T yy = y * y; T zz = z * z;
			T xy = x * y; T xz = x * z; T yz = y * z;
			T xw = x * w; T yw = y * w; T zw = z * w;

			Matrix4x4<T> res = Matrix4x4<T>::identity();
			res(0, 0) = Math<T>::val(1) - Math<T>::val(2) * (yy + zz);
			res(1, 0) = Math<T>::val(2) * (xy + zw);
			res(2, 0) = Math<T>::val(2) * (xz - yw);

			res(0, 1) = Math<T>::val(2) * (xy - zw);
			res(1, 1) = Math<T>::val(1) - Math<T>::val(2) * (xx + zz);
			res(2, 1) = Math<T>::val(2) * (yz + xw);

			res(0, 2) = Math<T>::val(2) * (xz + yw);
			res(1, 2) = Math<T>::val(2) * (yz - xw);
			res(2, 2) = Math<T>::val(1) - Math<T>::val(2) * (xx + yy);
			return res;
		}

		static Quaternion fromMatrix(const Matrix4x4<T>& m) noexcept
		{
			auto rot = m.template slice<3, 3>(0, 0);

			T tr = rot(0, 0) + rot(1, 1) + rot(2, 2);
			T qx, qy, qz, qw;

			if (tr > 0) {
				T s = Math<T>::sqrt(tr + Math<T>::val(1.0)) * Math<T>::val(2.0);
				qw = Math<T>::val(0.25) * s;
				qx = (rot(2, 1) - rot(1, 2)) / s;
				qy = (rot(0, 2) - rot(2, 0)) / s;
				qz = (rot(1, 0) - rot(0, 1)) / s;
			}
			else if ((rot(0, 0) > rot(1, 1)) && (rot(0, 0) > rot(2, 2))) {
				T s = Math<T>::sqrt(Math<T>::val(1.0) + rot(0, 0) - rot(1, 1) - rot(2, 2)) * Math<T>::val(2.0);
				qw = (rot(2, 1) - rot(1, 2)) / s;
				qx = Math<T>::val(0.25) * s;
				qy = (rot(0, 1) + rot(1, 0)) / s;
				qz = (rot(0, 2) + rot(2, 0)) / s;
			}
			else if (rot(1, 1) > rot(2, 2)) {
				T s = Math<T>::sqrt(Math<T>::val(1.0) + rot(1, 1) - rot(0, 0) - rot(2, 2)) * Math<T>::val(2.0);
				qw = (rot(0, 2) - rot(2, 0)) / s;
				qx = (rot(0, 1) + rot(1, 0)) / s;
				qy = Math<T>::val(0.25) * s;
				qz = (rot(1, 2) + rot(2, 1)) / s;
			}
			else {
				T s = Math<T>::sqrt(Math<T>::val(1.0) + rot(2, 2) - rot(0, 0) - rot(1, 1)) * Math<T>::val(2.0);
				qw = (rot(1, 0) - rot(0, 1)) / s;
				qx = (rot(0, 2) + rot(2, 0)) / s;
				qy = (rot(1, 2) + rot(2, 1)) / s;
				qz = Math<T>::val(0.25) * s;
			}

			return Quaternion(qx, qy, qz, qw).normalized();
		}

		static Quaternion nlerpUnclamped(const Quaternion& a, const Quaternion& b, T t) noexcept
		{
			T dot = Quaternion::dot(a, b);
			T t1 = Math<T>::val(1) - t;
			Quaternion res = (dot < 0) ? (a * t1) - (b * t) : (a * t1) + (b * t);
			return res.normalized();
		}

		static Quaternion nlerp(const Quaternion& a, const Quaternion& b, T t) noexcept
		{
			return nlerpUnclamped(a, b, Math<T>::clamp(t, 0, 1));
		}

		static Quaternion slerpUnclamped(const Quaternion& a, const Quaternion& b, T t) noexcept
		{
			T dot = Quaternion::dot(a, b);

			Quaternion end = b;
			if (dot < Math<T>::val(0))
			{
				dot = -dot;
				end = -b;
			}

			if (dot > Math<T>::AlmostOne)
			{
				return nlerpUnclamped(a, end, t);
			}

			T theta_0 = Math<T>::acos(dot);
			T theta = theta_0 * t;

			T sin_theta = Math<T>::sin(theta);
			T sin_theta_0 = Math<T>::sin(theta_0);

			T s0 = Math<T>::cos(theta) - dot * sin_theta / sin_theta_0;
			T s1 = sin_theta / sin_theta_0;

			return ((a * s0) + (end * s1)).normalized();
		}

		static Quaternion slerp(const Quaternion& a, const Quaternion& b, T t) noexcept
		{
			return slerpUnclamped(a, b, Math<T>::clamp(t, 0, 1));
		}

		Quaternion operator*(const Quaternion& other) const noexcept
		{
			Quaternion res = *this;
			res *= other;
			return res;
		}

		Quaternion operator*(T scalar) const noexcept
		{
			Quaternion res = *this;
			res *= scalar;
			return res;
		}

		friend Quaternion operator*(T scalar, const Quaternion& q) noexcept
		{
			return q * scalar;
		}

		Vector<T, 3> operator*(const Vector<T, 3>& vec) const noexcept
		{
			return rotate(vec);
		}

		Quaternion operator+(const Quaternion& rhs) const noexcept
		{
			return Quaternion(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
		}

		Quaternion operator-(const Quaternion& rhs) const noexcept
		{
			return Quaternion(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
		}

		Quaternion operator-() const noexcept
		{
			return Quaternion(-x, -y, -z, -w);
		}

		Quaternion& operator*=(const Quaternion& other) noexcept
		{
			T tx = w * other.x + x * other.w + y * other.z - z * other.y;
			T ty = w * other.y + y * other.w + z * other.x - x * other.z;
			T tz = w * other.z + z * other.w + x * other.y - y * other.x;
			T tw = w * other.w - x * other.x - y * other.y - z * other.z;

			x = tx;
			y = ty;
			z = tz;
			w = tw;

			return *this;
		}

		Quaternion& operator*=(T scalar) noexcept
		{
			x *= scalar;
			y *= scalar;
			z *= scalar;
			w *= scalar;
			return *this;
		}

		Quaternion& operator+=(const Quaternion& other) noexcept
		{
			x += other.x;
			y += other.y;
			z += other.z;
			w += other.w;
			return *this;
		}

		Quaternion& operator-=(const Quaternion& rhs) noexcept
		{
			x -= rhs.x;
			y -= rhs.y;
			z -= rhs.z;
			w -= rhs.w;
			return *this;
		}

		T operator[](size_t i) const noexcept
		{
			assert(i < 4 && "Can't index more than four elements (x, y, z, w)!");
			return data[i];
		}

		T& operator[](size_t i) noexcept
		{
			assert(i < 4 && "Can't index more than four elements (x, y, z, w)!");
			return data[i];
		}

		static bool isEqual(const Quaternion& lhs,  const Quaternion& rhs, T epsilon = Math<T>::Epsilon) noexcept
		{
			for (size_t i = 0; i < 4; i++)
			{
				if (!Math<T>::isEqual(lhs[i], rhs[i], epsilon)) return false;
			}

			return true;
		}

		bool equals(const Quaternion& rhs, T epsilon = Math<T>::Epsilon) const noexcept
		{
			return isEqual(*this, rhs, epsilon);
		}

		bool operator==(const Quaternion& rhs) const noexcept
		{
			return equals(rhs);
		}

		bool operator!=(const Quaternion& rhs) const noexcept
		{
			return !equals(rhs);
		}

		std::string toString() const
		{
			std::stringstream ss;
			ss << "(" << x << ", " << y << ", " << z << ", " << w << ")";
			return ss.str();
		}

		friend std::ostream& operator<<(std::ostream& os, const detail::Quaternion<T>& q)
		{
			os << q.toString();
			return os;
		}
	};
}

#endif