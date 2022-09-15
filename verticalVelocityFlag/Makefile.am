lib_LTLIBRARIES = verticalVelocityFlag.la

verticalVelocityFlag_la_SOURCES = verticalVelocityFlag.cpp
verticalVelocityFlag_la_CPPFLAGS= -I$(top_srcdir)/include -I$(top_srcdir)/plugins/plugin_utils
verticalVelocityFlag_la_LDFLAGS = -module -avoid-version -shared
verticalVelocityFlag_la_LIBADD = $(top_builddir)/plugins/plugin_utils/libplugin_utils.la

AM_CPPFLAGS = $(CONF_CPPFLAGS)
AM_CFLAGS = $(CONF_CFLAGS)
AM_CXXFLAGS = $(CONF_CXXFLAGS)

EXTRA_DIST = \
	README.verticalVelocityFlag.txt \
	verticalVelocityFlag.sln \
	verticalVelocityFlag.vcxproj	\
	verticalVelocityFlag.vcxproj.filters

MAINTAINERCLEANFILES =	\
	Makefile.in
