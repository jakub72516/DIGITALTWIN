cmake_minimum_required(VERSION 3.25)
project(EarthSystemsDigitalTwin VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ETWIN_BUILD_TESTS "Build test suite" ON)
option(ETWIN_BUILD_API "Build API and CLI" ON)
option(ETWIN_ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(ETWIN_ENABLE_TSAN "Enable ThreadSanitizer" OFF)
option(ETWIN_ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(MSVC)
    add_compile_options(/W4 /permissive- /Zc:__cplusplus)
else()
    add_compile_options(
        -Wall -Wextra -Wpedantic -Wshadow -Wnon-virtual-dtor
        -Wold-style-cast -Wcast-align -Wunused -Woverloaded-virtual
        -Wconversion -Wsign-conversion -Wnull-dereference
        -Wdouble-promotion -Wformat=2
    )
endif()

if(ETWIN_ENABLE_ASAN)
    if(MSVC)
        add_compile_options(/fsanitize=address)
        add_link_options(/fsanitize=address)
    else()
        add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
        add_link_options(-fsanitize=address)
    endif()
endif()

if(ETWIN_ENABLE_TSAN)
    if(NOT MSVC)
        add_compile_options(-fsanitize=thread)
        add_link_options(-fsanitize=thread)
    endif()
endif()

if(ETWIN_ENABLE_UBSAN)
    if(NOT MSVC)
        add_compile_options(-fsanitize=undefined)
        add_link_options(-fsanitize=undefined)
    endif()
endif()

include(FetchContent)

FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(json)

find_path(PROJ_INCLUDE_DIR proj.h)
if(PROJ_INCLUDE_DIR)
    message(STATUS "PROJ found: ${PROJ_INCLUDE_DIR}")
    add_library(etwin_proj_interface INTERFACE)
    target_include_directories(etwin_proj_interface INTERFACE ${PROJ_INCLUDE_DIR})
    target_compile_definitions(etwin_proj_interface INTERFACE ETWIN_HAVE_PROJ)
endif()

add_library(etwin_core
    src/core/units.cpp
    src/core/time.cpp
    src/core/crs.cpp
    src/core/provenance.cpp
    src/core/spatial.cpp
    src/core/quality.cpp
    src/core/uncertainty.cpp
)

target_include_directories(etwin_core PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(etwin_core PUBLIC nlohmann_json::nlohmann_json)
if(PROJ_INCLUDE_DIR)
    target_link_libraries(etwin_core PUBLIC etwin_proj_interface)
endif()

add_library(etwin_catalog
    src/catalog/catalog.cpp
    src/catalog/connector.cpp
    src/catalog/validation.cpp
    src/connectors/weather_connector.cpp
    src/connectors/satellite_connector.cpp
    src/connectors/ocean_connector.cpp
    src/connectors/seismic_connector.cpp
    src/connectors/ecological_connector.cpp
    src/connectors/agricultural_connector.cpp
    src/connectors/population_connector.cpp
    src/connectors/infrastructure_connector.cpp
)

target_link_libraries(etwin_catalog PUBLIC etwin_core)

add_library(etwin_state
    src/state/earth_state.cpp
    src/state/storage.cpp
    src/state/query.cpp
)

target_link_libraries(etwin_state PUBLIC etwin_core etwin_catalog)

add_library(etwin_models
    src/models/model_module.cpp
    src/models/atmosphere/simple_atmosphere.cpp
    src/models/hydrology/hbv.cpp
    src/models/ocean/layered_ocean.cpp
    src/models/cryosphere/degree_day.cpp
    src/models/soil/soil_column.cpp
    src/models/geohazard/coulomb.cpp
    src/models/ecosystem/carbon_cycle.cpp
    src/models/agriculture/crop_model.cpp
    src/models/forestry/forest_gap.cpp
    src/models/wildfire/fire_model.cpp
    src/models/population/population_model.cpp
    src/models/infrastructure/building_stock.cpp
    src/models/energy/dispatch_model.cpp
)

target_link_libraries(etwin_models PUBLIC etwin_core etwin_state)

add_library(etwin_coupling
    src/coupling/coupling_service.cpp
    src/coupling/regridding.cpp
    src/coupling/time_interpolation.cpp
)

target_link_libraries(etwin_coupling PUBLIC etwin_core etwin_models)

add_library(etwin_scenario
    src/scenario/scenario_engine.cpp
    src/scenario/ensemble.cpp
    src/scenario/manifest.cpp
    src/scenario/intervention.cpp
)

target_link_libraries(etwin_scenario PUBLIC etwin_core etwin_coupling)

add_library(etwin_validation
    src/validation/validator.cpp
    src/validation/uncertainty_propagation.cpp
    src/validation/hindcast.cpp
)

target_link_libraries(etwin_validation PUBLIC etwin_core etwin_models)

add_library(etwin_execution
    src/execution/scheduler.cpp
    src/execution/checkpoint.cpp
    src/execution/resource_monitor.cpp
)

target_link_libraries(etwin_execution PUBLIC etwin_core etwin_scenario)

add_library(etwin_security
    src/security/access_control.cpp
    src/security/audit.cpp
    src/security/input_validator.cpp
    src/security/integrity.cpp
)

target_link_libraries(etwin_security PUBLIC etwin_core)

if(ETWIN_BUILD_API)
    add_executable(etwin_cli
        src/api/cli.cpp
        src/api/rest.cpp
        src/api/main.cpp
    )
    target_link_libraries(etwin_cli PRIVATE
        etwin_core etwin_catalog etwin_state etwin_models
        etwin_coupling etwin_scenario etwin_validation
        etwin_execution etwin_security
    )
endif()

if(ETWIN_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
