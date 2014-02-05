#include <stdio.h>
#include <stdlib.h>

#include "src/generated-sources/sandbox.pb-c.h"
#include "src/util/xmalloc.h"
#include "src/util/constants.h"

#define FILE_NAME "/tmp/pb.tmp"
#define MAX_LEN  1024

/*
 * Note: __pack and __unpack will allocate memory from Heap,
 * please remember to free them.
 *
 * Notice:
   there is no has_ flag for optional submessages -- if the pointer is non-NULL, then we assume that it is a value.
 */
void pack_request_to_file()
{
	void *buf;
	unsigned len;
	FILE *fp = NULL;
	int i;

	RequestProto request = REQUEST_PROTO__INIT;
	request.has_value = true;
	request.value = 1;

	len = request_proto__get_packed_size(&request);

	buf = xmalloc(len);
	request_proto__pack(&request, buf);

	fprintf(stderr, "Writing %d serialized bytes\n", len); // See the length of message

	if (NULL == (fp = fopen(FILE_NAME, "w"))) {
		fprintf(stderr, "fopen failed");
		exit(1);
	}
	fwrite(buf, len, 1, fp);
	fclose(fp);

	xfree(buf); // Free the allocated serialized buffer
}

void unpack_request_from_file()
{
	FILE *fp = NULL;
	char buf[MAX_LEN];
	int sum, total, i;
	RequestProto *msg = NULL;

	if (NULL == (fp = fopen(FILE_NAME, "r"))) {
		fprintf(stderr, "fopen failed");
		exit(-1);
	}

	total = 0;
	while (0 < (sum = fread(buf+total, 1, MAX_LEN, fp))) {
		total += sum;
		if (total >= MAX_LEN) {
			fprintf(stderr, "read more than MAX_LEN");
			exit(1);
		}
	}

	if (NULL == (msg = request_proto__unpack(NULL, total, buf))) {
		fprintf(stderr, "request_proto__unpack failed");
		exit(1);
	}

	if (msg->has_value) {
		printf("msg->value = %d\n", msg->value);
	}

	request_proto__free_unpacked(msg, NULL);

	fclose(fp);
}




int main (int argc, const char * argv[])
{
	pack_request_to_file();
	unpack_request_from_file();
	return 0;
}
