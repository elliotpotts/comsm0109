project('sim', 'cpp', default_options: ['cpp_std=c++2a'])
libfmt = dependency('fmt')
incdir = include_directories('include')
common_sources = [
    'src/control.cpp',
    'src/reservation_station.cpp',
    'src/reorder_buffer.cpp',
    'src/insn.cpp',
    'src/execution_unit.cpp',
    'src/lsq.cpp'
]
executable('sim', ['src/main.cpp'] + common_sources, dependencies: libfmt, include_directories: incdir)
executable('testrunner', ['src/test.cpp'] + common_sources, dependencies: libfmt, include_directories: incdir)
executable('bmark', ['src/benchmark.cpp'] + common_sources, dependencies: libfmt, include_directories: incdir)
