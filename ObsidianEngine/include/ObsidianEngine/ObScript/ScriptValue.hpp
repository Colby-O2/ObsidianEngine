#ifndef __OBSIDIANENGINE_OBSCRIPT_SCRIPT_VALUE_HPP__
#define __OBSIDIANENGINE_OBSCRIPT_SCRIPT_VALUE_HPP__

#include "ObsidianEngine/Math/Math.hpp"

#include <variant>
#include <string>
#include <vector>
#include <functional>

namespace ObsidianEngine
{
	struct Chunk;
	struct ScriptValue;

	struct ScriptComponentRef
	{
		enum class Kind {Script, Native } kind;

		std::string componentName;
		EntityID owner;

		std::unordered_map<std::string, ScriptValue>* scriptProperties = nullptr;

		void* nativeComponentPtr = nullptr;
	};

	struct ScriptFunction
	{
		std::string name;
		std::vector<std::string> parameterNames;
		std::shared_ptr<Chunk> chunk;
	};

	struct ScriptNativeFunction
	{
		std::function<ScriptValue(const std::vector<ScriptValue>&)> function;
		std::string name;

		ScriptNativeFunction(std::function<ScriptValue(const std::vector<ScriptValue>&)> func, const std::string& name): function(func), name(name) {}
	};

	enum class ScriptValueType 
	{ 
		Nil,
		Boolean,
		Number,
		String,
		Function,
		NativeFunction,
		ComponentRef,
		ComponentDef
	};

	struct ScriptComponentDef
	{
		std::string name;
		std::unordered_map<std::string, ScriptValue> methods;
		std::unordered_map<std::string, ScriptValue> defaultProperties;
		std::shared_ptr<Chunk> initChunk;
	};

	struct ScriptValue
	{
		ScriptValueType type = ScriptValueType::Nil;

		std::variant
		<
			std::monostate, 
			bool, 
			double, 
			std::string, 
			std::shared_ptr<ScriptFunction>, 
			std::shared_ptr<ScriptNativeFunction>, 
			std::shared_ptr<ScriptComponentRef>, 
			std::shared_ptr<ScriptComponentDef>
		> data;

		ScriptValue() : type(ScriptValueType::Nil), data(std::monostate{}) {}
		ScriptValue(bool b) : type(ScriptValueType::Boolean), data(b) {}
		ScriptValue(double n) : type(ScriptValueType::Number), data(n) {}
		ScriptValue(std::string s) : type(ScriptValueType::String), data(s) {}
		ScriptValue(std::shared_ptr<ScriptFunction> f) : type(ScriptValueType::Function), data(f) {}
		ScriptValue(std::shared_ptr<ScriptNativeFunction> f) : type(ScriptValueType::NativeFunction), data(f) {}
		ScriptValue(std::shared_ptr<ScriptComponentRef> c) : type(ScriptValueType::ComponentRef), data(c) {}
		ScriptValue(std::shared_ptr<ScriptComponentDef> c) : type(ScriptValueType::ComponentDef), data(c) {}
	};
}

#endif