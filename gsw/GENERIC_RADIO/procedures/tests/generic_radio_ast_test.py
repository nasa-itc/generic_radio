import sys
import glob
import time

for p in glob.glob('/gems/gems/openc3-cosmos-nos3-*/targets/GENERIC_RADIO/scripts'):
    if p not in sys.path:
        sys.path.append(p)

from openc3.script import cmd, tlm, check, wait_check

try:
    from nos3.generic_radio_lib import *
except ImportError:
    pass


def run_generic_radio_ast_test():
    ##
    ## This script tests the cFS component in an automated scenario.
    ## Currently this includes: 
    ##   Hardware failure
    ##

    ##
    ## Hardware failure
    ##
    for n in range(GENERIC_RADIO_TEST_LOOP_COUNT):
        generic_radio_prepare_ast()

        # Disable sim and confirm device error counts increase
        get_GENERIC_RADIO_hk()
        dev_cmd_cnt = tlm("GENERIC_RADIO_DEBUG GENERIC_RADIO_HK_TLM DEVICE_COUNTER")
        dev_cmd_err_cnt = tlm("GENERIC_RADIO_DEBUG GENERIC_RADIO_HK_TLM DEVICE_ERR_COUNT")

        generic_radio_sim_disable()
        time.sleep(1)
        
        cmd("GENERIC_RADIO_DEBUG GENERIC_RADIO_NOOP_CC") # sending command that should fail due to sim disabled
        time.sleep(1)

        get_GENERIC_RADIO_hk()

        wait_check(f"GENERIC_RADIO_DEBUG GENERIC_RADIO_HK_TLM DEVICE_COUNTER >= {dev_cmd_cnt}", 5)
        wait_check(f"GENERIC_RADIO_DEBUG GENERIC_RADIO_HK_TLM DEVICE_ERR_COUNT > {dev_cmd_err_cnt}", 5)

        # Enable sim and confirm return to nominal operation
        generic_radio_sim_enable()
        get_GENERIC_RADIO_hk()
        confirm_GENERIC_RADIO_data_loop()