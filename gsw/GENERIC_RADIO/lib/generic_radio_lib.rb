# Library for GENERIC_RADIO Target
require 'cosmos'
require 'cosmos/script'

#
# Definitions
#
GENERIC_RADIO_CMD_SLEEP = 0.25
GENERIC_RADIO_RESPONSE_TIMEOUT = 5
GENERIC_RADIO_TEST_LOOP_COUNT = 1
GENERIC_RADIO_DEVICE_LOOP_COUNT = 5


# cmd("GENERIC_RADIO GENERIC_RADIO_NOOP_CC")
# cmd("GENERIC_RADIO GENERIC_RADIO_CONFIG_CC with DEVICE_CONFIG 0")
# cmd("GENERIC_RADIO GENERIC_RADIO_CONFIG_CC with DEVICE_CONFIG 1")
# cmd("GENERIC_RADIO GENERIC_RADIO_PROXIMITY_CC with SCID 0, PROX_DATA 0x1930C00000010000")
# cmd("GENERIC_RADIO GENERIC_RADIO_PROXIMITY_CC with SCID 1, PROX_DATA 0x1930C00000010000")
# cmd("GENERIC_RADIO GENERIC_RADIO_RST_COUNTERS_CC")
# cmd("GENERIC_RADIO GENERIC_RADIO_REQ_HK")

#
# Functions
#
def get_GENERIC_RADIO_hk()
    cmd("GENERIC_RADIO GENERIC_RADIO_REQ_HK")
    wait_check_packet("GENERIC_RADIO", "GENERIC_RADIO_HK_TLM", 1, GENERIC_RADIO_RESPONSE_TIMEOUT)
    sleep(GENERIC_RADIO_CMD_SLEEP)
end

def get_GENERIC_RADIO_data()
    cmd("GENERIC_RADIO GENERIC_RADIO_REQ_DATA")
    wait_check_packet("GENERIC_RADIO", "GENERIC_RADIO_DATA_TLM", 1, GENERIC_RADIO_RESPONSE_TIMEOUT)
    sleep(GENERIC_RADIO_CMD_SLEEP)
end

def GENERIC_RADIO_cmd(*command)
    count = tlm("GENERIC_RADIO GENERIC_RADIO_HK_TLM CMD_COUNT") + 1

    if (count == 256)
        count = 0
    end

    cmd(*command)
    get_GENERIC_RADIO_hk()
    current = tlm("GENERIC_RADIO GENERIC_RADIO_HK_TLM CMD_COUNT")
    if (current != count)
        # Try again
        cmd(*command)
        get_GENERIC_RADIO_hk()
        current = tlm("GENERIC_RADIO GENERIC_RADIO_HK_TLM CMD_COUNT")
        if (current != count)
            # Third times the charm
            cmd(*command)
            get_GENERIC_RADIO_hk()
            current = tlm("GENERIC_RADIO GENERIC_RADIO_HK_TLM CMD_COUNT")
        end
    end
    check("GENERIC_RADIO GENERIC_RADIO_HK_TLM CMD_COUNT >= #{count}")
end

def enable_GENERIC_RADIO()
    # Send command
    GENERIC_RADIO_cmd("GENERIC_RADIO GENERIC_RADIO_ENABLE_CC")
    # Confirm
    check("GENERIC_RADIO GENERIC_RADIO_HK_TLM DEVICE_ENABLED == 'ENABLED'")
end

def disable_GENERIC_RADIO()
    # Send command
    GENERIC_RADIO_cmd("GENERIC_RADIO GENERIC_RADIO_DISABLE_CC")
    # Confirm
    check("GENERIC_RADIO GENERIC_RADIO_HK_TLM DEVICE_ENABLED == 'DISABLED'")
end

def safe_GENERIC_RADIO()
    get_GENERIC_RADIO_hk()
    state = tlm("GENERIC_RADIO GENERIC_RADIO_HK_TLM DEVICE_ENABLED")
    if (state != "DISABLED")
        disable_GENERIC_RADIO()
    end
end

def confirm_GENERIC_RADIO_data()
    dev_cmd_cnt = tlm("GENERIC_RADIO GENERIC_RADIO_HK_TLM DEVICE_COUNT")
    dev_cmd_err_cnt = tlm("GENERIC_RADIO GENERIC_RADIO_HK_TLM DEVICE_ERR_COUNT")
    
    get_GENERIC_RADIO_data()
    # Note these checks assume default simulator configuration
    raw_x = tlm("GENERIC_RADIO GENERIC_RADIO_DATA_TLM RAW_GENERIC_RADIO_X")
    check("GENERIC_RADIO GENERIC_RADIO_DATA_TLM RAW_GENERIC_RADIO_Y >= #{raw_x*2}")
    check("GENERIC_RADIO GENERIC_RADIO_DATA_TLM RAW_GENERIC_RADIO_Z >= #{raw_x*3}")

    get_GENERIC_RADIO_hk()
    check("GENERIC_RADIO GENERIC_RADIO_HK_TLM DEVICE_COUNT >= #{dev_cmd_cnt}")
    check("GENERIC_RADIO GENERIC_RADIO_HK_TLM DEVICE_ERR_COUNT == #{dev_cmd_err_cnt}")
end

def confirm_GENERIC_RADIO_data_loop()
    GENERIC_RADIO_DEVICE_LOOP_COUNT.times do |n|
        confirm_GENERIC_RADIO_data()
    end
end

#
# Simulator Functions
#
def GENERIC_RADIO_prepare_ast()
    # Get to known state
    safe_GENERIC_RADIO()

    # Enable
    enable_GENERIC_RADIO()

    # Confirm data
    confirm_GENERIC_RADIO_data_loop()
end

def GENERIC_RADIO_sim_enable()
    cmd("SIM_CMDBUS_BRIDGE GENERIC_RADIO_SIM_ENABLE")
end

def GENERIC_RADIO_sim_disable()
    cmd("SIM_CMDBUS_BRIDGE GENERIC_RADIO_SIM_DISABLE")
end

def GENERIC_RADIO_sim_set_status(status)
    cmd("SIM_CMDBUS_BRIDGE GENERIC_RADIO_SIM_SET_STATUS with STATUS #{status}")
end
