set(_glfw3_HEADER_SEARCH_DIRS
    "/usr/include"
    "/usr/local/include"
    "${CMAKE_SOURCE_DIR}/includes"
    "C:/dev/glfw/include"
)

set(_glfw3_LIB_SEARCH_DIRS
    "/usr/lib"
    "/usr/local/lib"
    "${CMAKE_SOURCE_DIR}/lib"
    "C:/dev/glfw/lib-vc2022"
)

set(_glfw3_ENV_ROOT $ENV{GLFW3_ROOT})
if(NOT GLFW3_ROOT AND _glfw3_ENV_ROOT)
    set(GLFW3_ROOT ${_glfw3_ENV_ROOT})
endif()

if(GLFW3_ROOT)
    list(INSERT _glfw3_HEADER_SEARCH_DIRS 0 "${GLFW3_ROOT}/include")
    list(INSERT _glfw3_LIB_SEARCH_DIRS 0 "${GLFW3_ROOT}/lib")
endif()

find_path(GLFW3_INCLUDE_DIR
    NAMES GLFW/glfw3.h
    PATHS ${_glfw3_HEADER_SEARCH_DIRS}
)

find_library(GLFW3_LIBRARY
    NAMES glfw3 glfw
    PATHS ${_glfw3_LIB_SEARCH_DIRS}
)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(glfw3 DEFAULT_MSG
    GLFW3_LIBRARY GLFW3_INCLUDE_DIR
)

if(GLFW3_FOUND AND NOT TARGET glfw3)
    message(STATUS "GLFW Found!")

    add_library(glfw3 UNKNOWN IMPORTED)

    set_target_properties(glfw3 PROPERTIES
        IMPORTED_LOCATION "${GLFW3_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${GLFW3_INCLUDE_DIR}"
    )
else()
    message(STATUS "GLFW Not Found!")
endif()