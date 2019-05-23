# ppxnet-config.cmake - package configuration file

get_filename_component(SELF_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

if(EXISTS ${SELF_DIR}/ppxnet-target.cmake)
	include(${SELF_DIR}/ppxnet-target.cmake)
endif()

if(EXISTS ${SELF_DIR}/ppxnet-static-target.cmake)
	include(${SELF_DIR}/ppxnet-static-target.cmake)
endif()
