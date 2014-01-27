#include <stdio.h>
#include <stdlib.h>

#include "src/generated-sources/sub_msg.pb-c.h"
#include "src/util/xmalloc.h"

#define FILE_NAME "/tmp/pb.tmp"
#define MAX_LEN  1024

/*
 * Note: __pack and __unpack will allocate memory from Heap,
 * please remember to free them.
 *
 * Notice:
   there is no has_ flag for optional submessages -- if the pointer is non-NULL, then we assume that it is a value.
 */
void pack_to_file(int argc, int argv[])
{
	void *buf;
	unsigned len;
	FILE *fp = NULL;
	int i;

	CompositeMsg compositeMsg = COMPOSITE_MSG__INIT;

	SubMsg subMsg1 = SUB_MSG__INIT;
	subMsg1.value = argv[0];
	compositeMsg.a = &subMsg1;

	SubMsg subMsg2 = SUB_MSG__INIT;
	//if argc != 2, then compisiteMsg.b = NULL
	if (argc == 2) {
		subMsg2.value = argv[1];
		compositeMsg.b = &subMsg2;
	}

	len = composite_msg__get_packed_size(&compositeMsg);

	buf = xmalloc(len);
	composite_msg__pack(&compositeMsg, buf);

	fprintf(stderr, "Writing %d serialized bytes\n", len); // See the length of message

	if (NULL == (fp = fopen(FILE_NAME, "w"))) {
		fprintf(stderr, "fopen failed");
		exit(1);
	}
	fwrite(buf, len, 1, fp);
	fclose(fp);

	xfree(buf); // Free the allocated serialized buffer
}

void unpack_from_file()
{
	FILE *fp = NULL;
	char buf[MAX_LEN];
	int sum, total, i;
	CompositeMsg *msg = NULL;

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

	if (NULL == (msg = composite_msg__unpack(NULL, total, buf))) {
		fprintf(stderr, "repeated_msg_proto__unpack failed");
		exit(1);
	}

	printf("msg->a->value = %d\n", msg->a->value);
	//cool!!
	if (msg->b) {
		printf("msg->b->value = %d\n", msg->b->value);
	}

	composite_msg__free_unpacked(msg, NULL);

	fclose(fp);
}

int main (int argc, const char * argv[])
{
	int array[1] = {9};
	pack_to_file(1, array);
	unpack_from_file();

	puts("-----------------------");
	int array2[2] = {1, 2};
	pack_to_file(2, array2);
	unpack_from_file();
	return 0;
}
