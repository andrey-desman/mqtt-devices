add_library(mqtt-device SHARED mqtt_switch_controller.cpp mqtt_connection.cpp mqtt_app.cpp mbus_switch.cpp options.cpp)
target_link_libraries(mqtt-device ${PAHO_CPP_LIB} ${EV_LIB} ${MBUS_LIB} ${LUA_LIB})

install(TARGETS mqtt-device
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib)
