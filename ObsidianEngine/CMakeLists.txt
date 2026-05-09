
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(SLANG_DIR "C:/VulkanSDK/1.4.341.1")
set(GLAD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/glad")
set(GLSLCONV_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/glslconv")

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

file(GLOB_RECURSE SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${GLAD_DIR}/src/*.c"
    "${GLSLCONV_DIR}/*.c"
)

add_library(ObsidianEngine STATIC ${SOURCES})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ObsidianEngine PROPERTY CXX_STANDARD 20)
endif()

target_include_directories(ObsidianEngine 
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ObsidianEngine
    ${SLANG_DIR}/include
    ${GLAD_DIR}/include
    ${GLSLCONV_DIR}

)

target_link_directories(ObsidianEngine PUBLIC
    ${SLANG_DIR}/Lib
)

target_link_libraries(ObsidianEngine
    PUBLIC
    Vulkan::Vulkan
    glfw3
    slang
)