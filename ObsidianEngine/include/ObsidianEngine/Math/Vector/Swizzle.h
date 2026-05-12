#ifndef __OBSIDIANENGINE_MATH_SWIZZLE_H__
#define __OBSIDIANENGINE_MATH_SWIZZLE_H__

#include "ObsidianEngine/StringLiteral.h"

namespace ObsidianEngine::detail
{
	template<typename T, size_t N>
	struct Vector;

	constexpr size_t swizzleIndex(char c)
	{
		switch (c)
		{
		case 'x': return 0;
		case 'y': return 1;
		case 'z': return 2;
		case 'w': return 3;

		case 'r': return 0;
		case 'g': return 1;
		case 'b': return 2;
		case 'a': return 3;
		}

		return static_cast<size_t>(c - '0');
	}

	template<size_t...>
	struct HasDuplicates : std::false_type {};

	template<size_t First, size_t... Rest>
	struct HasDuplicates<First, Rest...>
	{
		static constexpr bool value = ((First == Rest) || ...) || HasDuplicates<Rest...>::value;
	};

	template<StringLiteral Str>
	constexpr auto parseSwizzle()
	{
		constexpr size_t Count = sizeof(Str.value) - 1;

		std::array<size_t, Count> indices{};
		for (size_t i = 0; i < Count; ++i)
		{
			indices[i] = swizzleIndex(Str.value[i]);
		}
		return indices;
	}

	template<typename T, size_t... Indices>
	struct SwizzleProxy
	{
		static constexpr size_t Size = sizeof...(Indices);

		T* data;

		operator Vector<T, Size>() const
		{
			return eval();
		}

		Vector<T, Size> eval() const
		{
			return Vector<T, Size>{ data[Indices]... };
		}

		template<typename U>
		operator Vector<U, Size>() const
		{
			return Vector<U, Size>(eval());
		}

		template<typename U, size_t N>
		operator Vector<U, N>() const
		{
			Vector<U, N> result{};

			constexpr size_t M = Size;
			constexpr size_t copyCount = (N < M) ? N : M;

			for (size_t i = 0; i < copyCount; i++)
			{
				result[i] = static_cast<U>((*this)[i]);
			}

			return result;
		}

		template<StringLiteral Str>
		constexpr auto swizzle() const
		{
			constexpr size_t len = sizeof(Str.value) - 1;

			static_assert(
				[]<size_t... I>(std::index_sequence<I...>) {
				return ((swizzleIndex(Str.value[I]) < Size) && ...);
			}(std::make_index_sequence<len>{}),
				"Swizzle index out of range for this Proxy!"
				);

			return swizzleToPack<Str>(std::make_index_sequence<len>{});
		}

		SwizzleProxy& operator=(const Vector<T, Size>& rhs)
		{
			static_assert(!HasDuplicates<Indices...>::value, "Cannot assign to swizzle with duplicate components!");

			size_t i = 0;
			((data[Indices] = rhs[i++]), ...);

			return *this;
		}

		T& operator[](size_t i)
		{
			assert(i < Size && "Vector index out of range!");
			constexpr size_t lookup[] = { Indices... };
			return data[lookup[i]];
		}

		const T& operator[](size_t i) const
		{
			assert(i < Size && "Vector index out of range!");
			constexpr size_t lookup[] = { Indices... };
			return data[lookup[i]];
		}

		friend Vector<T, Size> operator+(const SwizzleProxy& lhs, const Vector<T, Size>& rhs)
		{
			return lhs.eval() + rhs;
		}

		friend Vector<T, Size> operator-(const SwizzleProxy& lhs, const Vector<T, Size>& rhs)
		{
			return lhs.eval() - rhs;
		}

		friend Vector<T, Size> operator*(const SwizzleProxy& lhs, const Vector<T, Size>& rhs)
		{
			return lhs.eval() * rhs;
		}

		friend Vector<T, Size> operator+(const Vector<T, Size>& lhs, const SwizzleProxy& rhs)
		{
			return lhs + rhs.eval();
		}

		friend Vector<T, Size> operator-(const Vector<T, Size>& lhs, const SwizzleProxy& rhs)
		{
			return lhs - rhs.eval();
		}

		friend Vector<T, Size> operator*(const Vector<T, Size>& lhs, const SwizzleProxy& rhs)
		{
			return lhs * rhs.eval();
		}

		template<size_t... OtherIndices>
		friend Vector<T, Size> operator+(const SwizzleProxy& lhs, const SwizzleProxy<T, OtherIndices...>& rhs)
		{
			return lhs.eval() + rhs.eval();
		}

		template<size_t... OtherIndices>
		friend Vector<T, Size> operator-(const SwizzleProxy& lhs, const SwizzleProxy<T, OtherIndices...>& rhs)
		{
			return lhs.eval() - rhs.eval();
		}

		template<size_t... OtherIndices>
		friend Vector<T, Size> operator*(const SwizzleProxy& lhs, const SwizzleProxy<T, OtherIndices...>& rhs)
		{
			return lhs.eval() * rhs.eval();
		}

		friend Vector<T, Size> operator-(const SwizzleProxy& lhs)
		{
			return -lhs.eval();
		
		}

		friend std::ostream& operator<<(std::ostream& os, const SwizzleProxy& v)
		{
			os << v.eval();
			return os;
		}
	private:
		template<size_t... LocalIndices>
		constexpr auto makeSwizzle() const
		{
			constexpr size_t current[] = { Indices... };
			return SwizzleProxy<T, current[LocalIndices]...>{ data };
		}

		template<StringLiteral Str, size_t... I>
		constexpr auto swizzleToPack(std::index_sequence<I...>) const
		{
			return makeSwizzle<swizzleIndex(Str.value[I])...>();
		}
	};

	template<typename T, size_t... Indices, typename U>
	inline auto operator*(const SwizzleProxy<T, Indices...>& lhs, const Vector<U, sizeof...(Indices)>& rhs) -> Vector<std::common_type_t<T, U>, sizeof...(Indices)>
	{
		return lhs.eval() * rhs;
	}

	template<typename T, size_t... Indices, typename U>
	inline auto operator*(const Vector<U, sizeof...(Indices)>& lhs, const SwizzleProxy<T, Indices...>& rhs) -> Vector<std::common_type_t<T, U>, sizeof...(Indices)>
	{
		return lhs * rhs.eval();
	}

	template<typename T, size_t... Indices, typename U, size_t... OtherIndices>
	inline auto operator*(const SwizzleProxy<T, Indices...>& lhs, const SwizzleProxy<U, OtherIndices...>& rhs) -> Vector<std::common_type_t<T, U>, sizeof...(Indices)>
	{
		static_assert(sizeof...(Indices) == sizeof...(OtherIndices), "Swizzles must be the same size!");
		return lhs.eval() * rhs.eval();
	}

	template<typename T, size_t... Indices, typename U>
	inline auto operator+(const SwizzleProxy<T, Indices...>& lhs, const Vector<U, sizeof...(Indices)>& rhs) -> Vector<std::common_type_t<T, U>, sizeof...(Indices)>
	{
		return lhs.eval() + rhs;
	}

	template<typename T, size_t... Indices, typename U>
	inline auto operator+(const Vector<U, sizeof...(Indices)>& lhs, const SwizzleProxy<T, Indices...>& rhs) -> Vector<std::common_type_t<T, U>, sizeof...(Indices)>
	{
		return lhs + rhs.eval();
	}

	template<typename T, size_t... Indices, typename U, size_t... OtherIndices>
	inline auto operator+(const SwizzleProxy<T, Indices...>& lhs, const SwizzleProxy<U, OtherIndices...>& rhs) -> Vector<std::common_type_t<T, U>, sizeof...(Indices)>
	{
		static_assert(sizeof...(Indices) == sizeof...(OtherIndices), "Swizzles must be the same size!");
		return lhs.eval() + rhs.eval();
	}
	template<typename T, size_t... Indices, typename U>
	inline auto operator-(const SwizzleProxy<T, Indices...>& lhs, const Vector<U, sizeof...(Indices)>& rhs) -> Vector<std::common_type_t<T, U>, sizeof...(Indices)>
	{
		return lhs.eval() - rhs;
	}

	template<typename T, size_t... Indices, typename U>
	inline auto operator-(const Vector<U, sizeof...(Indices)>& lhs, const SwizzleProxy<T, Indices...>& rhs) -> Vector<std::common_type_t<T, U>, sizeof...(Indices)>
	{
		return lhs - rhs.eval();
	}

	template<typename T, size_t... Indices, typename U, size_t... OtherIndices>
	inline auto operator-(const SwizzleProxy<T, Indices...>& lhs, const SwizzleProxy<U, OtherIndices...>& rhs) -> Vector<std::common_type_t<T, U>, sizeof...(Indices)>
	{
		static_assert(sizeof...(Indices) == sizeof...(OtherIndices), "Swizzles must be the same size!");
		return lhs.eval() - rhs.eval();
	}
}

#endif