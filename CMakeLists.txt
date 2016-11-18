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
include_directories(devices)

SET( CMAKE_SHARED_LINKER_FLAGS  "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined" )
SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11" )

find_package(Boost REQUIRED COMPONENTS program_options)

find_add_library(MBUS mbus mbus.h)
find_add_library(PAHO_C_ASYNC paho-mqtt3a MQTTAsync.h)
find_add_library(EV ev ev++.h)
find_add_library(PAHO_CPP mqttpp mqtt/async_client.h)
find_add_library(LUA lua5.3 lua5.3/lua.h)

add_subdirectory(lib)

add_executable(sample_switch devices/sample/main.cpp)
target_link_libraries(sample_switch mqtt-device)

add_executable(kmtronic_usb_relay devices/kmtronic/kmtronic_usb_relay.cpp devices/kmtronic/main.cpp)
target_link_libraries(kmtronic_usb_relay mqtt-device)

add_executable(hd0742m_dimmer devices/hd0742m/hd0742m_dimmer.cpp devices/hd0742m/main.cpp)
target_link_libraries(hd0742m_dimmer mqtt-device)

add_executable(rs485_2ch_relay devices/rs485_2ch_relay/rs485_2ch_relay.cpp devices/rs485_2ch_relay/main.cpp)
target_link_libraries(rs485_2ch_relay mqtt-device)

add_executable(keyboard devices/keyboard/mqtt_evdev.cpp devices/keyboard/main.cpp)
target_link_libraries(keyboard mqtt-device)

add_executable(logic_controller devices/logic_controller/lua_timer.cpp devices/logic_controller/main.cpp)
target_link_libraries(logic_controller ${LUA_LIB} mqtt-device)

add_custom_command(
    TARGET logic_controller POST_BUILD
	COMMAND rm -f ${CMAKE_BINARY_DIR}/lua
	COMMAND ln -sf ${CMAKE_SOURCE_DIR}/lua ${CMAKE_BINARY_DIR}/lua
)

