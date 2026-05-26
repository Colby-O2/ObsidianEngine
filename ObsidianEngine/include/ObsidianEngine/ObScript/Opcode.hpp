#ifndef __OBSIDIANENGINE_OBSCRIPT_OPCODE_HPP__
#define __OBSIDIANENGINE_OBSCRIPT_OPCODE_HPP__

#include "ObsidianEngine/ObScript/ScriptValue.hpp"

#include <cstdint>
#include <vector>

namespace ObsidianEngine
{
	enum class Opcode : uint8_t
	{
		Constant, Load, Store,

		Add, Subtract, Multiply, Divide, Modulus, Negate,
		Equal, NoEqual, Greater, Less,

		Jump, JumpIfFalse,

		Return, Call
	};

	struct Chunk
	{
		std::vector<uint8_t> bytecode;
		std::vector<ScriptValue> constants;
	};
}

#endif