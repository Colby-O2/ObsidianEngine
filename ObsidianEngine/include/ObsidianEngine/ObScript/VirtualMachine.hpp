#ifndef __OBSIDIANENGINE_OBSCRIPT_VIRTUAL_MACHINE_HPP__
#define __OBSIDIANENGINE_OBSCRIPT_VIRTUAL_MACHINE_HPP__

#include "ObsidianEngine/ObScript/Opcode.hpp"
#include "ObsidianEngine/ObScript/AST.hpp"
#include "ObsidianEngine/ObScript/ScriptValue.hpp"

#include <memory>
#include <vector>
#include <cstdint>
#include <string>
#include <unordered_map>

namespace ObsidianEngine
{
	class VirtualMachine
	{
	public:
        using Environment = std::unordered_map<std::string, ScriptValue>;

		void run(std::shared_ptr<Chunk> chunk, Environment& localEnv, const Environment& globalEnv)
		{
            execute(chunk, localEnv, globalEnv);
		}

        void call(const std::string& functionName, const std::vector<ScriptValue>& args,
            Environment& localEnv, const Environment& globalEnv)
        {
            auto it = localEnv.find(functionName);
            ScriptValue calleeValue;

            if (it != localEnv.end())
            {
                calleeValue = it->second;
            }
            else if (auto git = globalEnv.find(functionName); git != globalEnv.end())
            {
                calleeValue = git->second;
            }
            else
            {
                throw std::runtime_error("VM Error: Function '" + functionName + "' does not exist!");
            }

            if (calleeValue.type == ScriptValueType::NativeFunction)
            {
                auto nativeFunc = std::get<std::shared_ptr<ScriptNativeFunction>>(calleeValue.data);
                ScriptValue res = nativeFunc->function(args);
                m_stack.push_back(res);
                return;
            }

            if (calleeValue.type != ScriptValueType::Function)
            {
                throw std::runtime_error("VM Error: Property '" + functionName + "' is not callable.");
            }

            auto func = std::get<std::shared_ptr<ScriptFunction>>(calleeValue.data);

            for (size_t i = 0; i < func->parameterNames.size() && i < args.size(); ++i)
            {
                localEnv[func->parameterNames[i]] = args[i];
            }

            execute(func->chunk, localEnv, globalEnv);
        }

	private:
		std::shared_ptr<Chunk> m_chunk;
		size_t m_ptr;
		std::vector<ScriptValue> m_stack;

        ScriptValue resolveVariable(const std::string& name, const Environment& localEnv, const Environment& globalEnv)
        {
            auto lit = localEnv.find(name);
            if (lit != localEnv.end()) return lit->second;

            auto git = globalEnv.find(name);
            if (git != globalEnv.end()) return git->second;

            return ScriptValue();
        }

        void execute(std::shared_ptr<Chunk> chunk, Environment& localEnv, const Environment& globalEnv)
        {
            m_chunk = chunk;
            m_ptr = 0;

            while (m_ptr < m_chunk->bytecode.size())
            {
                uint8_t instruction = readByte();
                switch (static_cast<Opcode>(instruction))
                {
                case Opcode::Constant:
                {
                    uint8_t idx = readByte();
                    m_stack.push_back(m_chunk->constants[idx]);
                    break;
                }
                case Opcode::Load:
                {
                    uint8_t idx = readByte();
                    std::string name = std::get<std::string>(m_chunk->constants[idx].data);
                    m_stack.push_back(resolveVariable(name, localEnv, globalEnv));
                    break;
                }
                case Opcode::Store:
                {
                    uint8_t idx = readByte();
                    if (m_chunk->constants[idx].type != ScriptValueType::String) 
                    {
                        throw std::runtime_error("VM Error: Identifier index is not a string.");
                    }
                    std::string name = std::get<std::string>(m_chunk->constants[idx].data);

                    if (m_stack.empty())
                    {
                        throw std::runtime_error("VM Error: Stack underflow during Store assignment to '" + name + "'");
                    }
                    localEnv[name] = popStack();
                    break;
                }
                case Opcode::Negate:
                {
                    double a = std::get<double>(popStack().data);
                    m_stack.push_back(ScriptValue(-a));
                    break;
                }
                case Opcode::Add:
                {
                    double b = std::get<double>(popStack().data);
                    double a = std::get<double>(popStack().data);
                    m_stack.push_back(ScriptValue(a + b));
                    break;
                }
                case Opcode::Subtract:
                {
                    double b = std::get<double>(popStack().data);
                    double a = std::get<double>(popStack().data);
                    m_stack.push_back(ScriptValue(a - b));
                    break;
                }
                case Opcode::Divide:
                {
                    double b = std::get<double>(popStack().data);
                    double a = std::get<double>(popStack().data);
                    m_stack.push_back(ScriptValue(a / b));
                    break;
                }
                case Opcode::Modulus:
                {
                    double b = std::get<double>(popStack().data);
                    double a = std::get<double>(popStack().data);

                    double div = a / b;
                    long long q = static_cast<long long>(div);

                    if (static_cast<double>(q) > div) 
                    {
                        q -= 1;
                    }

                    double mod = a - b * q;

                    m_stack.push_back(ScriptValue(mod));
                    break;
                }
                case Opcode::Multiply:
                {
                    double b = std::get<double>(popStack().data);
                    double a = std::get<double>(popStack().data);
                    m_stack.push_back(ScriptValue(a * b));
                    break;
                }
                case Opcode::Equal:
                {
                    double b = std::get<double>(popStack().data);
                    double a = std::get<double>(popStack().data);
                    m_stack.push_back(ScriptValue(a == b));
                    break;
                }
                case Opcode::Greater:
                {
                    double b = std::get<double>(popStack().data);
                    double a = std::get<double>(popStack().data);
                    m_stack.push_back(ScriptValue(a > b));
                    break;
                }
                case Opcode::Less:
                {
                    double b = std::get<double>(popStack().data);
                    double a = std::get<double>(popStack().data);
                    m_stack.push_back(ScriptValue(a < b));
                    break;
                }
                case Opcode::JumpIfFalse:
                {
                    uint16_t offset = (readByte() << 8) | readByte();
                    bool condition = std::get<bool>(popStack().data);
                    if (!condition) m_ptr += offset;
                    break;
                }
                case Opcode::Jump:
                {
                    uint16_t offset = (readByte() << 8) | readByte();
                    m_ptr += offset;
                    break;
                }
                case Opcode::Call:
                {
                    uint8_t argCount = readByte();

                    std::vector<ScriptValue> args(argCount);
                    for (int i = argCount - 1; i >= 0; --i)
                    {
                        args[i] = popStack();
                    }

                    ScriptValue callee = popStack();

                    if (callee.type == ScriptValueType::NativeFunction)
                    {
                        auto nativeFunc = std::get<std::shared_ptr<ScriptNativeFunction>>(callee.data);
                        ScriptValue result = nativeFunc->function(args);
                        m_stack.push_back(result);
                    }
                    else if (callee.type == ScriptValueType::Function)
                    {
                        //TODO
                    }
                    else
                    {
                        throw std::runtime_error("VM Error: Property is not callable.");
                    }
                    break;
                }
                case Opcode::Return:
                {
                    return;
                }
                default:
                    throw std::runtime_error("VM Error: Unknown instruction opcode encountered '" + std::to_string(static_cast<int>(instruction)) + "'.");
                }
            }
        }

		uint8_t readByte() 
		{ 
			return m_chunk->bytecode[m_ptr++]; 
		}

		ScriptValue popStack() 
		{
			if (m_stack.empty()) return ScriptValue();
			ScriptValue val = m_stack.back();
			m_stack.pop_back();
			return val;
		}
	};
}

#endif