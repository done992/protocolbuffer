#include <stdio.h>
#include <stdlib.h>

#include "src/generated-sources/sub_repeated_msg.pb-c.h"
#include "src/util/xmalloc.h"

#define FILE_NAME "/tmp/pb.tmp"
#define MAX_LEN  1024

/*
 * Note: __pack and __unpack will allocate memory from Heap,
 * please remember to free them.
 */
void pack_to_file(int argc, int argv[])
{
	CompMsg msg = COMP_MSG__INIT;
	void *buf;
	unsigned len;
	FILE *fp = NULL;
	int i;

	msg.n_a = argc;
	msg.a =  xmalloc(sizeof(EleMsg *) * msg.n_a);
	for (i = 0; i < argc; i++) {
		msg.a[i] = xmalloc(sizeof(EleMsg));
		//Note: remember to init
		ele_msg__init(msg.a[i]);
		msg.a[i]->value = argv[i];
	}

	len = comp_msg__get_packed_size(&msg);

	buf = xmalloc(len);
	comp_msg__pack(&msg, buf);

	fprintf(stderr, "Writing %d serialized bytes\n", len); // See the length of message

	if (NULL == (fp = fopen(FILE_NAME, "w"))) {
		fprintf(stderr, "fopen failed");
		exit(1);
	}
	fwrite(buf, len, 1, fp);
	fclose(fp);

	for (i = 0; i < argc; i++) {
		xfree(msg.a[i]);
	}
	xfree(msg.a);
	xfree(buf); // Free the allocated serialized buffer
}

void unpack_from_file()
{
	FILE *fp = NULL;
	char buf[MAX_LEN];
	int sum, total, i;
	CompMsg *msg = NULL;

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

	if (NULL == (msg = comp_msg__unpack(NULL, total, buf))) {
		fprintf(stderr, "comp_msg__unpack failed");
		exit(1);
	}

	printf("msg->n_a = %d\n", msg->n_a);
	for (i = 0; i < msg->n_a; i++) {
		printf("msg->a[%d]->value = %d\n", i, msg->a[i]->value);
	}

	comp_msg__free_unpacked(msg, NULL);

	fclose(fp);
}

int main (int argc, const char * argv[])
{
	int array[3] = {1, 2, 3};
	pack_to_file(3, array);
	unpack_from_file();
	return 0;
}
