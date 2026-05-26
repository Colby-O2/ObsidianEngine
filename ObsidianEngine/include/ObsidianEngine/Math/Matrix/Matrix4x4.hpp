#ifndef __OBSIDIANENGINE_MATH_MATRIX4x4_HPP__
#define __OBSIDIANENGINE_MATH_MATRIX4x4_HPP__

#include "ObsidianEngine/Math/MathUtils.hpp"
#include "ObsidianEngine/Math/Matrix/MatrixMxN.hpp"

#include "ObsidianEngine/Math/Quaternion/QuaternionTypes.hpp"
#include "ObsidianEngine/Math/Vector/VectorTypes.hpp"

#include <numbers>
#include <limits>

namespace ObsidianEngine::detail
{
	template<std::floating_point T>
	struct Matrix4x4 : public Matrix<4, 4, T>
	{
		using Base = Matrix<4, 4, T>;
		using Base::Base;

		Matrix4x4(const Base& other) : Base(other) {}

		T determinant() const
		{
			const auto& m = *this;
			return m(0, 0) * (m(1, 1) * m(2, 2) * m(3, 3) + m(1, 2) * m(2, 3) * m(3, 1) + m(1, 3) * m(2, 1) * m(3, 2)
				- m(1, 3) * m(2, 2) * m(3, 1) - m(1, 2) * m(2, 1) * m(3, 3) - m(1, 1) * m(2, 3) * m(3, 2))
				- m(1, 0) * (m(0, 1) * m(2, 2) * m(3, 3) + m(0, 2) * m(2, 3) * m(3, 1) + m(0, 3) * m(2, 1) * m(3, 2)
					- m(0, 3) * m(2, 2) * m(3, 1) - m(0, 2) * m(2, 1) * m(3, 3) - m(0, 1) * m(2, 3) * m(3, 2))
				+ m(2, 0) * (m(0, 1) * m(1, 2) * m(3, 3) + m(0, 2) * m(1, 3) * m(3, 1) + m(0, 3) * m(1, 1) * m(3, 2)
					- m(0, 3) * m(1, 2) * m(3, 1) - m(0, 2) * m(1, 1) * m(3, 3) - m(0, 1) * m(1, 3) * m(3, 2))
				- m(3, 0) * (m(0, 1) * m(1, 2) * m(2, 3) + m(0, 2) * m(1, 3) * m(2, 1) + m(0, 3) * m(1, 1) * m(2, 2)
					- m(0, 3) * m(1, 2) * m(2, 1) - m(0, 2) * m(1, 1) * m(2, 3) - m(0, 1) * m(1, 3) * m(2, 2));
		}

		Matrix4x4 inverse() const
		{
			T det = this->determinant();

			if (Math<T>::abs(det) < Math<T>::val(1e-7))
			{
				return Base::identity();
			}

			T invDet = Math<T>::val(1) / det;
			Matrix4x4 result;

			for (size_t r = 0; r < 4; ++r)
			{
				for (size_t c = 0; c < 4; ++c)
				{
					result(c, r) = this->cofactor(r, c) * invDet;
				}
			}
			return result;
		}

		Vector<T, 3> multiplyPoint(const Vector<T, 3>& p) const
		{
			Vector<T, 4> r = (*this) * Vector<T, 4>{ p.x, p.y, p.z, 1.0f };
			return { r.x, r.y, r.z };
		}

		Vector<T, 3> multiplyDirection(const Vector<T, 3>& v) const
		{
			Vector<T, 4> r = (*this) * Vector<T, 4>{ v.x, v.y, v.z, 0.0f };
			return { r.x, r.y, r.z };
		}

		Vector<T, 3> lossyScale() const
		{
			T x = Vector<T, 3>{ (*this)(0, 0), (*this)(1, 0), (*this)(2, 0) }.sqrMagnitude();
			T y = Vector<T, 3>{ (*this)(0, 1), (*this)(1, 1), (*this)(2, 1) }.sqrMagnitude();
			T z = Vector<T, 3>{ (*this)(0, 2), (*this)(1, 2), (*this)(2, 2) }.sqrMagnitude();

			return Vector<T, 3>{
				Math<T>::sqrt(x),
					Math<T>::sqrt(y),
					Math<T>::sqrt(z)
			};
		}

		Quaternion<T> rotation() const
		{
			Matrix4x4 m = *this;

			Vector<T, 3> right(
				m(0, 0), m(1, 0), m(2, 0)
			);

			Vector<T, 3> up(
				m(0, 1), m(1, 1), m(2, 1)
			);

			Vector<T, 3> forward(
				m(0, 2), m(1, 2), m(2, 2)
			);

			right.normalize();
			up.normalize();
			forward.normalize();

			m(0, 0) = right.x;
			m(1, 0) = right.y;
			m(2, 0) = right.z;

			m(0, 1) = up.x;
			m(1, 1) = up.y;
			m(2, 1) = up.z;

			m(0, 2) = forward.x;
			m(1, 2) = forward.y;
			m(2, 2) = forward.z;

			return Quaternion<T>::fromMatrix(m);
		}

		Vector<T, 3> position() const
		{
			return Vector<T, 3>{
				(*this)(0, 3),
				(*this)(1, 3),
				(*this)(2, 3)
			};
		}

		static Matrix4x4 rotateX(T r)
		{
			T c = Math<T>::cos(r);
			T s = Math<T>::sin(r);
			Matrix4x4 m = Base::identity();
			m(1, 1) = c;  m(1, 2) = -s;
			m(2, 1) = s;  m(2, 2) = c;
			return m;
		}

		static Matrix4x4 rotateY(T r)
		{
			T c = Math<T>::cos(r);
			T s = Math<T>::sin(r);
			Matrix4x4 m = Base::identity();
			m(0, 0) = c;  m(0, 2) = s;
			m(2, 0) = -s; m(2, 2) = c;
			return m;
		}

		static Matrix4x4 rotateZ(T r)
		{
			T c = Math<T>::cos(r);
			T s = Math<T>::sin(r);
			Matrix4x4 m = Base::identity();
			m(0, 0) = c;  m(0, 1) = -s;
			m(1, 0) = s;  m(1, 1) = c;
			return m;
		}

		static Matrix4x4 rotate(const Quaternion<T>& q)
		{
			return q.toMatrix();
		}

		static Matrix4x4 rotate(const Vector<T, 3>& e)
		{
			return Quaternion<T>::fromEuler(e).toMatrix();
		}

		static Matrix4x4 translate(const Vector<T, 3>& t)
		{
			Matrix4x4 m = Base::identity();
			m(0, 3) = t.x;
			m(1, 3) = t.y;
			m(2, 3) = t.z;
			return m;
		}

		static Matrix4x4 scale(const Vector<T, 3>& s)
		{
			Matrix4x4 m = Base::identity();
			m(0, 0) = s.x;
			m(1, 1) = s.y;
			m(2, 2) = s.z;
			return m;
		}

		static Matrix4x4 trs(const Vector<T, 3>& pos, const Quaternion<T>& rot, const Vector<T, 3>& s)
		{
			return translate(pos) * rotate(rot) * scale(s);
		}

		bool validTRS() const
		{
			return Math<T>::abs(this->determinant()) > Math<T>::val(1e-9);
		}

		static Matrix4x4 lookAt(const Vector<T, 3>& eye, const Vector<T, 3>& target, const Vector<T, 3>& up)
		{
			Vector<T, 3> f = (target - eye).normalized();
			Vector<T, 3> s = Vector<T, 3>::cross(f, up).normalized();
			Vector<T, 3> u = Vector<T, 3>::cross(s, f);

			Matrix4x4 m = Base::zero();

			m(0, 0) = s.x;  m(1, 0) = u.x;  m(2, 0) = -f.x; m(3, 0) = 0;
			m(0, 1) = s.y;  m(1, 1) = u.y;  m(2, 1) = -f.y; m(3, 1) = 0;
			m(0, 2) = s.z;  m(1, 2) = u.z;  m(2, 2) = -f.z; m(3, 2) = 0;

			m(0, 3) = -Vector<T, 3>::dot(s, eye);
			m(1, 3) = -Vector<T, 3>::dot(u, eye);
			m(2, 3) = Vector<T, 3>::dot(f, eye);
			m(3, 3) = 1;

			return m;
		}

		static Matrix4x4 perspective(T fov, T aspect, T nearZ, T farZ)
		{
			T fovRad = fov * Math<T>::Deg2Rad;
			T f = Math<T>::val(1.0) / Math<T>::tan(fovRad / Math<T>::val(2.0));

			Matrix4x4 m = Base::zero();
			m(0, 0) = f / aspect;
			m(1, 1) = f;
			m(2, 2) = -(farZ + nearZ) / (farZ - nearZ);
			m(2, 3) = -(Math<T>::val(2) * farZ * nearZ) / (farZ - nearZ);
			m(3, 2) = -1;
			m(3, 3) = 0;
			return m;
		}

		static Matrix4x4 ortho(T left, T right, T bottom, T top, T nearZ, T farZ)
		{
			Matrix4x4 m = Base::identity();
			m(0, 0) = Math<T>::val(2) / (right - left);
			m(1, 1) = Math<T>::val(2) / (top - bottom);
			m(2, 2) = -Math<T>::val(2) / (farZ - nearZ);

			m(0, 3) = -(right + left) / (right - left);
			m(1, 3) = -(top + bottom) / (top - bottom);
			m(2, 3) = -(farZ + nearZ) / (farZ - nearZ);

			return m;
		}

		static Matrix4x4 frustum(T left, T right, T bottom, T top, T nearZ, T farZ)
		{
			T rl = right - left;
			T tb = top - bottom;
			T fn = farZ - nearZ;

			Matrix4x4 m = Base::zero();
			m(0, 0) = (Math<T>::val(2) * nearZ) / rl;
			m(1, 1) = (Math<T>::val(2) * nearZ) / tb;

			m(0, 2) = (right + left) / rl;
			m(1, 2) = (top + bottom) / tb;
			m(2, 2) = -(farZ + nearZ) / fn;
			m(2, 3) = -(Math<T>::val(2) * farZ * nearZ) / fn;

			m(3, 2) = -1;
			m(3, 3) = 0;
			return m;
		}
	};
}

#endif