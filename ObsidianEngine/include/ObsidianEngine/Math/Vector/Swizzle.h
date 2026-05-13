#ifndef __OBSIDIANENGINE_MATH_SWIZZLE_H__
#define __OBSIDIANENGINE_MATH_SWIZZLE_H__

#include "ObsidianEngine/StringLiteral.h"

namespace ObsidianEngine::detail
{
	template<typename T, size_t N>
	struct Vector;

	constexpr size_t swizzleIndex(char c) noexcept
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
	constexpr auto parseSwizzle() noexcept
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

		operator Vector<T, Size>() const noexcept
		{
			return eval();
		}

		Vector<T, Size> eval() const noexcept
		{
			return Vector<T, Size>{ data[Indices]... };
		}

		template<typename U>
		requires std::is_arithmetic_v<U>
		auto cast() const noexcept
		{
			return eval().cast<U>();
		}

		template<size_t M>
		auto resize() const noexcept
		{
			return eval().cast<M>();
		}

		template<typename U, size_t M>
		requires std::is_arithmetic_v<U>
		auto resizeAs() const noexcept
		{
			return eval().cast<U, M>();
		}

		template<StringLiteral Str>
		constexpr auto swizzle() const noexcept
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

		SwizzleProxy& operator=(const Vector<T, Size>& rhs) noexcept
		{
			static_assert(!HasDuplicates<Indices...>::value, "Cannot assign to swizzle with duplicate components!");

			size_t i = 0;
			((data[Indices] = rhs[i++]), ...);

			return *this;
		}

		T& operator[](size_t i) noexcept
		{
			assert(i < Size && "Vector index out of range!");
			constexpr size_t lookup[] = { Indices... };
			return data[lookup[i]];
		}

		const T& operator[](size_t i) const noexcept
		{
			assert(i < Size && "Vector index out of range!");
			constexpr size_t lookup[] = { Indices... };
			return data[lookup[i]];
		}

		friend Vector<T, Size> operator+(const SwizzleProxy& lhs, const Vector<T, Size>& rhs) noexcept
		{
			return lhs.eval() + rhs;
		}

		friend Vector<T, Size> operator-(const SwizzleProxy& lhs, const Vector<T, Size>& rhs) noexcept
		{
			return lhs.eval() - rhs;
		}

		friend Vector<T, Size> operator*(const SwizzleProxy& lhs, const Vector<T, Size>& rhs) noexcept
		{
			return lhs.eval() * rhs;
		}

		friend Vector<T, Size> operator+(const Vector<T, Size>& lhs, const SwizzleProxy& rhs) noexcept
		{
			return lhs + rhs.eval();
		}

		friend Vector<T, Size> operator-(const Vector<T, Size>& lhs, const SwizzleProxy& rhs) noexcept
		{
			return lhs - rhs.eval();
		}

		friend Vector<T, Size> operator*(const Vector<T, Size>& lhs, const SwizzleProxy& rhs) noexcept
		{
			return lhs * rhs.eval();
		}

		template<size_t... OtherIndices>
		friend Vector<T, Size> operator+(const SwizzleProxy& lhs, const SwizzleProxy<T, OtherIndices...>& rhs) noexcept
		{
			return lhs.eval() + rhs.eval();
		}

		template<size_t... OtherIndices>
		friend Vector<T, Size> operator-(const SwizzleProxy& lhs, const SwizzleProxy<T, OtherIndices...>& rhs) noexcept
		{
			return lhs.eval() - rhs.eval();
		}

		template<size_t... OtherIndices>
		friend Vector<T, Size> operator*(const SwizzleProxy& lhs, const SwizzleProxy<T, OtherIndices...>& rhs) noexcept
		{
			return lhs.eval() * rhs.eval();
		}

		friend Vector<T, Size> operator-(const SwizzleProxy& lhs) noexcept
		{
			return -lhs.eval();
		
		}

		friend std::ostream& operator<<(std::ostream& os, const SwizzleProxy& v) noexcept
		{
			os << v.eval();
			return os;
		}
	private:
		template<size_t... LocalIndices>
		constexpr auto makeSwizzle() const noexcept
		{
			constexpr size_t current[] = { Indices... };
			return SwizzleProxy<T, current[LocalIndices]...>{ data };
		}

		template<StringLiteral Str, size_t... I>
		constexpr auto swizzleToPack(std::index_sequence<I...>) const noexcept
		{
			return makeSwizzle<swizzleIndex(Str.value[I])...>();
		}
	};
}

#endif