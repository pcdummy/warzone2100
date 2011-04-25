SUBDIRS = \
	miniupnpc \
	bson

AM_CPPFLAGS = $(WZ_CPPFLAGS) $(QT4_CFLAGS)
AM_CFLAGS = $(WZ_CFLAGS)
AM_CXXFLAGS = $(WZ_CXXFLAGS) $(QT4_CFLAGS)

noinst_LIBRARIES = libnetplay.a
noinst_HEADERS = \
	netlobby.h \
	netlog.h \
	netplay.h \
	netqueue.h \
	netsocket.h \
	nettypes.h

libnetplay_a_SOURCES = \
	netsocket.cpp \
	netlobby.cpp \
	netlog.cpp \
	netplay.cpp \
	netqueue.cpp \
	nettypes.cpp

libnetplay_a_LIBADD = \
	$(top_builddir)/lib/framework/libframework.a \
	$(top_builddir)/lib/netplay/miniupnpc/libminiupnpc.a \
	$(top_builddir)/lib/netplay/bson/libbson.a
