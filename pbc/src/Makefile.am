AM_CPPFLAGS=-I$(includedir) -I${top_srcdir}/generated-sources

AUTOMAKE_OPTIONS = subdir-objects

lib_LTLIBRARIES = libprotoc-eg.la

headers = 


libprotoc_eg_la_SOURCES = \
	$(headers) 	

# add pb generated sources	
libprotoc_eg_la_SOURCES += \
	generated-sources/amessage.pb-c.c
	
	

#noinst_PROGRAMS = \
#	sandboxtest
#
#sandboxtest_SOURCES = \
#    test/sandboxtest.c
#
#sandboxtest_LDADD = -lyarn
#sandboxtest_LDFLAGS = -lprotobuf-c