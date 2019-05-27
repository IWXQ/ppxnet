set(EXE_NAME test01)


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


if (NOT BUILD_SHARED_LIBS)
	# Use ppxnet static library
	add_definitions(-DPPXNET_STATIC -DPPXBASE_STATIC -DCURL_STATICLIB)
endif()

add_executable(
	${EXE_NAME}
	WIN32
	main.cpp
	)
	
# Win32 Console
set_target_properties(${EXE_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
set_target_properties(${EXE_NAME} PROPERTIES COMPILE_DEFINITIONS "_CONSOLE")

if(BUILD_SHARED_LIBS)
	target_link_libraries(${EXE_NAME} 
		$<TARGET_LINKER_FILE:ppxnet>)
	
	find_package(ppxbase REQUIRED)
    target_link_libraries(${EXE_NAME} ppxbase)
else()
	target_link_libraries(${EXE_NAME} $<TARGET_LINKER_FILE:ppxnet-static>)
	
	# CURL
	find_package(CURL REQUIRED)
	target_link_libraries(${EXE_NAME} ${CURL_LIBRARIES})
	
	find_package(ZLIB REQUIRED)
    target_link_libraries(${EXE_NAME} ZLIB::ZLIB)
	
	find_package(OpenSSL REQUIRED)
    target_link_libraries(${EXE_NAME} OpenSSL::SSL OpenSSL::Crypto)
	
	target_link_libraries(${EXE_NAME} Ws2_32.lib Crypt32.lib)


	find_package(ppxbase REQUIRED)
    target_link_libraries(${EXE_NAME} ppxbase-static)
endif()

