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
	add_definitions(-DPPXNET_STATIC)
endif()

add_executable(
	${EXE_NAME}
	WIN32
	main.cpp
	)
	
	message(STATUS "PPXBASE_LIBRARIES=${PPXBASE_LIBRARIES}")
	message(STATUS "PPXBASE_INCLUDE_DIRS=${PPXBASE_INCLUDE_DIRS}")
# Win32 Console
set_target_properties(${EXE_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
set_target_properties(${EXE_NAME} PROPERTIES COMPILE_DEFINITIONS "_CONSOLE")

target_include_directories(${EXE_NAME} PUBLIC ${PPXBASE_INCLUDE_DIRS})

if(BUILD_SHARED_LIBS)
	target_link_libraries(${EXE_NAME} 
		$<TARGET_LINKER_FILE:ppxnet> )
else()
	target_link_libraries(${EXE_NAME} 
		$<TARGET_LINKER_FILE:ppxnet>)
endif()

