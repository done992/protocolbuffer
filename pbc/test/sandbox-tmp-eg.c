#include "sandbox.pb-c.h"
#include "hadoop_rpc_utils.h"
#include "../yarn.h"
#include "net_utils.h"
#include "str_utils.h"
#include "log_utils.h"
#include "hadoop_rpc_constants.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdbool.h>
#include <string.h>

static int generate_sandbox_request(char** buffer, int* size) {
    RequestProto request = REQUEST_PROTO__INIT;
    request.has_value = true;
    request.value = 1234;
    *buffer = generate_delimited_message((const ProtobufCMessage*)&request, size);
    return 0;
}

int main(int argc, char** argv) {
    int rc;

    /* 1. init proxy */
    hadoop_rpc_proxy_t* proxy =
        (hadoop_rpc_proxy_t*)malloc(sizeof(hadoop_rpc_proxy_t));
    proxy->caller_id = 0;
    proxy->protocol_name = "sandbox.api.SandboxProtocolPB";

    // init socket for proxy, and connect to server
    proxy->socket_id = socket(AF_INET, SOCK_STREAM, 0);
    rc = connect_to_server(proxy->socket_id, argv[1], atoi(argv[2]));
    if (rc != 0) {
        free(proxy);
        return -1;
    }

    // write connection header to this socket
    rc = write_connection_header(proxy);
    if (rc != 0) {
        yarn_log_error("write connection header failed.\n");
        return NULL;
    }


    /* 2. create request */
    char* request = NULL;
    int request_len;

    // generate request
    rc = generate_sandbox_request(&request, &request_len);
    if (0 != rc) {
        yarn_log_error("generate new app request failed.");
        return -1;
    }

    // send request
    rc = send_rpc_request(proxy, "ping", request, request_len);
    if (0 != rc) {
        yarn_log_error("send new_application_request failed.");
        free(request);
        return -1;
    }

    // now we will not use it anymore
    free(request);

    // read response
    int response_msg_len;
    char* response = recv_rpc_response(proxy, &response_msg_len);
    if (!response) {
        return -1;
    }

    // unpack response
    ResponseProto* response_msg = 
        response_proto__unpack(NULL, response_msg_len, response);
    if (!response_msg) {
        yarn_log_error("failed to unpack response message of get_new_application_impl");
        free(response);
        return -1;
    }

    printf("recv: %d\n", response_msg->value);

    free(response);

    return 0;
}
