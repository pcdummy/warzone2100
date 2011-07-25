AM_CPPFLAGS = $(WZ_CPPFLAGS) $(QT4_CFLAGS) ${QJSON_CFLAGS}
AM_CFLAGS = $(WZ_CFLAGS)
AM_CXXFLAGS = $(WZ_CXXFLAGS) $(QT4_CFLAGS)

noinst_LIBRARIES = libimagemap.a
noinst_HEADERS = \
	container.h \
	containerimage.h \
	containersvg.h \
	imagemap_export.h \
	loader.h \
	loader_p.h \
	map.h \
	map_p.h

libimagemap_a_SOURCES = \
	container.cpp \
	containerimage.cpp \
	containersvg.cpp \
	loader.cpp \
	map.cpp