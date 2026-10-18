include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.5.0
    GIT_SHALLOW TRUE
)
set(CATCH_INSTALL_DOCS OFF CACHE BOOL "" FORCE)
set(CATCH_INSTALL_EXTRAS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(Catch2)

add_executable(etwin_tests
    component/test_units.cpp
    component/test_time.cpp
    component/test_provenance.cpp
    component/test_spatial.cpp
    component/test_uncertainty.cpp
    integration/test_coupling.cpp
    integration/test_scenario.cpp
    validation/test_validator.cpp
    reproducibility/test_reproducibility.cpp
)

target_link_libraries(etwin_tests PRIVATE
    Catch2::Catch2WithMain
    etwin_core etwin_catalog etwin_state etwin_models
    etwin_coupling etwin_scenario etwin_validation
    etwin_execution etwin_security
)

include(Catch)
catch_discover_tests(etwin_tests)
