#include <stdio.h>
#include <stdlib.h>

#include "src/generated-sources/repeated_msg.pb-c.h"
#include "src/util/xmalloc.h"

#define FILE_NAME "/tmp/pb.tmp"
#define MAX_LEN  1024

/*
 * Note: __pack and __unpack will allocate memory from Heap,
 * please remember to free them.
 */
void pack_to_file(int argc, int argv[])
{
	RepeatedMsgProto msg = REPEATED_MSG_PROTO__INIT;
	void *buf;
	unsigned len;
	FILE *fp = NULL;
	int i;

	msg.n_c = argc;
	msg.c = xmalloc(sizeof(int) * msg.n_c); // Allocate memory to store int32
	for (i = 0; i < argc; i++) {
		msg.c[i] = argv[i];
	}

	len = repeated_msg_proto__get_packed_size(&msg);

	buf = xmalloc(len);
	repeated_msg_proto__pack(&msg, buf);

	fprintf(stderr, "Writing %d serialized bytes\n", len); // See the length of message

	if (NULL == (fp = fopen(FILE_NAME, "w"))) {
		fprintf(stderr, "fopen failed");
		exit(1);
	}
	fwrite(buf, len, 1, fp);
	fclose(fp);

	xfree(msg.c);
	xfree(buf); // Free the allocated serialized buffer
}

void unpack_from_file()
{
	FILE *fp = NULL;
	char buf[MAX_LEN];
	int sum, total, i;
	RepeatedMsgProto *msg = NULL;

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

	if (NULL == (msg = repeated_msg_proto__unpack(NULL, total, buf))) {
		fprintf(stderr, "repeated_msg_proto__unpack failed");
		exit(1);
	}

	printf("msg->n_c = %d\n", msg->n_c);
	for (i = 0; i < msg->n_c; i++) {
		printf("msg->c[%d] = %d", i, msg->c[i]);
	}

	repeated_msg_proto__free_unpacked(msg, NULL);


	fclose(fp);
}

int main (int argc, const char * argv[])
{
	int array[3] = {1, 2, 3};
	pack_to_file(3, array);
	unpack_from_file();
	return 0;
}
