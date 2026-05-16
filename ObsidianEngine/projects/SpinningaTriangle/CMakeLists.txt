file(GLOB_RECURSE PROJECT_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_executable(SpinningTriangle ${PROJECT_SOURCES})

target_link_libraries(SpinningTriangle
    PRIVATE ObsidianEngine
)

target_include_directories(SpinningTriangle PRIVATE
    ${CMAKE_SOURCE_DIR}/ObsidianEngine/include
)