# CMakeList.txt : 顶级 CMake 项目文件，执行全局配置
# 和此处的包含子项目。
#
cmake_minimum_required (VERSION 3.8)

project ("Quadtree")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")

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
	${SCR_LIST}
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

find_package( OpenCV REQUIRED )

target_link_libraries( ${SHARED_LIBRARY} ${OpenCV_LIBS} )
target_link_libraries( ${STATIC_LIBRARY} ${OpenCV_LIBS} )