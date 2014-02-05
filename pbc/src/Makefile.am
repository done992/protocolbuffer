AM_CPPFLAGS=-I$(includedir) -I${top_srcdir}/generated-sources

AUTOMAKE_OPTIONS = subdir-objects

lib_LTLIBRARIES = libprotoc-eg.la

generate_headers = \
	generated-sources/amessage.pb-c.h \
	generated-sources/repeated_msg.pb-c.h \
	generated-sources/sub_msg.pb-c.h \
	generated-sources/sub_repeated_msg.pb-c.h \
	generated-sources/sandbox.pb-c.h
	
generate_sources = \
	generated-sources/amessage.pb-c.c \
	generated-sources/repeated_msg.pb-c.c \
	generated-sources/sub_msg.pb-c.c \
	generated-sources/sub_repeated_msg.pb-c.c \
	generated-sources/sandbox.pb-c.c

util_headers = \
	util/argv.h \
	util/common.h \
	util/constants.h \
	util/log.h \
	util/net.h \
	util/str.h \
	util/xmalloc.h 
	
util_sources = \
	util/argv.c \
	util/log.c \
	util/net.c \
	util/str.c \
	util/xmalloc.c

headers = \
	$(generate_headers) \
	$(util_headers)

libprotoc_eg_la_SOURCES = \
	$(headers) 	\
	$(generate_sources) \
	$(util_sources)