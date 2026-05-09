#ifndef STRING_LITERAL_H_
#define STRING_LITERAL_H_

namespace ObsidianEngine
{
	template<size_t N>
	struct StringLiteral {
		constexpr StringLiteral(const char(&str)[N]) {
			std::copy_n(str, N, value);
		}

		char value[N];
	};
}

#endif