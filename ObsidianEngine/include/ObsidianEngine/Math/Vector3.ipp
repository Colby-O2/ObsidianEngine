namespace ObsidianEngine::detail
{
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

	template<typename T, size_t... Indices>
	SwizzleProxy<T, Indices...>::operator Vector<T, Size>() const
	{
		return eval();
	}

}