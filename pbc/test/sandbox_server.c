#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include "src/util/constants.h"
#include "src/generated-sources/sandbox.pb-c.h"
#include <google/protobuf-c/protobuf-c-rpc.h>

static void
die (const char *format, ...)
{
  va_list args;
  va_start (args, format);
  vfprintf (stderr, format, args);
  va_end (args);
  fprintf (stderr, "\n");
  exit (1);
}

static void
usage (void)
{
  die ("usage: sandbox_server_run [--port=NUM | --unix=PATH] \n"
       "\n"
       "Options:\n"
       "  --port=NUM       Port to listen on for RPC clients.\n"
       "  --unix=PATH      Unix-domain socket to listen on.\n" );
}

static void *xmalloc (size_t size)
{
  void *rv;
  if (size == 0)
    return NULL;
  rv = malloc (size);
  if (rv == NULL)
    die ("out-of-memory allocating %u bytes", (unsigned) size);
  return rv;
}

static void *xrealloc (void *a, size_t size)
{
	void *rv;
	if (size == 0) {
		free(a);
		return NULL;
	}
	if (a == NULL) {
		return xmalloc(size);
	}
	rv = realloc(a, size);
	if (rv == NULL) {
		die("out-of-memory re-allocating %u bytes", (unsigned) size);
	}
	return rv;
}

static char *xstrdup (const char *str)
{
	if (str == NULL) {
		return NULL;
	}
	return strcpy(xmalloc(strlen(str) + 1), str);
}

static char *peek_next_token (char *buf)
{
	while (*buf && !isspace(*buf)) {
		buf++;
	}
	while (*buf && isspace(*buf)) {
		buf++;
	}
	return buf;
}

static protobuf_c_boolean is_whitespace (const char *text)
{
	while (*text != 0) {
		if (!isspace(*text)) {
			return 0;
		}
		text++;
	}
	return 1;
}

static void chomp_trailing_whitespace (char *buf)
{
	unsigned len = strlen(buf);
	while (len > 0) {
		if (!isspace(buf[len - 1])) {
			break;
		}
		len--;
	}
	buf[len] = 0;
}

static protobuf_c_boolean starts_with (const char *str, const char *prefix)
{
  return memcmp (str, prefix, strlen (prefix)) == 0;
}


static void example__ping(ProtobufCService *service, const RequestProto *input,
		ResponseProto_Closure closure, void *closure_data) {
	if (input == NULL || !input->has_value) {
		closure(NULL, closure_data);
	} else {
		printf("input->value = %d\n", input->value);

		ResponseProto response = RESPONSE_PROTO__INIT;
		response.has_value = true;
		response.value = input->value + 1;
		closure(&response, closure_data);
	}
}

static SandboxProtocolService_Service the_sandbox_service = SANDBOX_PROTOCOL_SERVICE__INIT(example__);

int main(int argc, char**argv)
{
	ProtobufC_RPC_Server *server;
	ProtobufC_RPC_AddressType address_type = 0;
	const char *address_name = NULL;
	unsigned i;

	for (i = 1; i < (unsigned) argc; i++) {
		if (starts_with(argv[i], "--port=")) {
			address_type = PROTOBUF_C_RPC_ADDRESS_TCP;
			address_name = strchr(argv[i], '=') + 1;
		} else if (starts_with(argv[i], "--unix=")) {
			address_type = PROTOBUF_C_RPC_ADDRESS_LOCAL;
			address_name = strchr(argv[i], '=') + 1;
		} else
			usage();
	}

	if (address_name == NULL) {
		die("missing --port=NUM or --unix=PATH");
	}

	server = protobuf_c_rpc_server_new(address_type, address_name, (ProtobufCService *) &the_sandbox_service, NULL);

	for (;;) {
		protobuf_c_dispatch_run(protobuf_c_dispatch_default());
	}

  return 0;
}
