set(LIB_NAME ppxnet)


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
source_group("Header Files/ppxnet"     FILES ${NET_HEADER_FILES})

set(_LINK_LIBRARYS "")



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
						OUTPUT_NAME ${LIB_NAME} )
	# if (DEFINED PPXBASE_INSTALL_DIR)		
		# target_link_libraries(${LIB_NAME} PUBLIC 
			# debug ${PPXBASE_INSTALL_DIR}/lib/ppxbase-d.lib
			# optimized ${PPXBASE_INSTALL_DIR}/lib/ppxbase.lib)
	# endif()
else()
	add_definitions(-DPPXNET_STATIC -DPPXBASE_STATIC -DCURL_STATICLIB)
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
		OUTPUT_NAME ${LIB_NAME}${STATIC_LIBNAME_SUFFIX})
		
	set_target_properties(${LIB_NAME} PROPERTIES 
		INTERFACE_COMPILE_DEFINITIONS PPXNET_STATIC)

	# if (DEFINED PPXBASE_INSTALL_DIR)
		# target_link_libraries(${LIB_NAME} PUBLIC 
			# debug ${PPXBASE_INSTALL_DIR}/lib/ppxbase-static-d.lib
			# optimized ${PPXBASE_INSTALL_DIR}/lib/ppxbase-static.lib)
	# endif()
endif()

set_target_properties (${LIB_NAME} PROPERTIES 
	DEBUG_POSTFIX ${DEBUG_LIBNAME_SUFFIX})

find_package(CURL REQUIRED)

target_link_libraries(${LIB_NAME} PRIVATE ${CURL_LIBRARIES})
target_include_directories(${LIB_NAME} PRIVATE ${CURL_INCLUDE_DIRS})
list(APPEND _LINK_LIBRARYS ${CURL_LIBRARIES})


if (PPXBASE_INSTALL_DIR STREQUAL "")
	message(STATUS "do find_package(PPXBASE REQUIRED)")
	find_package(ppxbase REQUIRED)

	target_link_libraries(${LIB_NAME} PRIVATE ${PPXBASE_LIBRARIES})
	target_include_directories(${LIB_NAME} PRIVATE ${PPXBASE_INCLUDE_DIRS})
	list(APPEND _LINK_LIBRARYS ${PPXBASE_LIBRARIES})
endif()

target_link_libraries(${LIB_NAME} PRIVATE 
	Ws2_32.lib Iphlpapi.lib Userenv.lib Dbghelp.lib)
list(APPEND _LINK_LIBRARYS Ws2_32.lib Iphlpapi.lib Userenv.lib Dbghelp.lib)

# Set output directory
set_target_properties(${LIB_NAME}
	PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" )

set_property(GLOBAL PROPERTY PPX_LINK_LIBRARYS "${_LINK_LIBRARYS}" )

target_include_directories(${LIB_NAME} INTERFACE
	$<INSTALL_INTERFACE:include>)

install(TARGETS ${LIB_NAME}
	EXPORT ppxnet-target
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib)
	
export(TARGETS ${LIB_NAME}
       APPEND FILE ${PROJECT_BINARY_DIR}/ppxnet-target.cmake
       NAMESPACE PPXNET::
)

install(EXPORT ppxnet-target
        FILE ppxnet-target.cmake
        NAMESPACE PPXNET::
        DESTINATION ${PPXNET_INSTALL_CMAKE_DIR}
)
	

install(FILES ${ROOT_HEADER_FILES} 			DESTINATION include)
install(FILES ${NET_HEADER_FILES} 			DESTINATION include/ppxnet)

