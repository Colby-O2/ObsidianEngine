#ifndef __OBSIDIANENGINE_MATH_SWIZZLE_H__
#define __OBSIDIANENGINE_MATH_SWIZZLE_H__

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

	template<typename T, size_t... Indices>
	struct SwizzleProxy
	{
		static constexpr size_t Size = sizeof...(Indices);

		T* data;

		operator Vector<T, Size>() const
		{
			return eval();
		}

		Vector<T, Size> eval() const;

		SwizzleProxy& operator=(const Vector<T, Size>& rhs);
		T& operator[](size_t i);
		const T& operator[](size_t i) const;

		friend std::ostream& operator<<(std::ostream& os, const SwizzleProxy& v);
	};
}

#endif