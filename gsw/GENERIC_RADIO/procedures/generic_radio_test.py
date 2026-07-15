import sys
import glob

for p in glob.glob('/gems/gems/openc3-cosmos-nos3-*/targets/GENERIC_RADIO/scripts'):
    if p not in sys.path:
        sys.path.append(p)

from openc3.testing import Group, Suite

try:
    from nos3.generic_radio_lib import *
    from tests.generic_radio_app_test import run_generic_radio_app_test
    from tests.generic_radio_device_test import run_generic_radio_device_test
    from tests.generic_radio_ast_test import run_generic_radio_ast_test
except ImportError:
    pass


class GENERIC_RADIO_Functional_Test(Group):
    def setup(self):
        safe_GENERIC_RADIO()

    def script_application(self):
        run_generic_radio_app_test()

    def script_device(self):
        run_generic_radio_device_test()

    def teardown(self):
        safe_GENERIC_RADIO()


class GENERIC_RADIO_Scenario_Test(Group):
    def setup(self):
        safe_GENERIC_RADIO()

    def script_AST(self):
        run_generic_radio_ast_test()

    def teardown(self):
        safe_GENERIC_RADIO()


class Generic_radio_Test(Suite):
    def setup(self):
        self.add_group('GENERIC_RADIO_Functional_Test', GENERIC_RADIO_Functional_Test)
        self.add_group('GENERIC_RADIO_Scenario_Test', GENERIC_RADIO_Scenario_Test)
        safe_GENERIC_RADIO()
  
    def teardown(self):
        safe_GENERIC_RADIO()