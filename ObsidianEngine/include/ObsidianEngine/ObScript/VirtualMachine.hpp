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

        void run
        (
            std::shared_ptr<Chunk> chunk,
            Environment& localEnv,
            const Environment& globalEnv
        )
        {
            execute(chunk, localEnv, localEnv, localEnv, globalEnv);
        }

        void run
        (
            std::shared_ptr<Chunk> chunk,
            Environment& frameEnv,
            Environment& localEnv,
            const Environment& globalEnv
        )
        {
            execute(chunk, frameEnv, localEnv, localEnv, globalEnv);
        }

        void call
        (
            const std::string& functionName,
            const std::vector<ScriptValue>& args,
            Environment& componentEnv,
            Environment& fileEnv,
            const Environment& globalEnv,
            std::shared_ptr<ScriptComponentRef> selfContext = nullptr
        )
        {
            auto it = componentEnv.find(functionName);
            ScriptValue calleeValue;

            if (it != componentEnv.end())
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

            Environment frameEnv;
            m_currentSelf = selfContext;

            for (size_t i = 0; i < func->parameterNames.size() && i < args.size(); ++i)
            {
                frameEnv[func->parameterNames[i]] = args[i];
            }

            execute(func->chunk, frameEnv, componentEnv, fileEnv, globalEnv);
        }

	private:
		std::shared_ptr<Chunk> m_chunk;
		size_t m_ptr;
		std::vector<ScriptValue> m_stack;
        std::shared_ptr<ScriptComponentRef> m_currentSelf = nullptr;

        ScriptValue resolveVariable(const std::string& name, const Environment& frameEnv, const Environment& componentEnv, const Environment& fileEnv, const Environment& globalEnv)
        {
            auto fit = frameEnv.find(name);
            if (fit != frameEnv.end()) return fit->second;

            auto cit = componentEnv.find(name);
            if (cit != componentEnv.end()) return cit->second;

            auto fie = fileEnv.find(name);
            if (fie != fileEnv.end()) return fie->second;

            auto git = globalEnv.find(name);
            if (git != globalEnv.end()) return git->second;

            return ScriptValue();
        }

        void execute(std::shared_ptr<Chunk> chunk, Environment& frameEnv, Environment& componentEnv, Environment& fileEnv, const Environment& globalEnv)
        {
            auto prevChunk = m_chunk;
            size_t prevPtr = m_ptr;

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
                    if (m_chunk->constants[idx].type != ScriptValueType::String)
                    {
                        throw std::runtime_error("VM Error: Load identifier index is not a string string constant.");
                    }
                    std::string name = std::get<std::string>(m_chunk->constants[idx].data);
                    m_stack.push_back(resolveVariable(name, frameEnv, componentEnv, fileEnv, globalEnv));
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

                    ScriptValue val = popStack();
                    if (componentEnv.count(name)) 
                    {
                        componentEnv[name] = val;
                    }
                    else if (fileEnv.count(name))
                    {
                        fileEnv[name] = val;
                    }
                    else 
                    {
                        frameEnv[name] = val;
                    }
                    break;
                }
                case Opcode::Negate:
                {
                    ScriptValue valA = popStack();
                    double a = getNumber(valA, "Negate");
                    m_stack.push_back(ScriptValue(-a));
                    break;
                }
                case Opcode::Add:
                {
                    ScriptValue valB = popStack();
                    ScriptValue valA = popStack();
                    double b = getNumber(valB, "Add");
                    double a = getNumber(valA, "Add");
                    m_stack.push_back(ScriptValue(a + b));
                    break;
                }
                case Opcode::Subtract:
                {
                    ScriptValue valB = popStack();
                    ScriptValue valA = popStack();
                    double b = getNumber(valB, "Subtract");
                    double a = getNumber(valA, "Subtract");
                    m_stack.push_back(ScriptValue(a - b));
                    break;
                }
                case Opcode::Divide:
                {
                    ScriptValue valB = popStack();
                    ScriptValue valA = popStack();
                    double b = getNumber(valB, "Divide");
                    double a = getNumber(valA, "Divide");
                    m_stack.push_back(ScriptValue(a / b));
                    break;
                }
                case Opcode::Modulus:
                {
                    ScriptValue valB = popStack();
                    ScriptValue valA = popStack();
                    double b = getNumber(valB, "Modulus");
                    double a = getNumber(valA, "Modulus");

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
                    ScriptValue valB = popStack();
                    ScriptValue valA = popStack();
                    double b = getNumber(valB, "Multiply");
                    double a = getNumber(valA, "Multiply");
                    m_stack.push_back(ScriptValue(a * b));
                    break;
                }
                case Opcode::Equal:
                {
                    ScriptValue valB = popStack();
                    ScriptValue valA = popStack();
                    double b = getNumber(valB, "Equal");
                    double a = getNumber(valA, "Equal");
                    m_stack.push_back(ScriptValue(a == b));
                    break;
                }
                case Opcode::Greater:
                {
                    ScriptValue valB = popStack();
                    ScriptValue valA = popStack();
                    double b = getNumber(valB, "Greater");
                    double a = getNumber(valA, "Greater");
                    m_stack.push_back(ScriptValue(a > b));
                    break;
                }
                case Opcode::Less:
                {
                    ScriptValue valB = popStack();
                    ScriptValue valA = popStack();
                    double b = getNumber(valB, "Less");
                    double a = getNumber(valA, "Less");
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
                        auto func = std::get<std::shared_ptr<ScriptFunction>>(callee.data);
                        Environment internalFrame;

                        for (size_t i = 0; i < func->parameterNames.size() && i < args.size(); ++i)
                        {
                            internalFrame[func->parameterNames[i]] = args[i];
                        }

                        execute(func->chunk, internalFrame, componentEnv, fileEnv, globalEnv);
                    }
                    else
                    {
                        throw std::runtime_error("VM Error: Property is not callable.");
                    }
                    break;
                }
                case Opcode::LoadSelf:
                {
                    if (!m_currentSelf) 
                    {
                        throw std::runtime_error("VM Error: Attempted to access 'self' outside of an active instance context.");
                    }
                    m_stack.push_back(ScriptValue(m_currentSelf));
                    break;
                }
                case Opcode::GetProperty:
                {
                    uint8_t idx = readByte();
                    std::string propName = std::get<std::string>(m_chunk->constants[idx].data);

                    ScriptValue objValue = popStack();
                    if (objValue.type != ScriptValueType::ComponentRef)
                    {
                        throw std::runtime_error("VM Error: Can only access properties on active component instances.");
                    }

                    auto ref = std::get<std::shared_ptr<ScriptComponentRef>>(objValue.data);
                    if (ref->kind == ScriptComponentRef::Kind::Script)
                    {
                        auto it = ref->scriptProperties->find(propName);
                        if (it != ref->scriptProperties->end())
                        {
                            m_stack.push_back(it->second);
                        }
                        else
                        {
                            m_stack.push_back(ScriptValue());
                        }
                    }
                    else
                    {
                        // C++ Component
                    }
                    break;
                }
                case Opcode::SetProperty:
                {
                    uint8_t idx = readByte();
                    std::string propName = std::get<std::string>(m_chunk->constants[idx].data);

                    ScriptValue objValue = popStack();
                    ScriptValue assignVal = popStack();

                    if (objValue.type != ScriptValueType::ComponentRef) 
                    {
                        throw std::runtime_error("VM Error: Cannot assign property fields on standard primitives.");
                    }

                    auto ref = std::get<std::shared_ptr<ScriptComponentRef>>(objValue.data);
                    if (ref->kind == ScriptComponentRef::Kind::Script) 
                    {
                        (*ref->scriptProperties)[propName] = assignVal;
                    }
                    else 
                    {
                        // C++ Component
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

            m_chunk = prevChunk;
            m_ptr = prevPtr;
        }

        double getNumber(const ScriptValue& val, const std::string& opName)
        {
            if (val.type != ScriptValueType::Number)
            {
                throw std::runtime_error("VM Error: Expected number type for operation " + opName);
            }
            return std::get<double>(val.data);
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