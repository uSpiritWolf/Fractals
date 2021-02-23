cmake_minimum_required (VERSION 3.8)
cmake_policy(SET CMP0091 NEW)
project("Fractals")

find_package( OpenGL REQUIRED )

set(GLEW_USE_STATIC_LIBS ON)
find_package( GLEW REQUIRED )

find_package( imgui REQUIRED )

add_compile_definitions(UNICODE)

if(MSVC)
	add_compile_definitions(WIN32_LEAN_AND_MEAN _CRT_SECURE_NO_WARNINGS)
endif()

include_directories(${PROJECT_SOURCE_DIR})

file(GLOB_RECURSE SRCS ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE HDRS ${PROJECT_SOURCE_DIR}/*.h)

add_executable (Fractals ${SRCS} ${HDRS})

if(MSVC)
	set_property(TARGET Fractals PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

set_property(TARGET Fractals PROPERTY CXX_STANDARD 20)

target_link_libraries(Fractals OpenGL::GL OpenGL::GLU GLEW::glew_s imgui::imgui)