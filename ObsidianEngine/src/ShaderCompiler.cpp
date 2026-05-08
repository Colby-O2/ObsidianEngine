#include "ShaderCompiler.h"

namespace ObsidianEngine
{
    ShaderCompiler::ShaderCompiler() 
    {
        if (SLANG_FAILED(createGlobalSession(m_globalSession.writeRef()))) 
        {
            throw std::runtime_error("Failed to create Slang global session");
        }
    }

    void ShaderCompiler::configureTarget(slang::TargetDesc& target, ShaderBackend backend)
    {
        if (backend == ShaderBackend::SPIRV)
        {
            target.format = SLANG_SPIRV;
            target.profile = m_globalSession->findProfile("spirv_1_4");
        }
        else if (backend == ShaderBackend::WGSL)
        {
            target.format = SLANG_WGSL;
            target.profile = m_globalSession->findProfile("wgsl_1_0");
        }
        else if (backend == ShaderBackend::GLSL)
        {
            target.format = SLANG_GLSL;
            target.profile = m_globalSession->findProfile("glsl_450");
        }
    }

    Slang::ComPtr<slang::IBlob> ShaderCompiler::compile(const std::string& filePath, ShaderBackend backend) 
    {
        slang::SessionDesc sessionDesc = {};
        slang::TargetDesc targetDesc = {};
        configureTarget(targetDesc, backend);

        sessionDesc.targets = &targetDesc;
        sessionDesc.targetCount = 1;

        slang::CompilerOptionEntry options[2];
        uint32_t optionCount = 0;

        if (backend == ShaderBackend::SPIRV)
        {
            options[0] = { slang::CompilerOptionName::EmitSpirvDirectly, { slang::CompilerOptionValueKind::Int, 1 } };
            options[1] = { slang::CompilerOptionName::VulkanUseEntryPointName, { slang::CompilerOptionValueKind::Int, 1 } };
            optionCount = 2;
        }

        sessionDesc.compilerOptionEntries = options;
        sessionDesc.compilerOptionEntryCount = optionCount;

        Slang::ComPtr<slang::ISession> session;
        m_globalSession->createSession(sessionDesc, session.writeRef());

        auto module = session->loadModule(filePath.c_str());
        if (!module) throw std::runtime_error("Failed to load shader module: " + filePath);

        Slang::ComPtr<slang::IEntryPoint> vertEntry, fragEntry;
        module->findEntryPointByName("vertMain", vertEntry.writeRef());
        module->findEntryPointByName("fragMain", fragEntry.writeRef());

        slang::IComponentType* components[] = { module, vertEntry, fragEntry };
        Slang::ComPtr<slang::IComponentType> program;
        session->createCompositeComponentType(components, 3, program.writeRef());

        Slang::ComPtr<slang::IComponentType> linkedProgram;
        program->link(linkedProgram.writeRef());

        Slang::ComPtr<slang::IBlob> blob;
        linkedProgram->getTargetCode(0, blob.writeRef());

        return blob;
    }

    void ShaderCompiler::saveShaderToFile(const std::string& fileName, Slang::ComPtr<slang::IBlob> blob) {
        if (!blob) {
            std::cerr << "Cannot save empty shader blob!" << std::endl;
            return;
        }

        std::ofstream outFile(fileName, std::ios::binary);

        if (!outFile) {
            throw std::runtime_error("Failed to open file for writing: " + fileName);
        }

        outFile.write(reinterpret_cast<const char*>(blob->getBufferPointer()),
            blob->getBufferSize());

        outFile.close();

        std::cout << "Shader saved successfully to: " << fileName
            << " (" << blob->getBufferSize() << " bytes)" << std::endl;
    }
}