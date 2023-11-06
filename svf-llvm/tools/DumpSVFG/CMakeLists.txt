if(DEFINED IN_SOURCE_BUILD)
  add_llvm_tool(dump-svfg.cpp)
else()
  add_executable(dump-svfg dump-svfg.cpp)

  target_link_libraries(dump-svfg SvfLLVM ${llvm_libs})

  set_target_properties(dump-svfg PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                          ${CMAKE_BINARY_DIR}/bin)
endif()

install(TARGETS dump-svfg DESTINATION bin)