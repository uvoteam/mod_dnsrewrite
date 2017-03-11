##
cmake_minimum_required(VERSION 2.6)
project(mod_dnsrewrite C)
set(PROJECT_VERSION "0.0.1")

## User settable options
option(DEBUG "Enable debugging output" OFF)
set(DNSREWRITE_CONFIG_ENV "DNSREWRITE_CONFIG" CACHE STRING "Name for environment variable to look for domain mapping file name")

## Define targets
add_library(dnsrewrite MODULE rewrite.c)

## Compiler setup
if(DEBUG)
	set(DEBUG_COMPILE_FLAGS "-g")
endif()
configure_file(config.h.in config.h)
include_directories(${mod_dnsrewrite_SOURCE_DIR} ${mod_dnsrewrite_BINARY_DIR})
set_target_properties(dnsrewrite PROPERTIES
                      PREFIX ""
					  COMPILE_FLAGS "-Wall ${DEBUG_COMPILE_FLAGS}")

## Set up installer
install(TARGETS dnsrewrite DESTINATION lib/${PROJECT_NAME})

## The End ## vim: se ts=4 sw=4: ##
