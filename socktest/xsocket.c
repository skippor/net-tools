#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/resource.h>
#include <linux/version.h>

#include "xsocket.h"

int set_nonblock(int fd)
{
    if (fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK) < 0) {
        return -1;
    }

    return 0;
}

int set_fd_limit(int limit)
{
    struct rlimit rlim;

    if (getrlimit(RLIMIT_NOFILE, &rlim)) {
        return -1;
    }

    rlim.rlim_cur = limit;
    return setrlimit(RLIMIT_NOFILE, &rlim);
}

int create_tcp_socket(const char *address, uint16_t port)
{
    struct sockaddr_in addr;
    int nreuse = 1;
    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd == -1) {
        return -1;
    }

    (void)setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &nreuse, sizeof(nreuse));
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 9, 0)
    (void)setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &nreuse, sizeof(nreuse));
#endif

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(address);
    addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr)) == -1) {
        close(sockfd);
        return -1;
    }

    return sockfd;
}

int create_udp_socket(const char *address, uint16_t port)
{
    struct sockaddr_in addr;
    int nreuse = 1;
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        return -1;
    }

    (void)setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &nreuse, sizeof(nreuse));
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 9, 0)
    (void)setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &nreuse, sizeof(nreuse));
#endif

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(address);
    addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr)) == -1) {
        close(sockfd);
        return -1;
    }

    return sockfd;
}

int create_raw_socket(const uint8_t protocol, const char *address, uint16_t port)
{
    struct sockaddr_in addr;
    int on = 1;
    int sockfd = socket(AF_INET, SOCK_RAW, protocol);
    if (sockfd == -1) {
        return -1;
    }

    if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, (const char *)&on, sizeof(on)) < 0) {
        close(sockfd);
        return -1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(address);
    addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr)) == -1) {
        close(sockfd);
        return -1;
    }

    return sockfd;
}

int create_unix_socket(const char *path)
{
    struct sockaddr_un addr;
    int sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        return -1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path + 1, path, sizeof(addr.sun_path) - 1);

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        close(sockfd);
        return -1;
    }

    return sockfd;
}