cmake_minimum_required(VERSION 2.8)

set(PROJECT_NAME aerotap_PalmTrack)

project(${PROJECT_NAME} LANGUAGES C CXX)

add_definitions(-std=c++03 -D_GLIBCXX_USE_CXX11_ABI=0)

set(AEROTAP_INCLUDE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../include)

if(UNIX)
	set(OPENGL_LIBS glut GL)
	IF (CMAKE_SYSTEM_PROCESSOR MATCHES "^aarch64")
		set(PLATFORM_DIR linux_aarch64)
		set(CMAKE_CXX_COMPILER "/usr/bin/g++")
	ELSEIF (CMAKE_SYSTEM_PROCESSOR MATCHES "^arm")
		set(PLATFORM_DIR linux_arm)
	ELSEIF(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "amd64")
		set(PLATFORM_DIR linux64)
	ENDIF()
elseif(WIN32)
	IF(CMAKE_SIZEOF_VOID_P EQUAL 4)
		set(PLATFORM_DIR win32)
	ELSEIF(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(PLATFORM_DIR win64)
	ENDIF()
	set(OPENGL_LIBS glut32 glu32 opengl32)
endif()

set(AEROTAP_LIBRARY_PATH ${CMAKE_CURRENT_LIST_DIR}/../../lib/${PLATFORM_DIR}) 

include_directories(
	.
	${AEROTAP_INCLUDE_PATH}
)

link_directories(${AEROTAP_LIBRARY_PATH})

set(SOURCES
    src/Sample.cpp
    src/main.cpp)
    
add_executable(${PROJECT_NAME} ${SOURCES})

set(LIBS ${OPENGL_LIBS} aeroTAP-sdk jpeg ) 


set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")


target_link_libraries(${PROJECT_NAME} ${LIBS})

