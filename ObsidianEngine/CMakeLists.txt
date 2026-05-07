
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

file(GLOB_RECURSE SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
)

add_executable(ObsidianEngine ${SOURCES} "include/Window.h" "src/Window.cpp" "src/Application.cpp" "include/VulkanPipeline.h" "include/Vector.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ObsidianEngine PROPERTY CXX_STANDARD 20)
endif()

set(SLANG_DIR "C:/VulkanSDK/1.4.341.1")

target_include_directories(ObsidianEngine PUBLIC
    ${SLANG_DIR}/Include
)

target_link_directories(ObsidianEngine PRIVATE
    ${SLANG_DIR}/Lib
)

target_link_libraries(ObsidianEngine
    PRIVATE
    Vulkan::Vulkan
    glfw3
    slang
)