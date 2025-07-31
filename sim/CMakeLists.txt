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



# Set gsw_flag to 1 if TCP connection to CryptoLib, else 0
set(gsw_flag 1)

# Add the preprocessor macro
target_compile_definitions(generic_radio_sim PUBLIC TCP_GROUND=${gsw_flag})

# Function to get a specific variable from env.sh
function(get_env_var VAR_NAME OUTPUT_VAR)
    execute_process(
        COMMAND bash -c "source ../../scripts/env.sh && echo $${VAR_NAME}"
        OUTPUT_VARIABLE VAR_VALUE
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE CMD_RESULT
    )
    
    # Check if command succeeded
    if(CMD_RESULT)
        message(FATAL_ERROR "Failed to source env.sh (exit code: ${CMD_RESULT})")
    endif()
    
    set(${OUTPUT_VAR} ${VAR_VALUE} PARENT_SCOPE)
endfunction()

# Get variables
get_env_var("RADIO_TX_FSW_PORT" TX_FSW_PORT)
get_env_var("RADIO_RX_FSW_PORT" RX_FSW_PORT)

# Test the conditions explicitly
if(TX_FSW_PORT)
    target_compile_definitions(generic_radio_sim PUBLIC TX_FSW_PORT=${TX_FSW_PORT})
else()
    set(TX_FSW_PORT 5010) # Default Value
    target_compile_definitions(generic_radio_sim PUBLIC TX_FSW_PORT=${TX_FSW_PORT})
endif()

if(RX_FSW_PORT)
    target_compile_definitions(generic_radio_sim PUBLIC RX_FSW_PORT=${RX_FSW_PORT})
else()
    set(RX_FSW_PORT 5011) # Default Value
    target_compile_definitions(generic_radio_sim PUBLIC RX_FSW_PORT=${RX_FSW_PORT})
endif()

target_link_libraries(generic_radio_sim ${generic_radio_sim_libs})
install(TARGETS generic_radio_sim LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
