#ifndef __OBSIDIANENGINE_OBSCRIPT_SCRIPT_SYSTEM_HPP__
#define __OBSIDIANENGINE_OBSCRIPT_SCRIPT_SYSTEM_HPP__

#include "ObsidianEngine/ECS/Entity.h"
#include "ObsidianEngine/ECS/System.h"
#include "ObsidianEngine/ECS/Registry.h"

#include "ObsidianEngine/ObScript/ScriptComponent.hpp"
#include "ObsidianEngine/ObScript/Lexer.hpp"
#include "ObsidianEngine/ObScript/Parser.hpp"
#include "ObsidianEngine/ObScript/Compiler.hpp"
#include "ObsidianEngine/ObScript/VirtualMachine.hpp"

#include <iostream>
#include <functional>

namespace ObsidianEngine
{
    ScriptValue nativePrint(const std::vector<ScriptValue>& args)
    {
        for (const auto& arg : args) 
        {
            if (arg.type == ScriptValueType::Number) std::cout << std::get<double>(arg.data);
            else if (arg.type == ScriptValueType::String) std::cout << std::get<std::string>(arg.data);
            else if (arg.type == ScriptValueType::ComponentRef) std::cout << std::get<std::shared_ptr<ScriptComponentRef>>(arg.data)->componentName;
            else if (arg.type == ScriptValueType::Nil) std::cout << "NULL";
        }
        std::cout << std::endl;
        return ScriptValue();
    }

    ScriptValue nativeStart(const std::vector<ScriptValue>& args)
    {
        return ScriptValue();
    }

    ScriptValue nativeUpdate(const std::vector<ScriptValue>& args)
    {
        return ScriptValue();
    }

    using NativeComponentGetter = std::function<void* (Registry&, EntityID)>;
    using NativePropertyGetter = std::function<ScriptValue(void*, const std::string&)>;
    using NativePropertySetter = std::function<void(void*, const std::string&, const ScriptValue&)>;

    struct NativeComponentReflection
    {
        NativeComponentGetter getter;
        std::unordered_map<std::string, NativePropertyGetter> propertyGetters;
        std::unordered_map<std::string, NativePropertySetter> propertySetters;
    };

	class ScriptSystem : public ISystem
	{
	public:
        ScriptSystem()
        {
            m_globalEnvironment["Start"] = ScriptValue(std::make_shared<ScriptNativeFunction>(nativeStart, "Start"));
            m_globalEnvironment["Update"] = ScriptValue(std::make_shared<ScriptNativeFunction>(nativeUpdate, "Update"));
            m_globalEnvironment["log"] = ScriptValue(std::make_shared<ScriptNativeFunction>(nativePrint, "log"));

            registerNativeComponents();
        }

        void update(Registry& registry, float dt) override
        {
            auto view = registry.view<ScriptComponent>();
            ScriptValue dtArg(static_cast<double>(dt));
            std::vector<ScriptValue> args = { dtArg };

            for (EntityID entity : view)
            {
                auto& scriptComp = registry.getComponent<ScriptComponent>(entity);

                for (auto& script : scriptComp.scripts)
                {
                    if (!script.isInitialized) continue;

                    injectEntityBindings(script.properties, registry, entity, scriptComp);

                    VirtualMachine vm;

                    auto selfRef = std::make_shared<ScriptComponentRef>();
                    selfRef->kind = ScriptComponentRef::Kind::Script;
                    selfRef->componentName = script.scriptTypeName;
                    selfRef->owner = entity;
                    selfRef->scriptProperties = &script.properties;

                    try
                    {
                        vm.call("Update", args, script.properties, script.fileEnvironment, m_globalEnvironment, selfRef);
                    }
                    catch (const std::runtime_error& error)
                    {
                        std::cerr << "Runtime Error in Script " << script.scriptTypeName << " during Update: " << error.what() << std::endl;
                    }
                }
            }
        }

        const std::unordered_map<std::string, ScriptValue>& getGlobalEnvironment() const
        {
            return m_globalEnvironment;
        }

	private:
        std::unordered_map<std::string, ScriptValue> m_globalEnvironment;
        std::unordered_map<std::string, NativeComponentReflection> m_nativeReflection;

        void registerNativeComponents()
        {

        }

        void injectEntityBindings
        (
            std::unordered_map<std::string, ScriptValue>& localEnv,
            Registry& registry,
            EntityID entity,
            ScriptComponent& scriptComponent)
        {
            localEnv["GetComponent"] = ScriptValue(std::make_shared<ScriptNativeFunction>(
                [&registry, entity, &scriptComponent, Atlantic = this](const std::vector<ScriptValue>& args) -> ScriptValue
                {
                    if (args.empty() || args[0].type != ScriptValueType::String) 
                    {
                        throw std::runtime_error("GetComponent requires a string component name.");
                    }
                    std::string compName = std::get<std::string>(args[0].data);

                    for (auto& targetScript : scriptComponent.scripts)
                    {
                        if (targetScript.scriptTypeName == compName)
                        {
                            auto ref = std::make_shared<ScriptComponentRef>();
                            ref->kind = ScriptComponentRef::Kind::Script;
                            ref->componentName = compName;
                            ref->owner = entity;
                            ref->scriptProperties = &targetScript.properties;
                            return ScriptValue(ref);
                        }
                    }

                    auto it = Atlantic->m_nativeReflection.find(compName);
                    if (it != Atlantic->m_nativeReflection.end())
                    {
                        void* nativePtr = it->second.getter(registry, entity);
                        if (nativePtr)
                        {
                            auto ref = std::make_shared<ScriptComponentRef>();
                            ref->kind = ScriptComponentRef::Kind::Native;
                            ref->componentName = compName;
                            ref->owner = entity;
                            ref->nativeComponentPtr = nativePtr;
                            return ScriptValue(ref);
                        }
                    }

                    return ScriptValue();
                }, "GetComponent"));

            localEnv["GetProperty"] = ScriptValue(std::make_shared<ScriptNativeFunction>(
                [this](const std::vector<ScriptValue>& args) -> ScriptValue
                {
                    if (args.size() < 2 || args[0].type != ScriptValueType::ComponentRef || args[1].type != ScriptValueType::String) 
                    {
                        throw std::runtime_error("GetProperty usage: GetProperty handles stringKey");
                    }

                    auto ref = std::get<std::shared_ptr<ScriptComponentRef>>(args[0].data);
                    std::string propName = std::get<std::string>(args[1].data);

                    if (ref->kind == ScriptComponentRef::Kind::Script)
                    {
                        auto propIt = ref->scriptProperties->find(propName);
                        return (propIt != ref->scriptProperties->end()) ? propIt->second : ScriptValue();
                    }
                    else
                    {
                        auto it = m_nativeReflection.find(ref->componentName);
                        if (it != m_nativeReflection.end() && it->second.propertyGetters.count(propName)) {
                            return it->second.propertyGetters[propName](ref->nativeComponentPtr, propName);
                        }
                    }
                    return ScriptValue();
                }, "GetProperty"));

            localEnv["SetProperty"] = ScriptValue(std::make_shared<ScriptNativeFunction>(
                [this](const std::vector<ScriptValue>& args) -> ScriptValue
                {
                    if (args.size() < 3 || args[0].type != ScriptValueType::ComponentRef || args[1].type != ScriptValueType::String) 
                    {
                        throw std::runtime_error("SetProperty usage: SetProperty handles stringKey value");
                    }

                    auto ref = std::get<std::shared_ptr<ScriptComponentRef>>(args[0].data);
                    std::string propName = std::get<std::string>(args[1].data);
                    ScriptValue valueToSet = args[2];

                    if (ref->kind == ScriptComponentRef::Kind::Script)
                    {
                        (*ref->scriptProperties)[propName] = valueToSet;
                    }
                    else
                    {
                        auto it = m_nativeReflection.find(ref->componentName);
                        if (it != m_nativeReflection.end() && it->second.propertySetters.count(propName)) {
                            it->second.propertySetters[propName](ref->nativeComponentPtr, propName, valueToSet);
                        }
                    }
                    return ScriptValue();
                }, "SetProperty"));
        }
	};

    class ScriptInitializationSystem : public ISystem
    {
    public:
        ScriptInitializationSystem(const std::unordered_map<std::string, ScriptValue>& globalEnv) : m_globalEnvironment(globalEnv) {}

        void update(Registry& registry, float dt) override
        {
            auto view = registry.view<ScriptComponent>();

            for (EntityID entity : view)
            {
                auto& scriptComp = registry.getComponent<ScriptComponent>(entity);

                for (auto& script : scriptComp.scripts)
                {
                    if (script.isInitialized) continue;
                    InitializeScript(script, entity);
                }
            }
        }

    private:
        const std::unordered_map<std::string, ScriptValue>& m_globalEnvironment;

        void InitializeScript(Script& script, EntityID entity)
        {
            try
            {
                Lexer lexer;
                auto tokens = lexer.parse(script.rawSourceCode);

                Parser parser(tokens);
                auto astRoot = parser.parse();

                Compiler compiler;
                std::shared_ptr<Chunk> compiledChunk = compiler.compile(astRoot.get());

                VirtualMachine initVM;
                initVM.run(compiledChunk, script.fileEnvironment, m_globalEnvironment);

                auto it = script.fileEnvironment.find(script.scriptTypeName);
                if (it != script.fileEnvironment.end() && it->second.type == ScriptValueType::ComponentDef)
                {
                    auto compDef = std::get<std::shared_ptr<ScriptComponentDef>>(it->second.data);

                    script.fileEnvironment.erase(script.scriptTypeName);

                    script.properties.clear();
                    for (const auto& [name, dummyVal] : compDef->defaultProperties)
                    {
                        script.properties[name] = ScriptValue();
                    }

                    VirtualMachine allocatorVM;
                    VirtualMachine::Environment temporaryFrame;
                    allocatorVM.run(compDef->initChunk, temporaryFrame, script.properties, m_globalEnvironment);

                    for (const auto& [methodName, methodBody] : compDef->methods)
                    {
                        script.properties[methodName] = methodBody;
                    }

                    script.properties["entity"] = ScriptValue(static_cast<double>(entity));
                }
                else
                {
                    throw std::runtime_error("Could not find component definition matching: " + script.scriptTypeName);
                }

                auto selfRef = std::make_shared<ScriptComponentRef>();
                selfRef->kind = ScriptComponentRef::Kind::Script;
                selfRef->componentName = script.scriptTypeName;
                selfRef->owner = entity;
                selfRef->scriptProperties = &script.properties;

                initVM.call("Start", {}, script.properties, script.fileEnvironment, m_globalEnvironment, selfRef);

                script.isInitialized = true;
                script.rawSourceCode.clear();
            }
            catch (const std::runtime_error& error)
            {
                std::cerr << "Initialization Failed: " << error.what() << std::endl;
                script.isInitialized = true;
            }
        }
    };
}

#endif