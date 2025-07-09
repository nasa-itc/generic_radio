project(generic_radio_sim)

find_package(ITC_Common REQUIRED QUIET COMPONENTS itc_logger)
find_package(NOSENGINE REQUIRED QUIET COMPONENTS common transport client uart)

include_directories(inc
                    ${sim_common_SOURCE_DIR}/inc
                    ${ITC_Common_INCLUDE_DIRS}
                    ${NOSENGINE_INCLUDE_DIRS})

set(generic_radio_sim_src
    src/generic_radio_hardware_model.cpp
    src/generic_radio_42_data_provider.cpp
    src/generic_radio_data_provider.cpp
    src/generic_radio_data_point.cpp
)

# For Code::Blocks and other IDEs
file(GLOB generic_radio_sim_inc inc/*.hpp)

set(generic_radio_sim_libs
    sim_common
    ${ITC_Common_LIBRARIES}
    ${NOSENGINE_LIBRARIES}
)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH}:$ORIGIN/../lib") # Pick up .so in install directory

add_library(generic_radio_sim SHARED ${generic_radio_sim_src} ${generic_radio_sim_inc})

# Run the shell command to extract <gsw>...</gsw>
execute_process(
    COMMAND bash -c "sed -n 's:.*<gsw>\\(.*\\)</gsw>.*:\\1:p' ../../cfg/nos3-mission.xml"
    OUTPUT_VARIABLE gsw_raw
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Set gsw_flag to 1 if output is "yamcs", else 0
if ("${gsw_raw}" STREQUAL "yamcs")
    set(gsw_flag 1)
else()
    set(gsw_flag 0)
endif()

message(STATUS "Ground software: ${gsw_raw}")
message(STATUS "TCP_GROUND set to: ${gsw_flag}")

# Add the preprocessor macro
target_compile_definitions(generic_radio_sim PUBLIC TCP_GROUND=${gsw_flag})

target_link_libraries(generic_radio_sim ${generic_radio_sim_libs})
install(TARGETS generic_radio_sim LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
