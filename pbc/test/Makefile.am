AM_CPPFLAGS=-g -I$(includedir) -I${top_srcdir}/src/ -I${top_srcdir}/src/generated-sources

LDADD = -L$(top_builddir)/src -lprotoc-eg -L/Users/caoj7/program/common-c/lib -lprotobuf-c

AUTOMAKE_OPTIONS = subdir-objects
		

bin_PROGRAMS = \
	amessage_run \
	repeated_run \
	sub_run \
	sub_repeated_run \
	sandbox_tofile_run \
	sandbox_server_run \
	sandbox_client_run \
	sandbox_toserver_run

amessage_run_SOURCES = amessage.c
repeated_run_SOURCES = repeated.c
sub_run_SOURCES = sub.c
sub_repeated_run_SOURCES = sub_repeated.c
sandbox_tofile_run_SOURCES = sandbox_tofile.c
sandbox_server_run_SOURCES = sandbox_server.c
sandbox_client_run_SOURCES = sandbox_client.c
sandbox_toserver_run_SOURCES = sandbox_toserver.c