cmake_minimum_required(VERSION 3.12...3.18 FATAL_ERROR)
if (POLICY CMP0109)
  # find_program requires permission to execute but not to read
  cmake_policy(SET CMP0109 NEW)
endif()
if (POLICY CMP0121)
  # Detect invalid indices in list()
  cmake_policy(SET CMP0121 NEW)
endif()
if (POLICY CMP0125)
  # Consistent behavior for cache variables managed by find_*()
  cmake_policy(SET CMP0125 NEW)
endif()
if (POLICY CMP0130)
  # Diagnose condition evaluation errors in while()
  cmake_policy(SET CMP0130 NEW)
endif()
if (POLICY CMP0132)
  # Consistent handling of compiler environment variables
  cmake_policy(SET CMP0132 NEW)
endif()
if (POLICY CMP0140)
  # Check validity of return() params
  cmake_policy(SET CMP0140 NEW)
endif()

list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")
include(RequireOutOfSourceBuild)

option(IRODS_DISABLE_COMPILER_OPTIMIZATIONS "Disables compiler optimizations by setting -O0." OFF)
option(IRODS_ENABLE_ADDRESS_SANITIZER "Enables detection of memory leaks and other features provided by Address Sanitizer." OFF)
option(IRODS_ENABLE_UNDEFINED_BEHAVIOR_SANITIZER "Enables detection of undefined behavior provided by Undefined Behavior Sanitizer." OFF)
option(IRODS_ENABLE_UNDEFINED_BEHAVIOR_SANITIZER_IMPLICIT_CONVERSION_CHECK "Enables detection of implicit conversions by Undefined Behavior Sanitizer." OFF)
option(IRODS_ENABLE_ALL_TESTS "Enables all tests (e.g. C/C++ unit tests, microservices specifically designed for testing)." OFF)

if (NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build {Debug, Release}." FORCE)
  message(STATUS "Setting unspecified CMAKE_BUILD_TYPE to '${CMAKE_BUILD_TYPE}'. This is the correct setting for normal builds.")
endif()

include(IrodsExternals)
include(IrodsCXXCompiler)

IRODS_MACRO_CHECK_DEPENDENCY_SET_FULLPATH_ADD_TO_IRODS_PACKAGE_DEPENDENCIES_LIST(BOOST boost1.81.0-2)
IRODS_MACRO_CHECK_DEPENDENCY_SET_FULLPATH_ADD_TO_IRODS_PACKAGE_DEPENDENCIES_LIST(NANODBC nanodbc2.13.0-3)
IRODS_MACRO_CHECK_DEPENDENCY_SET_FULLPATH_ADD_TO_IRODS_DEVELOP_DEPENDENCIES_LIST(JSONCONS jsoncons0.178.0-0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(IRODS_CXX_STANDARD "${CMAKE_CXX_STANDARD}")

# export-dynamic so stacktrace entries from executables have function names
set(CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,--export-dynamic -Wl,--enable-new-dtags -Wl,--as-needed")
set(CMAKE_MODULE_LINKER_FLAGS_INIT "-Wl,--enable-new-dtags -Wl,--as-needed")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "-Wl,--enable-new-dtags -Wl,--as-needed")

if (IRODS_ENABLE_ADDRESS_SANITIZER)
  # The following options are compiled into all binaries and allow them to start even
  # when an ODR violation is detected.
  #
  # Make sure the correct llvm-symbolizer binary is available to Address Sanitizer. This binary
  # allows debug symbols to be reported appropriately. There are two ways to do this:
  #
  #     export PATH=/opt/irods-externals/clang13.0.0-0/bin:$PATH
  #
  # - or -
  #
  #     export ASAN_SYMBOLIZER_PATH=/opt/irods-externals/clang13.0.0-0/bin/llvm-symbolizer
  #
  add_compile_definitions(IRODS_ADDRESS_SANITIZER_DEFAULT_OPTIONS="log_path=/tmp/irods_asan_output:detect_odr_violation=1")
  add_compile_options(-fsanitize=address)
  add_link_options(-fsanitize=address)
endif()

if (IRODS_ENABLE_UNDEFINED_BEHAVIOR_SANITIZER)
  # Make sure the correct llvm-symbolizer binary is available to Undefined Behavior Sanitizer. This binary
  # allows debug symbols to be reported appropriately. Additionally, set options for logging.
  #
  #     export PATH=/opt/irods-externals/clang13.0.1-0/bin:$PATH
  add_compile_definitions(IRODS_UNDEFINED_BEHAVIOR_SANITIZER_DEFAULT_OPTIONS="log_path=/tmp/irods_ubsan_output")

  add_compile_options(
    -fsanitize=undefined
    -fsanitize=float-divide-by-zero
    -fsanitize=unsigned-integer-overflow
    -fsanitize=local-bounds
    -fsanitize=nullability
    -fsanitize-ignorelist=${CMAKE_CURRENT_SOURCE_DIR}/ub-sanitizer-ignorelist.txt)
  add_link_options(
    -fsanitize=undefined
    -fsanitize=float-divide-by-zero
    -fsanitize=unsigned-integer-overflow
    -fsanitize=local-bounds
    -fsanitize=nullability)

  if (IRODS_ENABLE_UNDEFINED_BEHAVIOR_SANITIZER_IMPLICIT_CONVERSION_CHECK)
     add_compile_options(-fsanitize=implicit-conversion)
     add_link_options(-fsanitize=implicit-conversion)
  endif()
endif()

if (IRODS_ENABLE_ADDRESS_SANITIZER OR IRODS_ENABLE_UNDEFINED_BEHAVIOR_SANITIZER)
  add_compile_options(
    -fno-omit-frame-pointer
    -fno-optimize-sibling-calls
    -O1)
  add_link_options(
    -fno-omit-frame-pointer
    -fno-optimize-sibling-calls
    -O1)
else()
  set(CMAKE_EXE_LINKER_FLAGS_INIT "${CMAKE_EXE_LINKER_FLAGS_INIT} -Wl,-z,defs")
  set(CMAKE_MODULE_LINKER_FLAGS_INIT "${CMAKE_MODULE_LINKER_FLAGS_INIT} -Wl,-z,defs")
  set(CMAKE_SHARED_LINKER_FLAGS_INIT "${CMAKE_SHARED_LINKER_FLAGS_INIT} -Wl,-z,defs")
endif()

set(CMAKE_EXE_LINKER_FLAGS_RELEASE_INIT "-Wl,--gc-sections -Wl,-z,combreloc")
set(CMAKE_MODULE_LINKER_FLAGS_RELEASE_INIT "-Wl,--gc-sections -Wl,-z,combreloc")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE_INIT "-Wl,--gc-sections -Wl,-z,combreloc")
include(IrodsRunpathDefaults)

set(IRODS_VERSION_MAJOR "4")
set(IRODS_VERSION_MINOR "90")
set(IRODS_VERSION_PATCH "0")
set(IRODS_VERSION "${IRODS_VERSION_MAJOR}.${IRODS_VERSION_MINOR}.${IRODS_VERSION_PATCH}")
set(IRODS_PACKAGE_REVISION "0")

# Consider bumping this variable anytime the following requirements are satisfied:
# - The catalog schema has been modified
# - The next release involves a change in the major version number or backwards compatibility cannot be maintained
set(IRODS_CATALOG_SCHEMA_VERSION "12")
# Consider bumping this variable anytime the following requirements are satisfied:
# - A JSON schema has been modified
# - The next release involves a change in the major version number or backwards compatibility cannot be maintained
set(IRODS_CONFIGURATION_SCHEMA_VERSION "5")

project(irods
  VERSION ${IRODS_VERSION}
  LANGUAGES C CXX)

include(ObjectTargetHelpers)
include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)

if (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
  set(IRODS_BUILD_WITH_WERROR_DEFAULT ON)
else()
  set(IRODS_BUILD_WITH_WERROR_DEFAULT OFF)
endif()

set(IRODS_BUILD_WITH_WERROR ${IRODS_BUILD_WITH_WERROR_DEFAULT} CACHE BOOL "Choose whether to compile with -Werror.")

if (IRODS_BUILD_WITH_WERROR)
  add_compile_options(-Werror)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/InstallDirs.cmake)

if (NOT DEFINED CMAKE_POSITION_INDEPENDENT_CODE)
  set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
endif()

if (IRODS_DISABLE_COMPILER_OPTIMIZATIONS)
  add_compile_options(-O0)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fpermissive>)
endif()

add_compile_options(-Wall -Wextra)

check_cxx_compiler_flag(-Wno-write-strings can_use_wwrite_strings_cxx)
if (can_use_wwrite_strings_cxx)
  add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wno-write-strings>)
endif()
check_c_compiler_flag(-Wno-unused-function can_use_wunused_function_c)
check_cxx_compiler_flag(-Wno-unused-function can_use_wunused_function_cxx)
if (can_use_wunused_function_c)
  add_compile_options($<$<COMPILE_LANGUAGE:C>:-Wno-unused-function>)
endif()
if (can_use_wunused_function_cxx)
  add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wno-unused-function>)
endif()
check_c_compiler_flag(-Wno-unused-parameter can_use_wunused_parameter_c)
check_cxx_compiler_flag(-Wno-unused-parameter can_use_wunused_parameter_cxx)
if (can_use_wunused_parameter_c)
  add_compile_options($<$<COMPILE_LANGUAGE:C>:-Wno-unused-parameter>)
endif()
if (can_use_wunused_parameter_cxx)
  add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wno-unused-parameter>)
endif()
check_c_compiler_flag(-Wno-parentheses can_use_wparentheses_c)
check_cxx_compiler_flag(-Wno-parentheses can_use_wparentheses_cxx)
if (can_use_wparentheses_c)
  add_compile_options($<$<COMPILE_LANGUAGE:C>:-Wno-parentheses>)
endif()
if (can_use_wparentheses_cxx)
  add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wno-parentheses>)
endif()
check_cxx_compiler_flag(-Wno-ignored-qualifiers can_use_wignored_qualifiers_cxx)
check_c_compiler_flag(-Wno-ignored-qualifiers can_use_wignored_qualifiers_c)
if (can_use_wignored_qualifiers_c)
  add_compile_options($<$<COMPILE_LANGUAGE:C>:-Wno-ignored-qualifiers>)
endif()
if (can_use_wignored_qualifiers_cxx)
  add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wno-ignored-qualifiers>)
endif()

if (IRODS_BUILD_WITH_WERROR)
  check_c_compiler_flag(-Wno-error=unused-variable can_use_wunused_variable_c)
  check_cxx_compiler_flag(-Wno-error=unused-variable can_use_wunused_variable_cxx)
  if (can_use_wunused_variable_c)
    add_compile_options($<$<COMPILE_LANGUAGE:C>:-Wno-error=unused-variable>)
  endif()
  if (can_use_wunused_variable_cxx)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wno-error=unused-variable>)
  endif()
  check_c_compiler_flag(-Wno-error=unused-but-set-variable can_use_wunused_but_set_variable_c)
  check_cxx_compiler_flag(-Wno-error=unused-but-set-variable can_use_wunused_but_set_variable_cxx)
  if (can_use_wunused_but_set_variable_c)
    add_compile_options($<$<COMPILE_LANGUAGE:C>:-Wno-error=unused-but-set-variable>)
  endif()
  if (can_use_wunused_but_set_variable_cxx)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wno-error=unused-but-set-variable>)
  endif()
  # Do not error on definition of implicit copy constructor for classes with a
  # user-declared copy assignment operator.
  # This is needed because the boost headers throw this warning.
  check_cxx_compiler_flag(-Wno-error=deprecated-copy can_use_wdeprecated_copy_cxx)
  if (can_use_wdeprecated_copy_cxx)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wno-error=deprecated-copy>)
  endif()
  # Do not error when overlapping comparisons always evaluate to false.
  # This is needed because the boost headers throw this warning.
  check_c_compiler_flag(-Wno-error=tautological-overlap-compare can_use_wtautological_overlap_compare_c)
  check_cxx_compiler_flag(-Wno-error=tautological-overlap-compare can_use_wtautological_overlap_compare_cxx)
  if (can_use_wtautological_overlap_compare_c)
    add_compile_options($<$<COMPILE_LANGUAGE:C>:-Wno-error=tautological-overlap-compare>)
  endif()
  if (can_use_wtautological_overlap_compare_cxx)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wno-error=tautological-overlap-compare>)
  endif()
endif()

find_package(nlohmann_json "3.6.1" REQUIRED)
if (CMAKE_VERSION VERSION_LESS "3.19.0")
  find_package(fmt "8.1.1" REQUIRED)
else()
  find_package(fmt 8.1.1...<10.0.0 REQUIRED)
endif()
find_package(spdlog "1.9.2" REQUIRED)
find_package(LibArchive "3.3.1" REQUIRED)

if (TARGET fmt::fmt-header-only AND NOT TARGET fmt::fmt)
  add_library(fmt::fmt ALIAS fmt::fmt-header-only)
endif()
if (TARGET spdlog::spdlog_header_only AND NOT TARGET spdlog::spdlog)
  add_library(spdlog::spdlog ALIAS spdlog::spdlog_header_only)
endif()

if (NOT DEFINED THREADS_PREFER_PTHREAD_FLAG)
  set(THREADS_PREFER_PTHREAD_FLAG TRUE)
endif()
find_package(Threads REQUIRED)
find_package(CURL REQUIRED)

find_package(OpenSSL REQUIRED COMPONENTS Crypto SSL)

set(IRODS_USE_LIBSYSTEMD_DOCSTRING "Use libsystemd for service manager status communication")
if (NOT DEFINED IRODS_USE_LIBSYSTEMD)
	# if IRODS_USE_LIBSYSTEMD isn't set, check for systemd and set default value based on its presence.
	find_package(systemd)
	set(IRODS_USE_LIBSYSTEMD "${SYSTEMD_FOUND}" CACHE BOOL "${IRODS_USE_LIBSYSTEMD_DOCSTRING}")
else()
	set(IRODS_USE_LIBSYSTEMD "${IRODS_USE_LIBSYSTEMD}" CACHE BOOL "${IRODS_USE_LIBSYSTEMD_DOCSTRING}")
	if (IRODS_USE_LIBSYSTEMD)
		find_package(systemd REQUIRED)
	endif()
endif()

include(IrodsCPackPlatform)

set(IRODS_PLATFORM_STRING linux_platform)

set(
  IRODS_COMPILE_DEFINITIONS_PUBLIC
  ${IRODS_PLATFORM_STRING}
  _LARGEFILE_SOURCE
  _FILE_OFFSET_BITS=64
  _LARGE_FILES
  _LARGEFILE64_SOURCE
)
set(
  IRODS_COMPILE_DEFINITIONS_PRIVATE
  BOOST_SYSTEM_NO_DEPRECATED
  BOOST_CHRONO_DONT_PROVIDES_DEPRECATED_IO_SINCE_V2_0_0
  BOOST_STACKTRACE_USE_ADDR2LINE
  BOOST_ASIO_NO_DEPRECATED
  OPENSSL_NO_DEPRECATED_3_0
  SPDLOG_FMT_EXTERNAL
  SPDLOG_NO_TLS
)

if (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "15.0" AND NOT IRODS_BUILD_AGAINST_LIBCXX)
  message(AUTHOR_WARNING "Adding `consteval=constexpr` to `IRODS_COMPILE_DEFINITIONS_PRIVATE` to work around Clang bug with libstdc++ 13.")
  list(APPEND IRODS_COMPILE_DEFINITIONS_PRIVATE "consteval=constexpr")
endif()

# Define our component names here so they're defined everywhere
set(IRODS_PACKAGE_COMPONENT_RUNTIME_NAME "irods-runtime")
set(IRODS_PACKAGE_COMPONENT_SERVER_NAME "irods-server")
set(IRODS_PACKAGE_COMPONENT_DEVELOPMENT_NAME "irods-dev")
set(IRODS_PACKAGE_COMPONENT_POSTGRES_NAME "irods-database-plugin-postgres")
set(IRODS_PACKAGE_COMPONENT_MYSQL_NAME "irods-database-plugin-mysql")
set(IRODS_PACKAGE_COMPONENT_ORACLE_NAME "irods-database-plugin-oracle")

set(CMAKE_IRODS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(CMAKE_IRODS_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")

include(GetGitRevisionDescription)
get_git_head_revision(IRODS_GIT_REFSPEC IRODS_GIT_SHA1)
configure_file(
  "${CMAKE_IRODS_SOURCE_DIR}/version.json.dist.in"
  "${CMAKE_IRODS_BINARY_DIR}/version.json.dist"
  @ONLY
)

add_subdirectory(lib)
add_subdirectory(server)
add_subdirectory(plugins)
add_subdirectory(test/c_api_test)
add_subdirectory(test/post_install_test)
add_subdirectory(unit_tests)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/packaging.cmake")

include(CMakePackageConfigHelpers)
set(IRODS_INSTALL_CMAKECONFDIR "${IRODS_INSTALL_CMAKEDIR}/irods-${IRODS_VERSION_MAJOR}.${IRODS_VERSION_MINOR}")

configure_package_config_file(
  "${CMAKE_IRODS_SOURCE_DIR}/cmake/IRODSConfig.cmake.not_yet_installed.in"
  "${CMAKE_IRODS_BINARY_DIR}/IRODSConfig.cmake.not_yet_installed" # suffix prevents cmake's find_package() from using this copy of the file
  INSTALL_DESTINATION "${IRODS_INSTALL_CMAKECONFDIR}"
  PATH_VARS IRODS_INCLUDE_DIRS
)
configure_file(
  "${CMAKE_IRODS_SOURCE_DIR}/cmake/IRODSConfigVersion.cmake.in"
  "${CMAKE_IRODS_BINARY_DIR}/IRODSConfigVersion.cmake"
  @ONLY
)

install(
  FILES
  "${CMAKE_IRODS_BINARY_DIR}/IRODSConfig.cmake.not_yet_installed"
  DESTINATION "${IRODS_INSTALL_CMAKECONFDIR}"
  COMPONENT ${IRODS_PACKAGE_COMPONENT_DEVELOPMENT_NAME}
  RENAME IRODSConfig.cmake
)

install(
  FILES
  "${CMAKE_IRODS_BINARY_DIR}/IRODSConfigVersion.cmake"
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/IRODSTargetsWrapper.cmake"
  DESTINATION "${IRODS_INSTALL_CMAKECONFDIR}"
  COMPONENT ${IRODS_PACKAGE_COMPONENT_DEVELOPMENT_NAME}
)

install(
  FILES
  "${CMAKE_IRODS_SOURCE_DIR}/cmake/Modules/RequireOutOfSourceBuild.cmake"
  "${CMAKE_IRODS_SOURCE_DIR}/cmake/Modules/GetGitRevisionDescription.cmake"
  "${CMAKE_IRODS_SOURCE_DIR}/cmake/Modules/GetGitRevisionDescription.cmake.in"
  "${CMAKE_IRODS_SOURCE_DIR}/cmake/Modules/CheckCPPMacroDefinition.cmake"
  "${CMAKE_IRODS_SOURCE_DIR}/cmake/Modules/CheckCPPMacroDefinition.c.in"
  "${CMAKE_IRODS_SOURCE_DIR}/cmake/Modules/FindCatch2.cmake"
  "${CMAKE_IRODS_SOURCE_DIR}/cmake/Modules/Findnlohmann_json.cmake"
  "${CMAKE_IRODS_SOURCE_DIR}/cmake/Modules/Findsystemd.cmake"
  "${CMAKE_IRODS_SOURCE_DIR}/cmake/Modules/IrodsExternals.cmake"
  "${CMAKE_IRODS_SOURCE_DIR}/cmake/Modules/IrodsCXXCompiler.cmake"
  "${CMAKE_IRODS_SOURCE_DIR}/cmake/Modules/ObjectTargetHelpers.cmake"
  "${CMAKE_IRODS_SOURCE_DIR}/cmake/Modules/IrodsCPackPlatform.cmake"
  "${CMAKE_IRODS_SOURCE_DIR}/cmake/Modules/IrodsCPackCommon.cmake"
  "${CMAKE_IRODS_SOURCE_DIR}/cmake/Modules/IrodsRunpathDefaults.cmake"
  DESTINATION "${IRODS_INSTALL_CMAKECONFDIR}/Modules"
  COMPONENT ${IRODS_PACKAGE_COMPONENT_DEVELOPMENT_NAME}
)

install(
  FILES
  "${CMAKE_IRODS_SOURCE_DIR}/scripts/irods/distro_distill.py"
  DESTINATION "${IRODS_INSTALL_CMAKECONFDIR}/Modules"
  COMPONENT ${IRODS_PACKAGE_COMPONENT_DEVELOPMENT_NAME}
  PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

install(
  EXPORT
  IRODSTargets
  DESTINATION "${IRODS_INSTALL_CMAKECONFDIR}"
  COMPONENT ${IRODS_PACKAGE_COMPONENT_DEVELOPMENT_NAME}
)

foreach(DATABASE_PLUGIN postgres oracle mysql)
  string(TOUPPER ${DATABASE_PLUGIN} DATABASE_PLUGIN_UPPER)
  add_custom_target(
    non-package-install-${DATABASE_PLUGIN}
    COMMAND
    "${CMAKE_COMMAND}" -DCMAKE_INSTALL_COMPONENT=${IRODS_PACKAGE_COMPONENT_SERVER_NAME} -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_install.cmake"
    COMMAND
    "${CMAKE_COMMAND}" -DCMAKE_INSTALL_COMPONENT=${IRODS_PACKAGE_COMPONENT_DEVELOPMENT_NAME} -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_install.cmake"
    COMMAND
    "${CMAKE_COMMAND}" -DCMAKE_INSTALL_COMPONENT=${IRODS_PACKAGE_COMPONENT_RUNTIME_NAME} -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_install.cmake"
    COMMAND
    "${CMAKE_COMMAND}" -DCMAKE_INSTALL_COMPONENT=${IRODS_PACKAGE_COMPONENT_${DATABASE_PLUGIN_UPPER}_NAME} -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_install.cmake"
  )
  add_dependencies(
    non-package-install-${DATABASE_PLUGIN}
    all-lib
    all-server
    all-plugins_no_database
    irods_database_plugin-${DATABASE_PLUGIN}
    IRODS_PHONY_TARGET_icatSysTables_${DATABASE_PLUGIN}.sql
    irodsTestPutGet
  )
  if (IRODS_UNIT_TESTS_BUILD)
    add_dependencies(
      non-package-install-${DATABASE_PLUGIN}
      all-unit_tests
    )
  endif()
endforeach()

include(CPack)
