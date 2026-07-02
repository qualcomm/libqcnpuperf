cmake_minimum_required(VERSION 3.21)

project(qcnpuperf 
	VERSION 1.0
	DESCRIPTION "Qualcomm NPU Stats"
	LANGUAGES C
	)

# ------------------------------------------------------------
# Options / paths
# ------------------------------------------------------------
option(QCNPU_PERF_BUILD_CLI "Build ncurses-based CLI sample (qcnpuperf_cli)" ON)

# Allow overriding FastRPC paths at configure time
find_path(FASTRPC_INCLUDE_DIR AEEStdDef.h
    PATHS /usr/include/fastrpc /usr/local/include/fastrpc /usr/include/aarch64-linux-gnu
)

find_library(FASTRPC_LIBRARY NAMES cdsprpc
    PATHS /usr/lib /usr/local/lib /usr/lib/aarch64-linux-gnu/
)
# Install locations (can be overridden via CMAKE_INSTALL_PREFIX)
include(GNUInstallDirs)

# ------------------------------------------------------------
# Library
# ------------------------------------------------------------

add_library(qcnpuperf SHARED
    src/qcom_dsp.c
    src/dspquery_stub.c
)

set_target_properties(qcnpuperf PROPERTIES
    OUTPUT_NAME qcnpuperf
    VERSION 1.0
    SOVERSION 1
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/qcnpuperf.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/qcnpuperf.pc
    @ONLY
)

# ------------------------------------------------------------
# Include paths
# ------------------------------------------------------------

target_include_directories(qcnpuperf
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        ${FASTRPC_INCLUDE_DIR}
)

# ------------------------------------------------------------
# Link libraries
# ------------------------------------------------------------

target_link_directories(qcnpuperf
    PRIVATE
        /usr/local/lib
)

target_link_libraries(qcnpuperf
    PRIVATE
        ${FASTRPC_LIBRARY}
)

# ------------------------------------------------------------
# Compiler flags (optional but typical for shared libs)
# ------------------------------------------------------------

target_compile_options(qcnpuperf PRIVATE
    -fPIC
)

# ------------------------------------------------------------
# Optional sample app (ncurses)
# ------------------------------------------------------------
if(QCNPU_PERF_BUILD_CLI)
  find_package(Curses QUIET)

  if(CURSES_FOUND)
    add_executable(qcnpuperf_cli examples/qcnpuperf_cli.c)

    target_include_directories(qcnpuperf_cli PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/inc
      ${CURSES_INCLUDE_DIRS}
      ${FASTRPC_INCLUDE_DIR}
    )

    target_link_libraries(qcnpuperf_cli PRIVATE
      qcnpuperf
      ${CURSES_LIBRARIES}
    )

    # Optional: install the sample binary
    install(TARGETS qcnpuperf_cli RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
  else()
    message(STATUS "ncurses (Curses) not found; skipping qcnpuperf_cli. Install libncurses-dev or set -DQCNPU_PERF_BUILD_CLI=OFF")
  endif()
endif()

# ------------------------------------------------------------
# Install rules
# ------------------------------------------------------------

install(TARGETS qcnpuperf
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/qcnpuperf.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

install(FILES
    inc/qcom_dsp.h
    inc/qcom_dsp_types.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
