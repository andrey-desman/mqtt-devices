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

	if("${${var}_INCLUDE}" STREQUAL "${var}_INCLUDE-NOTFOUND")
		message(SEND_ERROR "Header ${header} not found...")
	else()
		message("Found include file ${header} at ${${var}_INCLUDE}")
		include_directories(${${var}_INCLUDE})
	endif()
endmacro(find_add_library)

include_directories(lib)

SET( CMAKE_SHARED_LINKER_FLAGS  "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined" )
SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11" )

find_package(Boost REQUIRED COMPONENTS program_options)

find_add_library(PAHO_C_ASYNC paho-mqtt3a MQTTAsync.h)
find_add_library(EV ev ev++.h)
find_add_library(PAHO_CPP mqttpp mqtt/async_client.h)

add_subdirectory(lib)

add_executable(sample_app sample_app.cpp)
target_link_libraries(sample_app mqtt_app)

add_executable(event_app event_app.cpp)
target_link_libraries(event_app mqtt_app)

add_executable(lua_app lua_app.cpp)
target_link_libraries(lua_app lua5.3 mqtt_app)
