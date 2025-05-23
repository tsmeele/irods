set(
  IRODS_LIB_FILESYSTEM_PATH_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/src/path.cpp"
)

set(
  IRODS_LIB_FILESYSTEM_PATH_HEADERS_PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/include/irods/filesystem/filesystem_error.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/irods/filesystem/path.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/irods/filesystem/path_traits.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/irods/filesystem/path_utilities.hpp"
)

add_library(
  irods_filesystem_path
  OBJECT
  "${IRODS_LIB_FILESYSTEM_PATH_SOURCES}"
  "${IRODS_LIB_FILESYSTEM_PATH_HEADERS_PUBLIC}"
)
target_include_directories(
  irods_filesystem_path
  PUBLIC
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<BUILD_INTERFACE:${CMAKE_IRODS_SOURCE_DIR}/lib/core/include>"
  PRIVATE
  "${IRODS_EXTERNALS_FULLPATH_BOOST}/include"
)
target_compile_definitions(
  irods_filesystem_path
  PUBLIC
  ${IRODS_COMPILE_DEFINITIONS_PUBLIC}
  PRIVATE
  ${IRODS_COMPILE_DEFINITIONS_PRIVATE}
)
set_target_properties(irods_filesystem_path PROPERTIES INTERFACE_POSITION_INDEPENDENT_CODE TRUE)
set_target_properties(irods_filesystem_path PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

set(
  IRODS_LIB_FILESYSTEM_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/src/filesystem.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/collection_iterator.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/recursive_collection_iterator.cpp"
)

set(
  IRODS_LIB_FILESYSTEM_HEADERS_PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/include/irods/filesystem.hpp"
)

set(
  IRODS_LIB_FILESYSTEM_HEADERS_SUBDIR_PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/include/irods/filesystem/collection_entry.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/irods/filesystem/collection_iterator.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/irods/filesystem/config.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/irods/filesystem/copy_options.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/irods/filesystem/filesystem.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/irods/filesystem/filesystem_error.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/irods/filesystem/object_status.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/irods/filesystem/permissions.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/irods/filesystem/recursive_collection_iterator.hpp"
)

foreach(variant IN ITEMS client server)
  add_library(
    irods_filesystem_${variant}
    OBJECT
    "${IRODS_LIB_FILESYSTEM_SOURCES}"
    "${IRODS_LIB_FILESYSTEM_HEADERS_PUBLIC}"
    "${IRODS_LIB_FILESYSTEM_HEADERS_SUBDIR_PUBLIC}"
  )
  objects_link_libraries(
    irods_filesystem_${variant}
    PUBLIC
    nlohmann_json::nlohmann_json
    fmt::fmt
  )
  target_include_directories(
    irods_filesystem_${variant}
    PUBLIC
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<BUILD_INTERFACE:${CMAKE_IRODS_BINARY_DIR}/lib/core/include>"
    "$<BUILD_INTERFACE:${CMAKE_IRODS_SOURCE_DIR}/lib/core/include>"
    "$<BUILD_INTERFACE:${CMAKE_IRODS_SOURCE_DIR}/lib/api/include>"
    "$<BUILD_INTERFACE:${CMAKE_IRODS_SOURCE_DIR}/lib/hasher/include>"
    "$<BUILD_INTERFACE:${CMAKE_IRODS_SOURCE_DIR}/server/core/include>"
    "$<BUILD_INTERFACE:${CMAKE_IRODS_SOURCE_DIR}/server/icat/include>"
    "$<BUILD_INTERFACE:${CMAKE_IRODS_SOURCE_DIR}/server/re/include>"
    "$<BUILD_INTERFACE:${CMAKE_IRODS_SOURCE_DIR}/server/drivers/include>"
    PRIVATE
    "${IRODS_EXTERNALS_FULLPATH_BOOST}/include"
  )
  target_compile_definitions(
    irods_filesystem_${variant}
    PUBLIC
    ${IRODS_COMPILE_DEFINITIONS_PUBLIC}
    PRIVATE
    ${IRODS_COMPILE_DEFINITIONS_PRIVATE}
  )
  set_target_properties(irods_filesystem_${variant} PROPERTIES INTERFACE_POSITION_INDEPENDENT_CODE TRUE)
  set_target_properties(irods_filesystem_${variant} PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
endforeach()
target_include_directories(
  irods_filesystem_server
  PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_IRODS_SOURCE_DIR}/server/api/include>"
)
target_compile_definitions(
  irods_filesystem_server
  PRIVATE
  IRODS_FILESYSTEM_ENABLE_SERVER_SIDE_API
  IRODS_TRANSPORT_ENABLE_SERVER_SIDE_API
)

install(
  FILES ${IRODS_LIB_FILESYSTEM_HEADERS_PUBLIC}
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/irods"
  COMPONENT ${IRODS_PACKAGE_COMPONENT_DEVELOPMENT_NAME}
)

install(
  FILES
  ${IRODS_LIB_FILESYSTEM_PATH_HEADERS_PUBLIC}
  ${IRODS_LIB_FILESYSTEM_HEADERS_SUBDIR_PUBLIC}
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/irods/filesystem"
  COMPONENT ${IRODS_PACKAGE_COMPONENT_DEVELOPMENT_NAME}
)
