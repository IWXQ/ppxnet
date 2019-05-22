# ppxnet-config.cmake - package configuration file

get_filename_component(SELF_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

if(CMAKE_BUILD_TYPE STREQUAL "" OR NOT DEFINED CMAKE_BUILD_TYPE)
	include(${SELF_DIR}/ppxnet-target.cmake)
else()
	include(${SELF_DIR}/ppxnet-target-${CMAKE_BUILD_TYPE}.cmake)
endif()

if(TARGET ppxnet)
	get_target_property(PPXNET_INCLUDE_DIRS ppxnet INTERFACE_INCLUDE_DIRECTORIES)
	get_target_property(PPXNET_LIBRARIES ppxnet "LOCATION${CMAKE_BUILD_TYPE}")
endif()