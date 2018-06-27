# CMakeList.txt : 顶级 CMake 项目文件，执行全局配置
# 和此处的包含子项目。
#
cmake_minimum_required (VERSION 3.8)

project ("Quadtree")
set(PROJECT_NAME_LOWER "quadtree")


# Default configuration values
set(DEFAULT_BUILD_TYPE "Release")
set(DEFAULT_ARCH "x86-64")
set(DEFAULT_HEADER_INSTALL_DIRECTORY "include")
set(DEFAULT_LIBRARY_INSTALL_DIRECTORY "lib")

# Detect platform
string(COMPARE EQUAL "Linux"   ${CMAKE_SYSTEM_NAME} LINUX)
string(COMPARE EQUAL "Windows" ${CMAKE_SYSTEM_NAME} WINDOWS)
string(COMPARE EQUAL "Darwin"  ${CMAKE_SYSTEM_NAME} OS_X)
if(LINUX)
    set(OS_STRING "linux")
elseif(WINDOWS)
    set(OS_STRING "windows")
elseif(OS_X)
    set(OS_STRING "osx")
else()
    set(OS_STRING "Unknown")
endif()

# Detect compiler
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CLANG 1)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    set(CLANG 1)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(GCC 1)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    set(INTEL 1)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(MSVC 1)
endif()

# Set build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE ${DEFAULT_BUILD_TYPE})
endif()
string(TOUPPER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE)

# Set architecture to default if not set
if(NOT ARCH)
    set(ARCH ${DEFAULT_ARCH})
endif()

# Set bitness
if(${ARCH} STREQUAL "x86-64")
    set(BITNESS "64")
    set(BITNESS_FLAG "-m${BITNESS}")
elseif(${ARCH} STREQUAL "x86")
    set(BITNESS "32")
    set(BITNESS_FLAG "-m${BITNESS}")
elseif(${ARCH} STREQUAL "armv8")
    set(BITNESS "64")
    set(BITNESS_FLAG "")
else()
    set(BITNESS "32")
    set(ARCH "x86")
    set(BITNESS_FLAG "-m${BITNESS}")
    message("Unknown architecture selected, defaulting to x86")
endif()

if(WINDOWS AND GCC)
	# MinGW (GCC on Windows) version 4.8.1 has a bug where _hypot is not found
	# when using any level of optimization. Defining __NO_INLINE__ fixes the bug.
	# The bug probably exists on other versions of MinGW as well, but that has not been verified by us.
    if(CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL "4.8.1")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -D__NO_INLINE__")
    endif()
    if(CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL "4.9.2")
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
		# From the GCC man pages:
		# If you use multiple -O options, with or without level numbers, the last such option is the one that is effective.
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -finline-functions")
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -funswitch-loops")
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fpredictive-commoning")
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fgcse-after-reload")
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -ftree-loop-vectorize")
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -ftree-loop-distribute-patterns")
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -ftree-slp-vectorize")
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fvect-cost-model")
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -ftree-partial-pre")
		# -fipa-cp-clone causes a SegFault in Eigen when enabled with MinGW i686 4.9.2 with dwarf exception model
		#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fipa-cp-clone")
	endif()
endif()

if(MSVC)
    # User cannot specify bitness with MSVC, so set it to whatever the generator is.
    string(TOLOWER ${CMAKE_GENERATOR} GENERATOR)
    if(GENERATOR MATCHES ".*win64.*")
        set(BITNESS "64")
    else()
        set(BITNESS "32")
    endif()
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ox")

elseif(GCC OR CLANG)
    # Treat warning return-type as error to avoid undefined behaviour
    # when a non-void function does not return a value.
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${BITNESS_FLAG} -std=c++11 -Werror=return-type")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wno-long-long")

elseif(INTEL)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${BITNESS_FLAG} -std=c++11")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
endif()


find_package( OpenCV REQUIRED )

# Header directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/catch)

set(INCLUDE_LIST
	include/quadtree.h
)

set(SRC_LIST
	src/quadtree.cpp
)

set(TEST_SRC_LIST
	${SRC_LIST}
	test/main.cpp
	test/quadtree_test.cpp
)

set(SHARED_LIBRARY ${PROJECT_NAME_LOWER}-${VERSION})
set(STATIC_LIBRARY ${PROJECT_NAME_LOWER}-static-${VERSION})
set(TEST "${PROJECT_NAME_LOWER}-test")

add_library(${SHARED_LIBRARY} SHARED ${SRC_LIST})
add_library(${STATIC_LIBRARY} STATIC ${SRC_LIST})

add_executable(${TEST} ${TEST_SRC_LIST})
target_link_libraries(${TEST} ${STATIC_LIBRARY})
target_link_libraries( ${TEST} ${OpenCV_LIBS} )


target_link_libraries( ${SHARED_LIBRARY} ${OpenCV_LIBS} )
target_link_libraries( ${STATIC_LIBRARY} ${OpenCV_LIBS} )