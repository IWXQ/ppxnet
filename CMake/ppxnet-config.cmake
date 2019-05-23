# ppxnet-config.cmake - package configuration file

get_filename_component(SELF_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

include(${SELF_DIR}/ppxnet-target.cmake)

if(TARGET ppxnet)
	get_target_property(PPXNET_INCLUDE_DIRS ppxnet INTERFACE_INCLUDE_DIRECTORIES)
	get_target_property(PPXNET_LIBRARIES ppxnet "LOCATION_${CMAKE_BUILD_TYPE}")
endif()