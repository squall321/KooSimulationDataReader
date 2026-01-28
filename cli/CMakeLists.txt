# CLI executable

add_executable(koo_sim_cli
    main.cpp
)

target_link_libraries(koo_sim_cli PRIVATE
    $<IF:$<BOOL:${BUILD_SHARED_LIBS}>,koo_sim,koo_sim_static>
)

set_project_warnings(koo_sim_cli)

install(TARGETS koo_sim_cli
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
