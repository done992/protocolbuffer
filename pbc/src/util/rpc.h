#ifndef RPC_H_
#define RPC_H_

extern int umbilical_socket_id;

extern int connect_and_handshake_with_localserver(char *port_str);

extern int rpc_send(int umbilical_socket_id, char* buffer, long length);

extern int rpc_recv(int umbilical_socket_id, char** buffer, int* length);


#endif /* RPC_H_ */
