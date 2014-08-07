cmake_minimum_required (VERSION 2.6)
project (open62541)
set (open62541_VERSION_MAJOR 0)
set (open62541_VERSION_MINOR 1)

set (CMAKE_VERBOSE_MAKEFILE on )
if (CMAKE_COMPILER_IS_GNUCC)
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -pedantic -pipe -fstack-protector -Wall -Wextra -Wno-unused-parameter -Wno-unused-function -Wno-unused-label -Wpointer-arith -Wformat -Wreturn-type -Wsign-compare -Wmultichar -Wformat-nonliteral -Winit-self -Wuninitialized -Wno-deprecated -Wformat-security -Werror -ffunction-sections -fdata-sections -Wl,--gc-sections")
endif (CMAKE_COMPILER_IS_GNUCC)

# multithreading
set (MULTITHREADING OFF CACHE BOOL "Enable multithreading")
if (MULTITHREADING)
    find_package (Threads REQUIRED)
endif (MULTITHREADING)

# encodings
set (UA_ENCODING_AMOUNT 1) # binary encoding

set (UA_ENCODING_XML OFF CACHE BOOL "Enable XML-encoding of the UA types")
if (UA_ENCODING_XML)
    MATH(EXPR UA_ENCODING_AMOUNT "${UA_ENCODING_AMOUNT}+1")
    find_package (EXPAT REQUIRED)
    if (EXPAT_FOUND)
        include_directories (${EXPAT_INCLUDE_DIRS})
    else (EXPAT_FOUND)
        message (FATAL_ERROR "Expat library not found.")
    endif (EXPAT_FOUND)
endif (UA_ENCODING_XML)

set (UA_ENCODING_JSON OFF CACHE BOOL "Enable JSON-encoding of the UA types")
if (UA_ENCODING_JSON)
    MATH(EXPR UA_ENCODING_AMOUNT "${UA_ENCODING_AMOUNT}+1")
endif (UA_ENCODING_JSON)

# build the library
configure_file ("src/ua_config.h.in" "${PROJECT_BINARY_DIR}/ua_config.h")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/util")
include_directories("${PROJECT_BINARY_DIR}") # generated header file

set (lib_sources src/ua_types.c
                 src/ua_types_encoding_binary.c
				 src/ua_application.c
				 src/ua_transport.c
				 src/ua_transport_binary.c
				 src/ua_transport_binary_secure.c
				 src/ua_services_attribute.c
				 src/ua_services_session.c
				 src/ua_services_discovery.c
				 src/ua_services_securechannel.c
				 src/ua_services_nodemanagement.c
				 src/ua_services_view.c
				 src/ua_services_subscription.c
				 src/ua_services_monitoreditems.c
				 src/util/ua_util.c
				 src/util/ua_list.c
				 src/util/ua_indexedList.c
				 src/util/ua_base64.c
				 ${PROJECT_BINARY_DIR}/ua_types_generated.c
				 ${PROJECT_BINARY_DIR}/ua_namespace_0.c) # do not pollute the src dir with generated files

if (MULTITHREADING)
    list (APPEND lib_sources src/ua_namespace_concurrent.c)
else ()
    list (APPEND lib_sources src/ua_namespace.c)
endif (MULTITHREADING)

set (generate_options "")

if (UA_ENCODING_XML)
    list (APPEND lib_sources src/ua_types_encoding_xml.c
                             src/ua_namespace_xml.c
                             src/ua_xml.c)
    set (generate_options "${generate_options}--with-xml")
endif (UA_ENCODING_XML)

if (UA_ENCODING_JSON)
    list (APPEND lib_sources src/ua_types_encoding_json.c)
    set (generate_options "${generate_options}--with-json")
endif (UA_ENCODING_JSON)

add_library (open62541 ${lib_sources})

# generate data structures
add_custom_command (OUTPUT ${PROJECT_BINARY_DIR}/ua_types_generated.c
                           ${PROJECT_BINARY_DIR}/ua_types_generated.h
                    COMMAND python ${PROJECT_SOURCE_DIR}/tools/generate_builtin.py ${generate_options} ${PROJECT_SOURCE_DIR}/schema/Opc.Ua.Types.bsd ${PROJECT_BINARY_DIR}/ua_types_generated
                    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/tools/generate_builtin.py
                            ${CMAKE_CURRENT_SOURCE_DIR}/schema/Opc.Ua.Types.bsd)

add_custom_command (OUTPUT ${PROJECT_BINARY_DIR}/ua_namespace_0.c
                           ${PROJECT_BINARY_DIR}/ua_namespace_0.h
                    COMMAND python ${PROJECT_SOURCE_DIR}/tools/generate_namespace.py ${generate_options} ${PROJECT_SOURCE_DIR}/schema/NodeIds.csv ${PROJECT_BINARY_DIR}/ua_namespace_0
                    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/tools/generate_namespace.py
                            ${CMAKE_CURRENT_SOURCE_DIR}/schema/NodeIds.csv)

# download queue.h if required
if (WIN32)
	if (NOT EXISTS "${PROJECT_BINARY_DIR}/queue.h")
        file (DOWNLOAD "http://openbsd.cs.toronto.edu/cgi-bin/cvsweb/~checkout~/src/sys/sys/queue.h" "${PROJECT_BINARY_DIR}/queue.h" STATUS result)
		list (GET result 0 download_ok)
		if (NOT ${download_ok} MATCHES 0)
			execute_process (COMMAND rm "${PROJECT_BINARY_DIR}/queue.h") # remove empty file if created
			message (FATAL_ERROR "queue.h could not be downloaded")
		endif ()
	endif ()
endif (WIN32)

# build example implementations
add_executable(exampleServer examples/src/opcuaServer.c
                             examples/src/networklayer.c)
target_link_libraries (exampleServer open62541) 