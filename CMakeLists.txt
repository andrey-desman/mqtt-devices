cmake_minimum_required(VERSION 3.0)

if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

macro(find_add_library var lib header)
	find_library(${var}_LIB ${lib})

	if("${${var}_LIB}" STREQUAL "${var}_LIB-NOTFOUND")
		message(SEND_ERROR "Library ${lib} not found...")
	else()
		message("Found library ${lib}")
	endif()

	find_path(${var}_INCLUDE ${header})
	find_file(${var}_INCLUDE_FILE ${header})

	if("${${var}_INCLUDE}" STREQUAL "${var}_INCLUDE-NOTFOUND")
		message(SEND_ERROR "Header ${header} not found...")
	else()
		message("Found include file ${header} at ${${var}_INCLUDE}")
		include_directories(${${var}_INCLUDE})
		get_filename_component(${var}_INCLUDE_FILE_DIR ${${var}_INCLUDE_FILE} DIRECTORY)
		include_directories(${${var}_INCLUDE_FILE_DIR})
	endif()
endmacro(find_add_library)

include_directories(lib)

SET( CMAKE_SHARED_LINKER_FLAGS  "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined" )
SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11" )

find_package(Boost REQUIRED COMPONENTS program_options)

find_add_library(PAHO_C_ASYNC paho-mqtt3a MQTTAsync.h)
find_add_library(EV ev ev++.h)
find_add_library(PAHO_CPP mqttpp mqtt/async_client.h)
find_add_library(LUA lua5.3 lua5.3/lua.h)

add_subdirectory(lib)

add_executable(sample_app sample_app.cpp)
target_link_libraries(sample_app mqtt_app)

add_executable(event_app event_app.cpp)
target_link_libraries(event_app mqtt_app)

add_executable(lua_app lua_app.cpp)
target_link_libraries(lua_app ${LUA_LIB} mqtt_app)

add_custom_command(
    TARGET lua_app POST_BUILD
	COMMAND rm -f ${CMAKE_BINARY_DIR}/lua
	COMMAND ln -sf ${CMAKE_SOURCE_DIR}/lua ${CMAKE_BINARY_DIR}/lua
)

