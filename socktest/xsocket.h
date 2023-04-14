#ifndef __XSOCKET_H_20230331__
#define __XSOCKET_H_20230331__


int create_tcp_socket(const char *address, uint16_t port);

int create_udp_socket(const char *address, uint16_t port);

int create_unix_socket(const char *path);

int create_raw_socket(const uint8_t protocol, const char *address, uint16_t port);

int set_nonblock(int fd);

int set_fd_limit(int limit);

#endif