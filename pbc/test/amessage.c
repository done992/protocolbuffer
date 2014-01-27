#include <stdio.h>
#include <stdlib.h>

#include "src/generated-sources/amessage.pb-c.h"

#define FILE_NAME "/tmp/pb.tmp"
#define MAX_LEN  1024

/*
 * Note: amessage__pack and amessage__unpack will allocate memory from Heap,
 * please remember to free them.
 */
void pack_to_file(int argc, char *argv[])
{
	AMessage msg = AMESSAGE__INIT; // AMessage
	void *buf;                     // Buffer to store serialized data
	unsigned len;                  // Length of serialized data
	FILE *fp = NULL;

	msg.a = atoi(argv[1]);
	if (argc == 3) {
		msg.has_b = 1;
		msg.b = atoi(argv[2]);
	}
	len = amessage__get_packed_size(&msg);

	buf = malloc(len);
	amessage__pack(&msg, buf);

	fprintf(stderr, "Writing %d serialized bytes\n", len); // See the length of message

	if (NULL == (fp = fopen(FILE_NAME, "w"))) {
		fprintf(stderr, "fopen failed");
		exit(-1);
	}
	fwrite(buf, len, 1, fp);
	fclose(fp);

	fwrite(buf, len, 1, stdout); // Write to stdout to allow direct command line piping
	free(buf); // Free the allocated serialized buffer
}

void unpack_from_file()
{
	FILE *fp = NULL;
	char buf[MAX_LEN];
	int sum, total;
	AMessage *amsg = NULL;

	if (NULL == (fp = fopen(FILE_NAME, "r"))) {
		fprintf(stderr, "fopen failed");
		exit(-1);
	}

	total = 0;
	while (0 < (sum = fread(buf+total, 1, MAX_LEN, fp))) {
		total += sum;
		if (total >= MAX_LEN) {
			fprintf(stderr, "read more than MAX_LEN");
			exit(-1);
		}
	}

	if (NULL == (amsg = amessage__unpack(NULL, total, buf))) {
		fprintf(stderr, "amessage__unpack failed");
		exit(-1);
	}

	printf("amsg->a = %d\n", amsg->a);
	if(amsg->has_b) {
		printf("amsg->b = %d\n", amsg->b);
	}

	amessage__free_unpacked(amsg, NULL);

	fclose(fp);
}

int main (int argc, const char * argv[])
{
	if (argc != 2 && argc != 3) {   // Allow one or two integers
		fprintf(stderr, "usage: amessage a [b]\n");
		return 1;
	}
	pack_to_file(argc, argv);
	unpack_from_file();
	return 0;
}
