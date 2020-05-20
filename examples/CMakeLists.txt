macro(mahi_fes_example target)
    # create executable
    add_executable(${target} "ex_${target}.cpp")
    # set dependencies
    target_link_libraries(${target} mahi::fes)
    # add install rule
    install(TARGETS ${target}
      RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
    set_target_properties(${target} PROPERTIES FOLDER "Examples")
    set_target_properties(${target} PROPERTIES DEBUG_POSTFIX -d)
endmacro(mahi_fes_example)

mahi_fes_example(test_stim)
mahi_fes_example(virtual_stim)
mahi_fes_example(visualization)
mahi_fes_example(raw)