IF("${CMAKE_BUILD_TYPE}" MATCHES "^Debug")
ADD_DEFINITIONS("-DIMAGEMAP_DEBUG")
ENDIF("${CMAKE_BUILD_TYPE}" MATCHES "^Debug")

find_package(QJSON REQUIRED)

SET(imagemap_INCLUDES ${QJSON_INCLUDE_DIR})
SET(imagemap_SOURCES "containerimage.cpp"
                     "containersvg.cpp"
                     "container.cpp"
                     "loader.cpp"
                     "map.cpp")

SET(imagemap_LIBRARIES ${QT_QTSVG_LIBRARY}
                       ${QJSON_LIBRARIES})

INCLUDE_DIRECTORIES(${QMLFRONTEND_INCLUDES})
ADD_LIBRARY(imagemap STATIC ${imagemap_SOURCES})