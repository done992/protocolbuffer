AM_CPPFLAGS=-I$(includedir) -I${top_srcdir}/src/ -I${top_srcdir}/src/generated-sources

LDADD = -L$(top_builddir)/src -lprotoc-eg -L/Users/caoj7/program/common-c/lib -lprotobuf-c

AUTOMAKE_OPTIONS = subdir-objects
		

bin_PROGRAMS = \
	amessage_run

amessage_run_SOURCES = \
    	amessage.c