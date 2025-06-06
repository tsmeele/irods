# create phony target to build everything in server folder
add_custom_target(all-server)

configure_file(
  "${CMAKE_IRODS_SOURCE_DIR}/cmake/paths_cmake.py.in"
  "${CMAKE_IRODS_BINARY_DIR}/scripts/irods/paths_cmake.py"
  @ONLY
)

# create irods_server target first, as it's needed in executables
add_library(
  irods_server
  SHARED
)

add_subdirectory(core)
add_subdirectory(genquery2)
add_subdirectory(api)
add_subdirectory(icat)
add_subdirectory(re)
add_subdirectory(drivers)
add_subdirectory(auth)
add_subdirectory(main_server)
add_subdirectory(hostname_resolves_to_local_address)
add_subdirectory(delay_server)
add_subdirectory(harness)

target_link_objects(
  irods_server
  PUBLIC
  irods_lib_api_server
  irods_lib_core_server
  irods_lib_metadata_server
  irods_server_api
  irods_server_core
  irods_filesystem_path
  irods_filesystem_client
  irods_filesystem_server
  irods_genquery2_parser
  irods_user_administration_client
  irods_user_administration_server
  irods_resource_administration_client
  irods_resource_administration_server
  irods_ticket_administration_client
  irods_ticket_administration_server
  irods_zone_administration_client
  irods_zone_administration_server
  irods_server_drivers
  irods_server_icat
  irods_server_re
)
target_link_libraries(
  irods_server
  PRIVATE
  "${IRODS_EXTERNALS_FULLPATH_BOOST}/lib/libboost_chrono.so"
  "${IRODS_EXTERNALS_FULLPATH_BOOST}/lib/libboost_filesystem.so"
  "${IRODS_EXTERNALS_FULLPATH_BOOST}/lib/libboost_thread.so"
  "${IRODS_EXTERNALS_FULLPATH_BOOST}/lib/libboost_regex.so"
  "${IRODS_EXTERNALS_FULLPATH_BOOST}/lib/libboost_system.so"
  "${IRODS_EXTERNALS_FULLPATH_NANODBC}/lib/libnanodbc.so"
  OpenSSL::SSL
  OpenSSL::Crypto
  ${CMAKE_DL_LIBS}
  rt
)
set_property(TARGET irods_server PROPERTY VERSION ${IRODS_VERSION})
set_property(TARGET irods_server PROPERTY SOVERSION ${IRODS_VERSION})

target_deduplicate_interface_properties(
  irods_server
  COMPILE_DEFINITIONS
  INCLUDE_DIRECTORIES
  LINK_LIBRARIES
)

install(
  TARGETS
  irods_server
  EXPORT IRODSTargets
  LIBRARY
  DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  COMPONENT ${IRODS_PACKAGE_COMPONENT_RUNTIME_NAME}
  NAMELINK_COMPONENT ${IRODS_PACKAGE_COMPONENT_DEVELOPMENT_NAME}
)

set(
  IRODS_CONFIGURATION_SCHEMA_FILES
  version.json
  client_environment.json
  client_hints.json
  configuration_directory.json
  database_config.json
  host_access_control.json
  host_resolution.json
  plugin.json
  resource.json
  rule_engine.json
  server.json
  server_config.json
  service_account_environment.json
  unattended_installation.json
  zone_bundle.json
)
foreach(SCHEMA_FILE IN LISTS IRODS_CONFIGURATION_SCHEMA_FILES)
  configure_file(
    "${CMAKE_IRODS_SOURCE_DIR}/schemas/configuration/v${IRODS_CONFIGURATION_SCHEMA_VERSION}/${SCHEMA_FILE}.in"
    "${CMAKE_IRODS_BINARY_DIR}/schemas/configuration/v${IRODS_CONFIGURATION_SCHEMA_VERSION}/${SCHEMA_FILE}"
    @ONLY
  )
  install(
    FILES
    "${CMAKE_IRODS_BINARY_DIR}/schemas/configuration/v${IRODS_CONFIGURATION_SCHEMA_VERSION}/${SCHEMA_FILE}"
    DESTINATION "${IRODS_HOME_DIRECTORY}/configuration_schemas/v${IRODS_CONFIGURATION_SCHEMA_VERSION}"
    COMPONENT ${IRODS_PACKAGE_COMPONENT_SERVER_NAME}
  )
endforeach()

install(
  DIRECTORY
  DESTINATION "${IRODS_HOME_DIRECTORY}/config/lockFileDir"
  COMPONENT ${IRODS_PACKAGE_COMPONENT_SERVER_NAME}
)

install(
  FILES "${CMAKE_IRODS_BINARY_DIR}/version.json.dist"
  DESTINATION "${IRODS_HOME_DIRECTORY}"
  COMPONENT ${IRODS_PACKAGE_COMPONENT_SERVER_NAME}
)

install(
  DIRECTORY
  DESTINATION "${IRODS_HOME_DIRECTORY}/log"
  COMPONENT ${IRODS_PACKAGE_COMPONENT_SERVER_NAME}
)

install(
  DIRECTORY
  DESTINATION "${CMAKE_INSTALL_SYSCONFDIR}/irods"
  COMPONENT ${IRODS_PACKAGE_COMPONENT_SERVER_NAME}
)

install(
  DIRECTORY
  DESTINATION "${CMAKE_INSTALL_RUNSTATEDIR}/irods"
  DIRECTORY_PERMISSIONS
    OWNER_EXECUTE OWNER_WRITE OWNER_READ
    GROUP_EXECUTE GROUP_WRITE GROUP_READ
    WORLD_READ
  COMPONENT ${IRODS_PACKAGE_COMPONENT_SERVER_NAME}
)

install(
  FILES
  "${CMAKE_IRODS_SOURCE_DIR}/README.md"
  DESTINATION "${IRODS_DOC_DIR}"
  COMPONENT ${IRODS_PACKAGE_COMPONENT_SERVER_NAME}
  RENAME readme
)

install(
  FILES
  "${CMAKE_IRODS_SOURCE_DIR}/LICENSE"
  DESTINATION "${IRODS_DOC_DIR}"
  COMPONENT ${IRODS_PACKAGE_COMPONENT_SERVER_NAME}
)

configure_file(
  "${CMAKE_IRODS_SOURCE_DIR}/packaging/server_config.json.template.in"
  "${CMAKE_IRODS_BINARY_DIR}/packaging/server_config.json.template"
  @ONLY
)
install(
  FILES
  "${CMAKE_IRODS_SOURCE_DIR}/packaging/connectControl.config.template"
  "${CMAKE_IRODS_SOURCE_DIR}/packaging/core.dvm.template"
  "${CMAKE_IRODS_SOURCE_DIR}/packaging/core.fnm.template"
  "${CMAKE_IRODS_SOURCE_DIR}/packaging/core.re.template"
  "${CMAKE_IRODS_SOURCE_DIR}/packaging/irodsMonPerf.config.in"
  "${CMAKE_IRODS_BINARY_DIR}/packaging/server_config.json.template"
  "${CMAKE_IRODS_SOURCE_DIR}/packaging/server_setup_instructions.txt"
  "${CMAKE_IRODS_SOURCE_DIR}/packaging/server_upgrade_instructions.txt"
  DESTINATION "${IRODS_HOME_DIRECTORY}/packaging"
  COMPONENT ${IRODS_PACKAGE_COMPONENT_SERVER_NAME}
  PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
)

if (
  (IRODS_LINUX_DISTRIBUTION_NAME STREQUAL "debian" AND IRODS_LINUX_DISTRIBUTION_VERSION_MAJOR LESS 13) OR
  (IRODS_LINUX_DISTRIBUTION_NAME STREQUAL "ubuntu" AND IRODS_LINUX_DISTRIBUTION_VERSION_MAJOR LESS 24) OR
  (IRODS_LINUX_DISTRIBUTION_NAME STREQUAL "rhel" AND IRODS_LINUX_DISTRIBUTION_VERSION_MAJOR LESS 9)
)
  install(
    FILES
    "${CMAKE_IRODS_SOURCE_DIR}/packaging/irods.service.pre-253.template"
    DESTINATION "${IRODS_HOME_DIRECTORY}/packaging"
    COMPONENT ${IRODS_PACKAGE_COMPONENT_SERVER_NAME}
    PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
    RENAME "irods.service.template"
  )
else()
  install(
    FILES
    "${CMAKE_IRODS_SOURCE_DIR}/packaging/irods.service.template"
    DESTINATION "${IRODS_HOME_DIRECTORY}/packaging"
    COMPONENT ${IRODS_PACKAGE_COMPONENT_SERVER_NAME}
    PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
  )
endif()

install(
  FILES
  "${CMAKE_IRODS_SOURCE_DIR}/packaging/find_os.sh"
  "${CMAKE_IRODS_SOURCE_DIR}/packaging/postinstall.sh"
  "${CMAKE_IRODS_SOURCE_DIR}/packaging/preremove.sh"
  DESTINATION "${IRODS_HOME_DIRECTORY}/packaging"
  COMPONENT ${IRODS_PACKAGE_COMPONENT_SERVER_NAME}
  PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ WORLD_READ
)

install(
  DIRECTORY "${CMAKE_IRODS_SOURCE_DIR}/scripts"
  DESTINATION "${IRODS_HOME_DIRECTORY}"
  USE_SOURCE_PERMISSIONS
  COMPONENT ${IRODS_PACKAGE_COMPONENT_SERVER_NAME}
  PATTERN "__pycache__" EXCLUDE
  PATTERN "*.pyc" EXCLUDE
  PATTERN "*.pyd" EXCLUDE
  PATTERN "*.pyo" EXCLUDE
)

install(
  FILES
  "${CMAKE_IRODS_BINARY_DIR}/scripts/irods/paths_cmake.py"
  DESTINATION "${IRODS_HOME_DIRECTORY}/scripts/irods"
  COMPONENT ${IRODS_PACKAGE_COMPONENT_SERVER_NAME}
)

install(
  FILES
  "${CMAKE_IRODS_SOURCE_DIR}/msiExecCmd_bin/test_execstream.py"
  "${CMAKE_IRODS_SOURCE_DIR}/msiExecCmd_bin/hello.template"
  DESTINATION "${IRODS_HOME_DIRECTORY}/msiExecCmd_bin"
  COMPONENT ${IRODS_PACKAGE_COMPONENT_SERVER_NAME}
  PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ WORLD_READ
)

install(
  FILES
  "${CMAKE_IRODS_SOURCE_DIR}/msiExecCmd_bin/irodsServerMonPerf.template"
  "${CMAKE_IRODS_SOURCE_DIR}/msiExecCmd_bin/univMSSInterface.sh.template"
  DESTINATION "${IRODS_HOME_DIRECTORY}/msiExecCmd_bin"
  COMPONENT ${IRODS_PACKAGE_COMPONENT_SERVER_NAME}
  PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
)

install(
  FILES
  "${CMAKE_IRODS_SOURCE_DIR}/test/test_framework_configuration.json"
  DESTINATION "${IRODS_HOME_DIRECTORY}/test"
  COMPONENT ${IRODS_PACKAGE_COMPONENT_SERVER_NAME}
)

install(
  DIRECTORY "${CMAKE_IRODS_SOURCE_DIR}/test/filesystem"
  DESTINATION "${IRODS_HOME_DIRECTORY}/test"
  COMPONENT ${IRODS_PACKAGE_COMPONENT_SERVER_NAME}
)
