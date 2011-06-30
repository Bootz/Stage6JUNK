# Copyright (C) 2008-2010 Trinity <http://www.trinitycore.org/>
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

file(GLOB_RECURSE sources_Protocol Protocol/*.h)
file(GLOB_RECURSE sources_Daemon Daemon/*.cpp Daemon/*.h)
file(GLOB_RECURSE sources_Socket Socket/*.cpp Socket/*.h)
file(GLOB_RECURSE sources_Buffered Buffered/*.cpp Buffered/*.h)
file(GLOB sources *.cpp *.h)

set(wardend_SRCS
  ${wardend_SRCS}
  ${sources_Protocol}
  ${sources_Daemon}
  ${sources_Socket}
  ${sources_Buffered}
  ${sources}
)

include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/dep/zlib
  ${CMAKE_SOURCE_DIR}/dep/mysqllite/include
  ${CMAKE_SOURCE_DIR}/src/server/shared
  ${CMAKE_SOURCE_DIR}/src/server/shared/Logging
  ${CMAKE_SOURCE_DIR}/src/server/shared/Packets
  ${CMAKE_SOURCE_DIR}/src/server/shared/Configuration
  ${CMAKE_SOURCE_DIR}/src/server/shared/Utilities
  ${CMAKE_SOURCE_DIR}/src/server/shared/Cryptography
  ${CMAKE_SOURCE_DIR}/src/server/shared/Database
  ${CMAKE_SOURCE_DIR}/src/server/shared/Database/Implementation
  ${CMAKE_SOURCE_DIR}/src/server/shared/Threading
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/Buffered
  ${CMAKE_CURRENT_SOURCE_DIR}/Protocol
  ${CMAKE_CURRENT_SOURCE_DIR}/Daemon
  ${CMAKE_CURRENT_SOURCE_DIR}/Socket
  ${ACE_INCLUDE_DIR}
  ${OPENSSL_INCLUDE_DIR}
)

set(wardend_LINK_FLAGS "")

add_executable(wardend ${wardend_SRCS})

add_dependencies(wardend revision.h)

set_target_properties(wardend PROPERTIES LINK_FLAGS "${wardend_LINK_FLAGS}")

target_link_libraries(wardend
  shared
  ${ACE_LIBRARY}
  ${OPENSSL_LIBRARIES}
  ${OPENSSL_EXTRA_LIBRARIES}
  ${ZLIB_LIBRARIES}
  ${MYSQL_LIBRARY}
  )

if( WIN32 )
  add_custom_command(TARGET wardend
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/wardend.conf.dist ${CMAKE_BINARY_DIR}/bin/$(ConfigurationName)/
  )
endif()