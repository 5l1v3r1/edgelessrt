add_custom_command(
  OUTPUT test_u.c
  DEPENDS ../test.edl edger8r
  COMMAND
    edger8r --untrusted ${CMAKE_CURRENT_SOURCE_DIR}/../test.edl --search-path
    ${PROJECT_SOURCE_DIR}/include --search-path ${PLATFORM_EDL_DIR})

add_executable(dynlink_host host.cpp test_u.c)
target_include_directories(dynlink_host PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(dynlink_host oehost)
