AM_CPPFLAGS = -DYY_NO_INPUT $(PHYSFS_CFLAGS) $(PNG_CFLAGS) $(OGGVORBIS_CFLAGS) $(OPENAL_CFLAGS) $(OPENGLC_CFLAGS) $(OPENGL_CFLAGS) $(WZ_CPPFLAGS) $(GLEW_CFLAGS)
AM_CFLAGS = $(WZ_CFLAGS)
AM_CXXFLAGS = $(WZ_CXXFLAGS) $(QT4_CFLAGS)
AM_LFLAGS = $(FLEX_FLAGS)
AM_YFLAGS = -d

warzone2100_MOCH = \
		confighandler.h \
		Frontend/wzhelper_moc.cpp \
		Frontend/wzqmlview_moc.cpp
		   
warzone2100_MOCFILES = \
		confighandler_moc.cpp \
		Frontend/wzhelper_moc.cpp \
		Frontend/wzqmlview_moc.cpp

%_moc.cpp: %.h
	$(MOC4) -o $@ $<

clean-moc-extra:
	rm -vf ${warzone2100_MOCFILES}

clean-am: clean-moc-extra

bin_PROGRAMS = warzone2100

noinst_HEADERS = \
	$(warzone2100_MOCH) \
	clparse.h \
	lconfig.h \
	Filesystem/filesystem.h \
	Filesystem/qphysfsenginehandler.h \
	Frontend/imageprovider.h \
	Frontend/wzhelper.h \
	Frontend/wzqmlview.h \
	confighandler.h \
	Map/levparser.h \
	Map/map.h

warzone2100_SOURCES = \
	${warzone2100_MOCFILES} \
	clparse.cpp \
	lconfig.cpp \
	confighandler.cpp \
	Filesystem/filesystem.cpp \
	Frontend/imageprovider.cpp \
	Frontend/wzhelper.cpp \
	Frontend/wzqmlview.cpp \
	main.cpp \
	Map/levparser.cpp \
	Map/map.cpp

warzone2100_LIBS = \
	$(top_builddir)/lib/imagemap/libimagemap.a \
	$(top_builddir)/lib/framework/libframework.a \
	$(top_builddir)/lib/qtgame/libqtgame.a

warzone2100_LDADD = $(warzone2100_LIBS) $(LTLIBINTL) $(PHYSFS_LIBS) $(PNG_LIBS) $(OGGVORBIS_LIBS) \
	$(OPENAL_LIBS) $(OPENGLC_LIBS) $(OPENGL_LIBS) $(QT4_LIBS) $(QJSON_LIBS) $(GLEW_LIBS) \
	$(X_LIBS) $(X_EXTRA_LIBS)