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
source_group("Header Files\\ppxnet"     FILES ${NET_HEADER_FILES})

set(_LINK_LIBRARYS "")



if(BUILD_SHARED_LIBS)
	add_definitions(-DPPXNET_EXPORTS)
	
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
						
	target_link_libraries(${LIB_NAME} PUBLIC 
		debug ${PPXBASE_INSTALL_DIR}/lib/Debug/ppxbase.lib
		optimized ${PPXBASE_INSTALL_DIR}/lib/Release/ppxbase.lib)
else()
	add_definitions(-DPPXNET_STATIC -DPPXBASE_STATIC -DCURL_STATICLIB)
	
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
		
	target_link_libraries(${LIB_NAME} PUBLIC 
		debug ${PPXBASE_INSTALL_DIR}/lib/Debug/ppxbase-static.lib
		optimized ${PPXBASE_INSTALL_DIR}/lib/Release/ppxbase-static.lib)
endif()


find_package(CURL REQUIRED)

target_link_libraries(${LIB_NAME} PRIVATE ${CURL_LIBRARIES})
target_include_directories(${LIB_NAME} PRIVATE ${CURL_INCLUDE_DIRS})
list(APPEND _LINK_LIBRARYS ${CURL_LIBRARIES})

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


install(TARGETS ${LIB_NAME}
	CONFIGURATIONS Debug
	RUNTIME DESTINATION bin/Debug
	LIBRARY DESTINATION lib/Debug
	ARCHIVE DESTINATION lib/Debug)
	
install(TARGETS ${LIB_NAME}
	CONFIGURATIONS Release
	RUNTIME DESTINATION bin/Release
	LIBRARY DESTINATION lib/Release
	ARCHIVE DESTINATION lib/Release)

install(FILES ${ROOT_HEADER_FILES} 			DESTINATION include)
install(FILES ${NET_HEADER_FILES} 			DESTINATION include/ppxnet)

