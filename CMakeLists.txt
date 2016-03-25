cmake_minimum_required(VERSION 2.6)
project(IMAGEPROCESSING)  

message(STATUS "Source DIR:" ${IMAGEPROCESSING_SOURCE_DIR})

# Base compile define.	
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -lstdc++ -lpthread -g")
if(APPLE)
	if(${APPLE_TYPE} STREQUAL "mac_x64")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -arch x86_64 -g")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -arch x86_64 -g")
	elseif(${APPLE_TYPE} STREQUAL "ios_emu")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -arch i386 -g")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -arch i386 -g")
	endif()
endif()

# Binary output path and binary name.
set(EXECUTABLE_OUTPUT_PATH ${IMAGEPROCESSING_SOURCE_DIR}/build)
	
# GSDK Unittest compile define.	
set(CMAKE_CXX_FLAGS -fPIC)

# Include path and link path.
include_directories(/opt/Qt/5.3/gcc/include)
# Source files.
set(SOURCE_ALL_LIST
	src/getopt.c 
	src/ImageProcess.cpp  
	src/Main.cpp 
	src/ParseParam.cpp 
	src/QtSnapshot.cpp 
	src/stdafx.cpp)
set(SOURCE_HEADER_LIST 
	src/stdafx.h
	src/QtSnapshot.h
	src/ParseParam.h 
	src/ImageProcess.h
 	src/getopt.h)
set(ALL_LIST 
	${SOURCE_ALL_LIST} 
	${SOURCE_HEADER_LIST})

link_directories(/opt/Qt/5.3/gcc/lib)
add_executable(ImageProcessing ${ALL_LIST})
set(LINK_EXT_LIBRARY 
	opencv_calib3d 
	opencv_contrib 
	opencv_core 
	opencv_video 
	opencv_flann 
	opencv_imgproc 
	opencv_ts 
	opencv_highgui 
	opencv_objdetect 
	opencv_ml 
	opencv_legacy 
	opencv_features2d 
	opencv_gpu 
	Qt5Core 
	Qt5Gui 
	Qt5OpenGL 
	Qt5PlatformSupport 
	Qt5Widgets )
target_link_libraries(ImageProcessing ${LINK_EXT_LIBRARY})

# Add library.
if(APPLE)
	#SET(GUI_TYPE MACOSX_BUNDLE)
	#INCLUDE_DIRECTORIES ( /Developer/Headers/FlatCarbon )
	FIND_LIBRARY(CARBON_LIBRARY Carbon)
	FIND_LIBRARY(QUICKTIME_LIBRARY QuickTime )
	FIND_LIBRARY(FOUNDATION_LIBRARY Foundation )
	FIND_LIBRARY(APP_SERVICES_LIBRARY ApplicationServices )
	MARK_AS_ADVANCED (CARBON_LIBRARY
		     QUICKTIME_LIBRARY
		     APP_SERVICES_LIBRARY)
	SET(EXTRA_LIBS ${CARBON_LIBRARY} ${QUICKTIME_LIBRARY} ${APP_SERVICES_LIBRARY} ${FOUNDATION_LIBRARY})

	if ("${CMAKE_SIZEOF_VOID_P}" EQUAL "8") 
		message(SYSTEM64 "it's x64")
		if(${APPLE_TYPE} STREQUAL "mac_x64")
			set(${BINARY_NAME} ${BINARY_NAME}_mac)
			target_link_libraries(${BINARY_NAME} ${EXTRA_LIBS} ${CONFIG_LINK_LIBRARY})
		elseif(${APPLE_TYPE} STREQUAL "ios_emu")
			 set(${BINARY_NAME} ${BINARY_NAME}_ios)
			 target_link_libraries(${BINARY_NAME}  ${EXTRA_LIBS} ${CONFIG_LINK_LIBRARY})
		endif()
	endif()
endif(APPLE)

