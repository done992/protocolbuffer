#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "rpc.h"
#include "net.h"
#include "constants.h"

extern int umbilical_socket_id = -1;

extern int rpc_send(int umbilical_socket_id, char* buffer, long length)
{
	int rc;
	// insanity check
	if (!buffer) {
		fprintf(stderr, "a NULL buffer passed in, what happened?\n");
		return -1;
	}

	// pack message and send
	write_endian_swap_int(umbilical_socket_id, length);
	rc = write_all(umbilical_socket_id, buffer, length);
	if (rc != 0) {
		fprintf(stderr, "failed to send buffer to server\n");
		return -1;
	}

	return 0;
}


extern int rpc_recv(int umbilical_socket_id, char** buffer, int* length)
{
    int rc;
    int8_t success;
    int len = 0;

    rc = read_all(umbilical_socket_id, &success, 1);
    if (0 != rc) {
    	fprintf(stderr, "read success status from socket failed\n");
        return -1;
    }
    if (success != RT_Succeed) {
    	fprintf(stderr, "recved error response from server\n");
        return -1;
    }

    // read len from socket
    rc = read_all(umbilical_socket_id, &len, sizeof(int));
    if (0 != rc) {
       	fprintf(stderr, "read length of umbilical socket failed.\n");
        return -1;
    }
    len = int_endian_swap(len);

    // create buffer and read buffer
    *buffer = (char*)malloc(len);
    if (!*buffer) {
    	fprintf(stderr, "failed to allocate memory for recv message\n");
        return -1;
    }
    rc = read_all(umbilical_socket_id, *buffer, len);
    if (rc != 0) {
    	fprintf(stderr, "failed to read response message from server\n");
        free(*buffer);
        return -1;
    }

    *length = len;
    return 0;
}


extern int connect_and_handshake_with_localserver(char *port_str)
{
    int rc;

    // init socket
    umbilical_socket_id = socket(AF_INET, SOCK_STREAM, 0);
    if (umbilical_socket_id < 0) {
    	fprintf(stderr, "setup socket failed\n");
    	return -1;
    }

    // read host/port
    char host[1024];
    rc = gethostname(host, 1023);
    if (!port_str || (0 != rc)) {
    	fprintf(stderr, "failed to get host or port when trying to connect to server\n");
        return -1;
    }
    int port = atoi(port_str);

    // connect to server
    rc = connect_to_server(umbilical_socket_id, host, port);
    if (0 != rc) {
    	fprintf(stderr, "connect to server failed.\n");
        return -1;
    }

    // send handshake
    rc = write_all(umbilical_socket_id, HandShake_MSG, strlen(HandShake_MSG));
    if (0 != rc) {
    	fprintf(stderr, "send handshake to socket failed\n");
        return -1;
    }

    // recv handshake
    char recv[strlen(HandShake_MSG) + 1];
    rc = read_all(umbilical_socket_id, recv, strlen(HandShake_MSG));
    if (0 != rc) {
    	fprintf(stderr, "read handshake from socket failed\n");
        return -1;
    }
    recv[strlen(HandShake_MSG)] = '\0';

    // valid handshake recved from server
    if (0 != strcmp(HandShake_MSG, recv)) {
    	fprintf(stderr, "failed to validate handshake from server, read str is %s \n", recv);
        return -1;
    }

    return 0;
}
