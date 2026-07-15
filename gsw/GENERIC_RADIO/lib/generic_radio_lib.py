# Library for GENERIC_RADIO_DEBUG Target
import sys
import glob

for p in glob.glob('/gems/gems/openc3-cosmos-nos3-*/targets/GENERIC_RADIO/scripts'):
    if p not in sys.path:
        sys.path.append(p)

try:
    from openc3.script import cmd, tlm, check, wait_check_packet
    import time
except ImportError:
    pass

#
# Definitions
#
GENERIC_RADIO_CMD_SLEEP = 0.25
GENERIC_RADIO_RESPONSE_TIMEOUT = 5
GENERIC_RADIO_TEST_LOOP_COUNT = 1
GENERIC_RADIO_DEVICE_LOOP_COUNT = 1


# cmd("GENERIC_RADIO_DEBUG GENERIC_RADIO_NOOP_CC")
# cmd("GENERIC_RADIO_DEBUG GENERIC_RADIO_CONFIG_CC with DEVICE_CONFIG 0")
# cmd("GENERIC_RADIO_DEBUG GENERIC_RADIO_CONFIG_CC with DEVICE_CONFIG 1")
# cmd("GENERIC_RADIO_DEBUG GENERIC_RADIO_PROXIMITY_CC with SCID 0, PROX_DATA 0x1930C00000010000")
# cmd("GENERIC_RADIO_DEBUG GENERIC_RADIO_PROXIMITY_CC with SCID 1, PROX_DATA 0x1930C00000010000")
# cmd("GENERIC_RADIO_DEBUG GENERIC_RADIO_RST_COUNTERS_CC")
# cmd("GENERIC_RADIO_DEBUG GENERIC_RADIO_REQ_HK")

#
# Functions
#
def get_GENERIC_RADIO_hk():
    cmd("GENERIC_RADIO_DEBUG GENERIC_RADIO_REQ_HK")
    wait_check_packet("GENERIC_RADIO_DEBUG", "GENERIC_RADIO_HK_TLM", 1, GENERIC_RADIO_RESPONSE_TIMEOUT)
    time.sleep(GENERIC_RADIO_CMD_SLEEP)


def GENERIC_RADIO_cmd(command_string):
    count = tlm("GENERIC_RADIO_DEBUG GENERIC_RADIO_HK_TLM CMD_COUNT") + 1

    if (count == 256):
        count = 0

    cmd(command_string)
    get_GENERIC_RADIO_hk()
    current = tlm("GENERIC_RADIO_DEBUG GENERIC_RADIO_HK_TLM CMD_COUNT")
    if (current != count):
        # Try again
        cmd(command_string)
        get_GENERIC_RADIO_hk()
        current = tlm("GENERIC_RADIO_DEBUG GENERIC_RADIO_HK_TLM CMD_COUNT")
        if (current != count):
            # Third times the charm
            cmd(command_string)
            get_GENERIC_RADIO_hk()
            current = tlm("GENERIC_RADIO_DEBUG GENERIC_RADIO_HK_TLM CMD_COUNT")
            
    check(f"GENERIC_RADIO_DEBUG GENERIC_RADIO_HK_TLM CMD_COUNT >= {count}")


def safe_GENERIC_RADIO():
    get_GENERIC_RADIO_hk()

def confirm_GENERIC_RADIO_data():
    dev_cmd_cnt = tlm("GENERIC_RADIO_DEBUG GENERIC_RADIO_HK_TLM DEVICE_COUNTER")
    dev_cmd_err_cnt = tlm("GENERIC_RADIO_DEBUG GENERIC_RADIO_HK_TLM DEVICE_ERR_COUNT")
    
    get_GENERIC_RADIO_hk()
    time.sleep(1)
    check(f"GENERIC_RADIO_DEBUG GENERIC_RADIO_HK_TLM DEVICE_COUNTER >= {dev_cmd_cnt}")
    check(f"GENERIC_RADIO_DEBUG GENERIC_RADIO_HK_TLM DEVICE_ERR_COUNT == {dev_cmd_err_cnt}")

def confirm_GENERIC_RADIO_data_loop():
    for n in range(GENERIC_RADIO_DEVICE_LOOP_COUNT):
        confirm_GENERIC_RADIO_data()

#
# Simulator Functions
#
def generic_radio_prepare_ast():
    # Get to known state
    safe_GENERIC_RADIO()

    # Confirm data
    confirm_GENERIC_RADIO_data_loop()

def generic_radio_sim_enable():
    cmd("SIM_CMDBUS_BRIDGE GENERIC_RADIO_SIM_ENABLE")

def generic_radio_sim_disable():
    cmd("SIM_CMDBUS_BRIDGE GENERIC_RADIO_SIM_DISABLE")