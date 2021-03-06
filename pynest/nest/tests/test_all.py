# -*- coding: utf-8 -*-
#
# test_all.py
#
# This file is part of NEST.
#
# Copyright (C) 2004 The NEST Initiative
#
# NEST is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# NEST is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with NEST.  If not, see <http://www.gnu.org/licenses/>.

import unittest
import nest

from . import compatibility

from . import test_errors
from . import test_stack
from . import test_create
from . import test_status
from . import test_connectapi
from . import test_findconnections
from . import test_getconnections
from . import test_dataconnect
from . import test_connectoptions
from . import test_events
from . import test_networks
from . import test_threads
from . import test_csa


def suite():

    suite = unittest.TestSuite()

    suite.addTest(test_errors.suite())
    suite.addTest(test_stack.suite())
    suite.addTest(test_create.suite())                    
    suite.addTest(test_status.suite())
    suite.addTest(test_connectapi.suite())
    suite.addTest(test_findconnections.suite())    
    suite.addTest(test_getconnections.suite())
    suite.addTest(test_dataconnect.suite())
    suite.addTest(test_connectoptions.suite())    
    suite.addTest(test_events.suite())
    suite.addTest(test_networks.suite())
    suite.addTest(test_threads.suite())    
    suite.addTest(test_csa.suite())    
    
    return suite


if __name__ == "__main__":

    debug = nest.get_debug()
    nest.set_debug(True)

    runner = unittest.TextTestRunner(verbosity=2)
    runner.run(suite())

    nest.set_debug(debug)
