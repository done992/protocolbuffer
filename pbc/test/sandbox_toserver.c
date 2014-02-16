#include <stdio.h>
#include <stdlib.h>

#include "src/generated-sources/sandbox.pb-c.h"
#include "src/util/xmalloc.h"
#include "src/util/constants.h"
#include "src/util/rpc.h"

#define MAX_LEN  1024

/*
 * Note: __pack and __unpack will allocate memory from Heap,
 * please remember to free them.
 *
 * Notice:
   there is no has_ flag for optional submessages -- if the pointer is non-NULL, then we assume that it is a value.
 */
void pack_request(int value, char **buf, int *len)
{
	RequestProto request = REQUEST_PROTO__INIT;
	request.has_value = true;
	request.value = value;

	*len = request_proto__get_packed_size(&request);

	*buf = xmalloc(len);
	request_proto__pack(&request, *buf);
}

int parse_response(char *buf, int len, int *rt_value)
{
	ResponseProto *msg = NULL;

	if (NULL == (msg = response_proto__unpack(NULL, len, buf))) {
		fprintf(stderr, "response_proto__unpack failed");
		return -1;
	}

	if (msg->has_value) {
		*rt_value = msg->value;
	}

	response_proto__free_unpacked(msg, NULL);
	return 0;
}


int main (int argc, const char * argv[])
{
	int rc;
	char *port_str = "8888";
	int value = 9;

	char *send_buf = NULL;
	int send_len;
	char *recv_buf = NULL;
	char recv_len;

	int rt_value;

//	//1. connect to server
//	rc = connect_and_handshake_with_localserver(port_str);
//	if (rc < 0) {
//		fprintf(stderr, "connect_and_handshake_with_localserver failed\n");
//		goto cleanup;
//	}

//	SandboxProtocolService_Service service =

	//2. pack
	pack_request(value, &send_buf, &send_len);

	//3. send
	if (0 > rpc_send(umbilical_socket_id, send_buf, send_len)) {
		fprintf(stderr, "rpc_send failed\n");
		goto cleanup;
	}

	//4. recv
	if (0 > rpc_recv(umbilical_socket_id, &recv_buf, &recv_len)) {
		fprintf(stderr, "rpc_recv failed\n");
		goto cleanup;
	}

	//5. parse
	parse_response(recv_buf, recv_len, &rt_value);
	printf("---------------------result--------------------------\n");
	printf("return_value = %d\n", rt_value);

	close(umbilical_socket_id);
	return 0;
cleanup:
	xfree(send_buf);
	xfree(recv_buf);
	close(umbilical_socket_id);
	return -1;
}
