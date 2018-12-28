Since starting the coursework I went down many dead ends and suffered badly from 'analysis parlysis'
As a result I rethought it a lot - the old stuff is in ./old
I settled on a final version in c++ though the code is very bad and ugly

The source files are located in sim.
The sample programs are compiled into the test & benchmark binaries as there is no program loader yet.
They can be written in decoded form so it's quite easy to write something in main.cpp and then recompile

Build dependencies
  libfmt 5.2.1
  meson
  ninja
  
Then do
  cd sim
  meson build
  ninja -C build && ./build/testrunner
  

