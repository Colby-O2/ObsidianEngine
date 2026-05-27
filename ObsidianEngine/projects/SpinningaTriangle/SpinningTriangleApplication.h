#ifndef SIERPINSKI_TRIANGLE_APPLICATION_H_
#define SIERPINSKI_TRIANGLE_APPLICATION_H_

#include "ObsidianEngine/Application.h"
#include "ObsidianEngine/Vertex.h"
#include "ObsidianEngine/MeshComponent.h"
#include "ObsidianEngine/ECS/Entity.h"

#include "ObsidianEngine/ObScript/ScriptComponent.hpp"
#include "ObsidianEngine/ObScript/ScriptSystem.hpp"
#include "ObsidianEngine/ObScript/Lexer.hpp"
#include "ObsidianEngine/ObScript/Parser.hpp"
#include "ObsidianEngine/ObScript/Compiler.hpp"
#include "ObsidianEngine/ObScript/VirtualMachine.hpp"



#include <vector>
#include <unordered_map>
#include <string>

using namespace ObsidianEngine;

class SpinningTriangleApplication : public ObsidianEngine::Application
{
protected:
    void onStartup() override 
    {
        std::cout << "Yo!" << std::endl;

        const std::vector<Vertex> vertices = {
            {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
            {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
        };

        const std::vector<uint16_t> indices = {
            0, 1, 2, 2, 3, 0
        };

        EntityID player = m_activeScene->createEntity();

        //ScriptComponent healthComp;
        //healthComp.scriptTypeName = "HealthComponent";
        //healthComp.properties["hp"] = ScriptValue(100.0);
        //healthComp.properties["maxHp"] = ScriptValue(100.0);
        //healthComp.properties["isAlive"] = ScriptValue(true);

        //m_activeScene->getRegistry().addComponent<ScriptComponent>(player, healthComp);

        //m_activeScene->addSystem<ScriptSystem>();

        //std::string src = R"(
        //        let a = 20 * 10 + -1 + (20 / 3)
        //        let b = 1.2
        //        let c = "Yo"
        //                        
        //        let isDeadPrevious = 
        //            if a > 5 then 
        //                return true
        //            else 
        //                return false
        //            end   
        //        end 

        //        let isDeadNow ->
        //            if a > 5 then 
        //                return true
        //            elif a == 2 then
        //            elif a == 3 then
        //            else 
        //                return false
        //            end   
        //        end 

        //        let Update dt -> 

        //        end
        //)";

        std::string srcHealth = R"(
                let a = 20 * 10 + -1 + (20 / 3)

                component HealthComponent
                    let maxHp = 100.0
                    let hp = maxHp      

                    fn Start()
                        log("a = ", a)
                    end

                    fn Update(dt)
                        self.hp -= 10.0 * dt
                    end
                end
        )";

        std::string srcFPS = R"(

                let timer = 0.0
                let interval = 1.0 

                fn Update(dt)
                    let timer = timer + dt
                    if timer > interval then 
                        let fps = 1 / dt
                        log "FPS: " fps
                        let timer = 0

                        let healthComp = GetComponent("HealthComponent")
                        let currentHp = GetProperty(healthComp, "hp")
                        log("FPS System checked health component. HP: ", currentHp)

                        if currentHp < 0 then
                            SetProperty(healthComp, "hp", 100)
                            log("FPS System set HP back to 100")
                        end
                   end 
                end
        )";

        std::string newSrc = R"yo(
            # a is not assessable with a entity.GetComponent().a as its not defined within component scope  
            let a = 2 * -2 + (3/2 * 3) 

            struct PlayerStats
                let hp = 100
                let maxHp = 100
    
                fn PlayerStats()
                    self.hp = self.maxHp
                end

                fn print()
                    log(self.hp, "/", self.maxHp)
                end
            end

            fn foo()
               log("Helper Func")
            end

            component FPSComponent
                # let only assessable from within this component
                let timer = 0.0
                # global are global variable that can be assessed anywhere 
                let interval = 1.0

                fn foo()
                    log("This function can be called in other scripts via entity.GetComponent().foo()")
                end 

                fn Start()
                    self.timer = 0.0
                end

                # function are local by default 
                fn Update(dt)
                    #log("At Update ", self.timer, " ", self.interval)
                    self.timer += dt
        
                    #foo()

                    if self.timer > self.interval then 
	                    let fps = 1.0 / dt
	                    log("FPS: ", fps)
                        timer = 0.0

                        let healthComp = self.GetComponent("HealthComponent")
                        let currentHp = healthComp.hp
                        log("FPS System on Entity ", self.entity, " checked health component. HP: ", currentHp)

                        if currentHp < 0.0 then
                              healthComp.hp = 100.0
                              log("FPS System set HP back to 100")
	                    #elif self.interval == 2 then
	                    #      log("Foo")
                        #else
                        #      log("Foo2") 
                        end
                    end
                end
            end
        )yo";

        Script healthComp;
        healthComp.scriptTypeName = "HealthComponent";
        healthComp.rawSourceCode = srcHealth;

        Script fpsComp;
        fpsComp.scriptTypeName = "FPSComponent";
        fpsComp.rawSourceCode = newSrc;

        ScriptComponent playerScripts{ fpsComp, healthComp };

        m_activeScene->getRegistry().addComponent<ScriptComponent>(player, playerScripts);

        EntityID player2 = m_activeScene->createEntity();
        m_activeScene->getRegistry().addComponent<ScriptComponent>(player2, playerScripts);

        m_activeScene->addSystem<ScriptSystem>();
        m_activeScene->addSystem<ScriptInitializationSystem>(m_activeScene->getSystem<ScriptSystem>()->getGlobalEnvironment());

        //Lexer tolkenizer{};
        //auto tokens = tolkenizer.parse(src);

        //for (auto& t : tokens)
        //{
        //    //std::cout << t << std::endl;
        //}

        //Parser parser(tokens);
        //std::unique_ptr<ASTNode> astRoot = parser.parse();

        //if (astRoot) 
        //{
        //    astRoot->print();
        //}

        //Compiler compiler{};

        //auto bytecode = compiler.compile(astRoot.get());
        //std::cout << bytecode->bytecode.size() << std::endl;

        //VirtualMachine vm{};

        //std::unordered_map<std::string, ScriptValue> environment;
        //auto printFn = std::make_shared<ScriptNativeFunction>(nativePrint, "print");
        //environment["print"] = ScriptValue(printFn);

        //vm.run(bytecode, environment);

        //vm.call("Update", {0.0001}, environment);

        EntityID entity = m_activeScene->createEntity();
        m_activeScene->getRegistry().addComponent<MeshComponent>(entity, vertices, indices, -1, false);
    }
};

#endif