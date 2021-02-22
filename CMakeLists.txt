cmake_minimum_required (VERSION 3.8)
cmake_policy(SET CMP0091 NEW)
project ("FractalsApp")

if(MSVC)
	add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/MP>)
endif()

add_subdirectory ("Fractals")