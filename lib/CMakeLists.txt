add_library(mqtt_app SHARED mqtt_switch_controller.cpp mqtt_evdev.cpp mqtt_connection.cpp mqtt_app.cpp mbus_switch.cpp options.cpp)
target_link_libraries(mqtt_app ${PAHO_CPP_LIB} ${EV_LIB} ${MBUS_LIB} boost_program_options)

