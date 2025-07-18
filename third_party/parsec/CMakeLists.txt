# Defs
set(IFACE_DIR ${CMAKE_CURRENT_LIST_DIR}/interface)

add_library(parsec INTERFACE)
target_include_directories(parsec INTERFACE ${IFACE_DIR})
target_link_libraries(parsec INTERFACE fmt::fmt)

