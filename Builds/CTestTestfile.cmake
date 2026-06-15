# CMake generated Testfile for 
# Source directory: C:/Users/renat/source/repos/voxel_engine
# Build directory: C:/Users/renat/source/repos/voxel_engine/Builds
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test([=[engine_tests]=] "C:/Users/renat/source/repos/voxel_engine/Builds/Debug/engine_tests.exe")
  set_tests_properties([=[engine_tests]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/renat/source/repos/voxel_engine/CMakeLists.txt;194;add_test;C:/Users/renat/source/repos/voxel_engine/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test([=[engine_tests]=] "C:/Users/renat/source/repos/voxel_engine/Builds/Release/engine_tests.exe")
  set_tests_properties([=[engine_tests]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/renat/source/repos/voxel_engine/CMakeLists.txt;194;add_test;C:/Users/renat/source/repos/voxel_engine/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test([=[engine_tests]=] "C:/Users/renat/source/repos/voxel_engine/Builds/MinSizeRel/engine_tests.exe")
  set_tests_properties([=[engine_tests]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/renat/source/repos/voxel_engine/CMakeLists.txt;194;add_test;C:/Users/renat/source/repos/voxel_engine/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test([=[engine_tests]=] "C:/Users/renat/source/repos/voxel_engine/Builds/RelWithDebInfo/engine_tests.exe")
  set_tests_properties([=[engine_tests]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/renat/source/repos/voxel_engine/CMakeLists.txt;194;add_test;C:/Users/renat/source/repos/voxel_engine/CMakeLists.txt;0;")
else()
  add_test([=[engine_tests]=] NOT_AVAILABLE)
endif()
