file(GLOB_RECURSE TEST_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(TESTS ${TEST_SOURCES})

target_include_directories(TESTS PRIVATE
    ${CMAKE_SOURCE_DIR}/ObsidianEngine/include
    ${CMAKE_SOURCE_DIR}/ObsidianEngine/external
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(TESTS PRIVATE ObsidianEngine)