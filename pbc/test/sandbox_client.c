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

static void usage (void)
{
  die ("usage: sandbox_client_run [--tcp=HOST:PORT | --unix=PATH]\n"
       "\n"
       "Options:\n"
       "  --tcp=HOST:PORT  Port to listen on for RPC clients.\n"
       "  --unix=PATH      Unix-domain socket to listen on.\n"
      );
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

static protobuf_c_boolean is_whitespace (const char *text)
{
  while (*text != 0)
    {
      if (!isspace (*text))
        return 0;
      text++;
    }
  return 1;
}

static void chomp_trailing_whitespace (char *buf)
{
  unsigned len = strlen (buf);
  while (len > 0)
    {
      if (!isspace (buf[len-1]))
        break;
      len--;
    }
  buf[len] = 0;
}

static protobuf_c_boolean starts_with (const char *str, const char *prefix)
{
  return memcmp (str, prefix, strlen (prefix)) == 0;
}

static void parse_response_handler(const ResponseProto *message,
		void *closure_data) {
	puts("parse_response_handler");

	if (message == NULL) {
		fprintf(stderr, "ReponseProto is NULL");
		exit(1);
	}

	if (! message->has_value) {
		printf("===============message->has_value is false\n");
	} else {
		printf("===============message->value = %d\n", message->value);
	}

	*(bool*)closure_data = true;
}

int main(int argc, char**argv) {
	ProtobufCService *service;
	ProtobufC_RPC_Client *client;
	ProtobufC_RPC_AddressType address_type = 0;
	const char *address_name = NULL;
	unsigned i;
	bool success = false;

	for (i = 1; i < (unsigned) argc; i++) {
		if (starts_with(argv[i], "--tcp=")) {
			address_type = PROTOBUF_C_RPC_ADDRESS_TCP;
			address_name = strchr(argv[i], '=') + 1;
		} else if (starts_with(argv[i], "--unix=")) {
			address_type = PROTOBUF_C_RPC_ADDRESS_LOCAL;
			address_name = strchr(argv[i], '=') + 1;
		} else
			usage();
	}

	if (address_name == NULL)
		die("missing --tcp=HOST:PORT or --unix=PATH");

	service = protobuf_c_rpc_client_new(address_type, address_name, &sandbox_protocol_service__descriptor, NULL);
	if (service == NULL)
		die("error creating client");

	client = (ProtobufC_RPC_Client *) service;

	fprintf(stderr, "Connecting... ");

	while (!protobuf_c_rpc_client_is_connected(client))
		protobuf_c_dispatch_run(protobuf_c_dispatch_default());

	fprintf(stderr, "done.\n");

	RequestProto request = REQUEST_PROTO__INIT;
	request.has_value = true;
	request.value = 1;

	sandbox_protocol_service__ping(service, &request, parse_response_handler, &success);
	while (!success) {
		protobuf_c_dispatch_run(protobuf_c_dispatch_default());
	}

	protobuf_c_service_destroy (service);

	return 0;
}
