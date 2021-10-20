CMAKE_MINIMUM_REQUIRED (VERSION 2.8.7)

PROJECT (sdaq)

ADD_DEFINITIONS(-Wall )
SET (EXTRA_C_FLAGS "-mfloat-abi=hard -mfpu=neon-fp-armv8")

SET(BINDIR  ${CMAKE_SOURCE_DIR}/bin)
SET(AUTHOR_NAME "ywkim")

SET ( PROJECT_VERSION_MAJOR 2 )
SET ( PROJECT_VERSION_MINOR 4 )
CONFIGURE_FILE ( ${CMAKE_SOURCE_DIR}/version.h.in  ${CMAKE_SOURCE_DIR}/inc/version.h )

INCLUDE_DIRECTORIES ( 	
	${CMAKE_SOURCE_DIR}/inc
        ${CMAKE_SOURCE_DIR}/board/inc
)


LINK_DIRECTORIES ( ${CMAKE_SOURCE_DIR}/lib )

FILE(GLOB SRCS
	src/main.cpp
        src/msg.cpp
        src/thread.cpp
        src/taskMgr.cpp
        src/timerMgr.cpp
	src/adcconfig.cpp
	src/config.cpp
	src/log.cpp
        src/core.cpp
	src/utils.cpp
	src/Socket.cpp
	src/ServerSocket.cpp
	src/SocketException.cpp
	src/server.cpp
	src/sensor.cpp
	src/dsp.cpp
	src/output.cpp
	src/ipsetup.cpp
)


message("build sdaq")
ADD_SUBDIRECTORY( ${CMAKE_SOURCE_DIR}/board )
ADD_EXECUTABLE(${PROJECT_NAME} ${SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} pthread rt crypto m stdc++ feature)

INSTALL(TARGETS ${PROJECT_NAME} DESTINATION ${BINDIR})

