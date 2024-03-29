﻿###############################################################################
# Copyright (C) 2018 - 2020, winsoft666, <winsoft666@outlook.com>.
#
# THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
# EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
#
# Expect bugs
#
# Please use and enjoy. Please let me know of any bugs/improvements
# that you have found/implemented and I will fix/incorporate them into this
# file.
###############################################################################

if(BUILD_SHARED_LIBS)
	set(LIB_NAME ppxnet)
else()
	set(LIB_NAME ppxnet-static)
endif()


if (MSVC AND USE_STATIC_CRT)
    set(CompilerFlags
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
        )
    foreach(CompilerFlag ${CompilerFlags})
        string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
    endforeach()
endif()



file(GLOB ROOT_HEADER_FILES 		../include/*.h ../include/*.hpp)
file(GLOB NET_HEADER_FILES 			../include/ppxnet/*.h ../include/ppxnet/*.hpp)

file(GLOB NET_SOURCE_FILES 			./*.cpp)


source_group("Source Files" 			FILES ${NET_SOURCE_FILES})

source_group("Header Files" 			FILES ${ROOT_HEADER_FILES})
source_group("Header Files\\ppxnet"      FILES ${NET_HEADER_FILES})



if(BUILD_SHARED_LIBS)
	add_definitions(-DPPXNET_EXPORTS)
	set(PPXNET_STATIC NO)
	
	add_library(
		${LIB_NAME}
		SHARED
		${NET_SOURCE_FILES}
		${ROOT_HEADER_FILES}
		${NET_HEADER_FILES}
	)
	
	# set output name prefix/suffix
	set_target_properties(${LIB_NAME} PROPERTIES 
		OUTPUT_NAME ${LIB_NAME}
		DEBUG_OUTPUT_NAME ${LIB_NAME}${DEBUG_LIBNAME_SUFFIX})
else()
	add_definitions(-DPPXNET_STATIC)
	set(PPXNET_STATIC YES)
	
	add_library(
		${LIB_NAME}
		STATIC
		${NET_SOURCE_FILES}
		${ROOT_HEADER_FILES}
		${NET_HEADER_FILES}
	)
	
	# set output name prefix/suffix
    set_target_properties(${LIB_NAME} PROPERTIES 
		OUTPUT_NAME ${LIB_NAME}
		DEBUG_OUTPUT_NAME ${LIB_NAME}${DEBUG_LIBNAME_SUFFIX})
		
	set_target_properties(${LIB_NAME} PROPERTIES 
		INTERFACE_COMPILE_DEFINITIONS PPXNET_STATIC)
endif()


# CURL
find_package(CURL REQUIRED)
target_link_libraries(${LIB_NAME} PUBLIC 
	${CURL_LIBRARIES})
target_include_directories(${LIB_NAME} PUBLIC 
	${CURL_INCLUDE_DIRS})

find_package(OpenSSL REQUIRED)
target_link_libraries(${LIB_NAME} PUBLIC OpenSSL::SSL OpenSSL::Crypto)

# PPXBASE
find_package(ppxbase REQUIRED)
if(BUILD_SHARED_LIBS)
	target_link_libraries(${LIB_NAME} PUBLIC 
		ppxbase)
else()
	target_link_libraries(${LIB_NAME} PUBLIC 
		ppxbase-static)
endif()


target_link_libraries(${LIB_NAME} PUBLIC 
	Ws2_32.lib Crypt32.lib)

# Set output directory
set_target_properties(${LIB_NAME}
	PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" )

target_include_directories(${LIB_NAME} INTERFACE
	$<INSTALL_INTERFACE:include>)

install(TARGETS ${LIB_NAME}
	EXPORT ${LIB_NAME}-target
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib)


install(FILES ${ROOT_HEADER_FILES} 			DESTINATION include)
install(FILES ${NET_HEADER_FILES} 			DESTINATION include/ppxnet)


install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../CMake/ppxnet-config.cmake 
	DESTINATION ${PPXNET_INSTALL_CMAKE_DIR})

export(TARGETS ${LIB_NAME}
       APPEND FILE ${PROJECT_BINARY_DIR}/${LIB_NAME}-target.cmake
)

install(EXPORT ${LIB_NAME}-target
        FILE ${LIB_NAME}-target.cmake
        DESTINATION ${PPXNET_INSTALL_CMAKE_DIR}
)
	


