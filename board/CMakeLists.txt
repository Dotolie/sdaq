FILE(GLOB BOARDSRCS
        src/feature.cpp
)
INCLUDE_DIRECTORIES(
	${CMAKE_SOURCE_DIR}/board/inc
	)
#ADD_LIBRARY(featyre STATIC  ${BOARDSRCS})
ADD_LIBRARY(feature SHARED  ${BOARDSRCS})
TARGET_LINK_LIBRARIES( feature pthread )
INSTALL(FILES ${CMAKE_SOURCE_DIR}/board/libfeature.so DESTINATION ${BINDIR})
#INSTALL(FILES ${CMAKE_SOURCE_DIR}/board/inc/sensor.h DESTINATION ${BINDIR})


