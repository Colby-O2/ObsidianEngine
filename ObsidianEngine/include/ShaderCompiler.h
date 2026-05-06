#ifndef SHADER_COMPILER_H_
#define SHADER_COMPILER_H_

#include <slang/slang.h>
#include <slang/slang-com-ptr.h>

#include <string>
#include <vector>
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <filesystem>

namespace ObsidianEngine 
{
    enum class ShaderBackend {
        SPIRV,
        GLSL,
        WGSL
    };

    class ShaderCompiler {
    public:
        ShaderCompiler();
        ~ShaderCompiler() = default;

        Slang::ComPtr<slang::IBlob> compile(const std::string& filePath, ShaderBackend backend);
        void saveShaderToFile(const std::string& fileName, Slang::ComPtr<slang::IBlob> blob);

    private:
        Slang::ComPtr<slang::IGlobalSession> m_globalSession;
        void configureTarget(slang::TargetDesc& target, ShaderBackend backend);
    };
}
#endif