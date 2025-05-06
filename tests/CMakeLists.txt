include_directories(${PROJECT_SOURCE_DIR}/include/eigen3)
add_definitions(-DLIBINTX_GPU_API_CUDA)

add_executable(interpolate.test EXCLUDE_FROM_ALL interpolate.test.cc)
target_link_libraries(interpolate.test libboys)
add_test(NAME interpolate COMMAND interpolate.test)

add_executable(boys.test EXCLUDE_FROM_ALL boys.test.cc)
target_link_libraries(boys.test libboys)
add_test(NAME boys COMMAND boys.test)

if (LIBINTX_ENABLE_CUDA)
  add_executable(boys.gpu.test EXCLUDE_FROM_ALL boys.gpu.test.cu)
  target_link_libraries(boys.gpu.test libboys)
  add_test(NAME boys.gpu COMMAND boys.gpu.test)
endif()

add_executable(libintx.benchmarks EXCLUDE_FROM_ALL libintx.benchmarks.cc)
target_link_libraries(libintx.benchmarks libintx.os)
if (TARGET libintx.libint2)
  add_definitions(-DLIBINTX_LIBINT2)
  set(libintx_test_reference libintx.libint2)
  target_link_libraries(libintx.benchmarks libintx.libint2)
endif()

if (NOT libintx_test_reference)
  set(libintx_test_reference libintx.rysq)
endif()

if (TARGET libintx.gpu.md.eri3)
  # benchmarks
  add_executable(libintx.gpu.md3.benchmarks EXCLUDE_FROM_ALL libintx.gpu.md3.benchmarks.cc)
  target_link_libraries(libintx.gpu.md3.benchmarks libintx.gpu.md.eri3)
  target_link_libraries(libintx.gpu.md3.benchmarks ${libintx_test_reference})
  # tests
  add_executable(libintx.gpu.md3.test EXCLUDE_FROM_ALL libintx.gpu.md3.test.cc)
  target_link_libraries(libintx.gpu.md3.test libintx.gpu.md.eri3)
  target_link_libraries(libintx.gpu.md3.test ${libintx_test_reference})
endif()

if (TARGET libintx.gpu.md.eri4)
  # benchmarks
  add_executable(libintx.gpu.md4.benchmarks EXCLUDE_FROM_ALL libintx.gpu.md4.benchmarks.cc)
  target_link_libraries(libintx.gpu.md4.benchmarks libintx.gpu.md.eri4)
  target_link_libraries(libintx.gpu.md4.benchmarks ${libintx_test_reference})
  # tests
  add_executable(libintx.gpu.md4.test EXCLUDE_FROM_ALL libintx.gpu.md4.test.cc)
  target_link_libraries(libintx.gpu.md4.test libintx.gpu.md.eri4)
  target_link_libraries(libintx.gpu.md4.test ${libintx_test_reference})
  add_test(NAME libintx.gpu.md4 COMMAND libintx.gpu.md4.test)
endif()

add_executable(libintx.md.test EXCLUDE_FROM_ALL libintx.md.test.cc)
target_link_libraries(libintx.md.test libboys)
add_test(NAME libintx.md COMMAND libintx.md.test)


#---Custom H20 STO-3G Basis set---

add_executable(custom_integral.test EXCLUDE_FROM_ALL
    custom_integral_test.cpp
    parse_basis_set_v2.cpp
)

target_link_libraries(custom_integral.test libintx.gpu libintx.gpu.md.eri4 libboys)
target_compile_definitions(custom_integral.test PUBLIC LIBINTX_GPU_API_CUDA)
add_test(NAME custom_integral COMMAND custom_integral.test)


