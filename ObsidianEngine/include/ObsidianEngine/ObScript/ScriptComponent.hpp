#ifndef __OBSIDIANENGINE_OBSCRIPT_SCRIPT_COMPONENT_HPP__
#define __OBSIDIANENGINE_OBSCRIPT_SCRIPT_COMPONENT_HPP__

#include "ObsidianEngine/ObScript/ScriptValue.hpp"

#include <unordered_map>
#include <string>
#include <vector>

namespace ObsidianEngine
{

	struct Script
	{
		std::string scriptTypeName;
		std::unordered_map<std::string, ScriptValue> properties;
		std::string sourceScriptPath;

		bool isInitialized = false;
		std::string rawSourceCode;
	};

	struct ScriptComponent
	{
		std::vector<Script> scripts;

		ScriptComponent() = default;

		ScriptComponent(std::initializer_list<Script> list) : scripts(list) {}
	};
}

#endif