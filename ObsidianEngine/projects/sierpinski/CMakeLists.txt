file(GLOB_RECURSE PROJECT_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_executable(Sierpinski ${PROJECT_SOURCES})

target_link_libraries(Sierpinski
    PRIVATE ObsidianEngine
)

target_include_directories(Sierpinski PRIVATE
    ${CMAKE_SOURCE_DIR}/ObsidianEngine/include
)