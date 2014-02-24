Running the Simulation
======================

`make` is used as both the build tool and also for execution. Simply
execute `make run` to run the simulation.

Environment
-----------

There are no dependencies to external libraries. The C++11 standard is
used and compiled with clang++.

Running the Tests
=================

The tests are commented out in `src/BayesNetTests.cpp`. 

1. Uncomment the test `runTests()` function in `main()`
2. Rebuild the solution `make clean && make`
3. Run the tests `make run`

Report
======

The report is available in `doc/doc.pdf`.
