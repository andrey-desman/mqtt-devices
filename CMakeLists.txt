cmake_minimum_required(VERSION 3.0)

if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

function(find_add_library var lib header)
	if(DEFINED ${var}_LIB)
		if(NOT "${${var}_LIB}" STREQUAL "${var}_LIB-NOTFOUND")
			message("Not testing ${lib} since ${var} was previously found at ${${var}_LIB}")
			include_directories(${${var}_INCLUDE})
		get_filename_component(${var}_INCLUDE_FILE_DIR ${${var}_INCLUDE_FILE} DIRECTORY)
			include_directories(${${var}_INCLUDE_FILE_DIR})
			return()
		endif()
	endif()
	find_library(${var}_LIB ${lib})

	if("${${var}_LIB}" STREQUAL "${var}_LIB-NOTFOUND")
		message("Library ${lib} not found...")
		return()
	else()
		message("Found library ${lib} at ${${var}_LIB}")
	endif()

	find_path(${var}_INCLUDE ${header})
	find_file(${var}_INCLUDE_FILE ${header})

	if("${${var}_INCLUDE}" STREQUAL "${var}_INCLUDE-NOTFOUND")
		message("Header ${header} not found...")
		SET(${var}_LIB "${var}_LIB-NOTFOUND" PARENT_SCOPE)
		return()
	else()
		message("Found include file ${header} at ${${var}_INCLUDE}")
		include_directories(${${var}_INCLUDE})
		get_filename_component(${var}_INCLUDE_FILE_DIR ${${var}_INCLUDE_FILE} DIRECTORY)
		include_directories(${${var}_INCLUDE_FILE_DIR})
	endif()
endfunction(find_add_library)

include_directories(lib)
include_directories(devices)

SET( CMAKE_SHARED_LINKER_FLAGS  "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined" )
SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++17" )

find_package(Boost)
include_directories(${Boost_INCLUDE_DIRS})

find_add_library(MODBUS modbus modbus/modbus.h)
find_add_library(PAHO_C_ASYNC paho-mqtt3a MQTTAsync.h)
find_add_library(EV ev ev++.h)
find_add_library(PAHO_CPP paho-mqttpp3 mqtt/async_client.h)
find_add_library(LUA lua lua.h)
find_add_library(LUA lua5.3 lua5.3/lua.h)

add_subdirectory(lib)

add_executable(mqtt_sample_switch devices/sample/main.cpp)
target_link_libraries(mqtt_sample_switch mqtt-device)

add_executable(mqtt_kmtronic_usb_relay devices/kmtronic/kmtronic_usb_relay.cpp devices/kmtronic/main.cpp)
target_link_libraries(mqtt_kmtronic_usb_relay mqtt-device)

add_executable(mqtt_am82tv devices/am82tv/am82tv.cpp devices/am82tv/main.cpp)
target_link_libraries(mqtt_am82tv mqtt-device)

add_executable(mqtt_hd0742m_dimmer devices/hd0742m/hd0742m_dimmer.cpp devices/hd0742m/main.cpp)
target_link_libraries(mqtt_hd0742m_dimmer mqtt-device)

add_executable(mqtt_rs485_2ch_relay devices/rs485_2ch_relay/rs485_2ch_relay.cpp devices/rs485_2ch_relay/main.cpp)
target_link_libraries(mqtt_rs485_2ch_relay mqtt-device)

add_executable(mqtt_input devices/input/mqtt_evdev.cpp devices/input/main.cpp)
target_link_libraries(mqtt_input mqtt-device)

add_executable(mqtt_controller
	devices/controller/lua_host.cpp
	devices/controller/lua_timer.cpp
	devices/controller/main.cpp)
target_link_libraries(mqtt_controller ${LUA_LIB} mqtt-device)

install(TARGETS mqtt_kmtronic_usb_relay mqtt_hd0742m_dimmer mqtt_rs485_2ch_relay mqtt_am82tv mqtt_input mqtt_controller
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib)

install(DIRECTORY lua/ DESTINATION /etc/mqtt-devices)
