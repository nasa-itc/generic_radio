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

def run_generic_radio_device_test():
    ##
    ## Enable / disable, control hardware communications
    ##
    for n in range(GENERIC_RADIO_TEST_LOOP_COUNT):

        safe_GENERIC_RADIO()

        #radio does not have enable/disable functionality, so we just check the device is alive.
        confirm_GENERIC_RADIO_data_loop()