The source files are located in sim.
The sample programs are compiled into the test & benchmark binaries as there is no program loader yet.
They can be written in decoded form so it's quite easy to write something in main.cpp and then recompile

The code is definitely not perfect and there is plenty of room for more features.

Build dependencies
  libfmt 5.2.1
  meson
  ninja
  
Then do
  cd sim
  meson build
  ninja -C build && ./build/testrunner
  

