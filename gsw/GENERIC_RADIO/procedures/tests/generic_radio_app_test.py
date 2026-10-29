import sys
import glob

for p in glob.glob('/gems/gems/openc3-cosmos-nos3-*/targets/GENERIC_RADIO/scripts'):
    if p not in sys.path:
        sys.path.append(p)

from openc3.script import cmd, tlm, check

try:
    from nos3.generic_radio_lib import *
except ImportError:
    pass


def run_generic_radio_app_test():

    safe_GENERIC_RADIO()

    ##
    ##   Housekeeping, request telemetry to be published on the software bus
    ##
    for n in range(GENERIC_RADIO_TEST_LOOP_COUNT):
        get_GENERIC_RADIO_hk()

    ##
    ## NOOP, no operation but confirm correct counters increment
    ##
    for n in range(GENERIC_RADIO_TEST_LOOP_COUNT):
        GENERIC_RADIO_cmd("GENERIC_RADIO_DEBUG GENERIC_RADIO_NOOP_CC")

    ##
    ## Reset counters, increment as done in NOOP and confirm ability to clear repeatably
    ##
    for n in range(GENERIC_RADIO_TEST_LOOP_COUNT):
        GENERIC_RADIO_cmd("GENERIC_RADIO_DEBUG GENERIC_RADIO_NOOP_CC")
        cmd("GENERIC_RADIO_DEBUG GENERIC_RADIO_RST_COUNTERS_CC")
        get_GENERIC_RADIO_hk()
        check("GENERIC_RADIO_DEBUG GENERIC_RADIO_HK_TLM CMD_COUNT == 0")
        check("GENERIC_RADIO_DEBUG GENERIC_RADIO_HK_TLM CMD_ERR_COUNT == 0")

    ##
    ## injecting bad commmands, checking error counters increase
    ##
    for n in range(GENERIC_RADIO_TEST_LOOP_COUNT):
        # Bad length
        cmd_cnt = tlm("GENERIC_RADIO_DEBUG GENERIC_RADIO_HK_TLM CMD_COUNT")
        cmd_err_cnt = tlm("GENERIC_RADIO_DEBUG GENERIC_RADIO_HK_TLM CMD_ERR_COUNT")
        cmd(f"GENERIC_RADIO_DEBUG GENERIC_RADIO_NOOP_CC with CCSDS_LENGTH {n+2}") # Note +2 due to CCSDS already being +1
        get_GENERIC_RADIO_hk()
        check(f"GENERIC_RADIO_DEBUG GENERIC_RADIO_HK_TLM CMD_COUNT == {cmd_cnt}")
        check(f"GENERIC_RADIO_DEBUG GENERIC_RADIO_HK_TLM CMD_ERR_COUNT == {cmd_err_cnt+1}")

    for n in range(6, 5 + GENERIC_RADIO_TEST_LOOP_COUNT + 1):
        # Bad command codes
        cmd_cnt = tlm("GENERIC_RADIO_DEBUG GENERIC_RADIO_HK_TLM CMD_COUNT")
        cmd_err_cnt = tlm("GENERIC_RADIO_DEBUG GENERIC_RADIO_HK_TLM CMD_ERR_COUNT")
        cmd(f"GENERIC_RADIO_DEBUG GENERIC_RADIO_NOOP_CC with CCSDS_FC {n+1}")
        get_GENERIC_RADIO_hk()
        check(f"GENERIC_RADIO_DEBUG GENERIC_RADIO_HK_TLM CMD_COUNT == {cmd_cnt}")
        check(f"GENERIC_RADIO_DEBUG GENERIC_RADIO_HK_TLM CMD_ERR_COUNT == {cmd_err_cnt+1}")