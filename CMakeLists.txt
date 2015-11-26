cmake_minimum_required(VERSION 2.8)
project(how_to_variant)

if(UNIX)
	add_definitions("-std=c++1y")

	add_definitions("-Wall -Wextra -Wconversion -Wvla")

	#warn when using undefined macros
	add_definitions("-Wundef")

	add_definitions("-Werror")

	add_definitions("-pedantic")
endif()

if(MSVC)
	set(Boost_USE_STATIC_LIBS ON) 
endif()
if(BOOST_ROOT)
	set(Boost_NO_SYSTEM_PATHS ON)
endif()
find_package(Boost REQUIRED COMPONENTS unit_test_framework system)
if(UNIX)
	add_definitions("-DBOOST_TEST_DYN_LINK")
	add_definitions("-DBOOST_USE_VALGRIND")
endif()
include_directories(SYSTEM ${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

include_directories(".")

add_executable(how_to_variant optional.hpp test_optional.cpp why_optional.cpp variant.hpp variant.cpp)
target_link_libraries(how_to_variant ${Boost_LIBRARIES})

if(WIN32)
	set(CLANG_FORMAT "C:/Program Files/LLVM/bin/clang-format.exe" CACHE TYPE PATH)
else()
	set(CLANG_FORMAT "clang-format-3.7" CACHE TYPE PATH)
endif()
add_custom_target(clang-format COMMAND ${CLANG_FORMAT} -i ${formatted} WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
