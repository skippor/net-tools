#include <sdtio.h>
#include "xsocket.h"

int run_client()
{
    int fd = create_udp_socket("127.0.0.1", 54321);

    /* 填写sockaddr_in*/
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(12345);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    while(1) {
		bzero(buffer, sizeof(buffer));

		printf("Please enter a string to send to server: \n");

		/* 从标准输入设备取得字符串*/
		len = read(STDIN_FILENO, buffer, sizeof(buffer));

		/* 将字符串传送给server端*/
		sendto(sockfd, buffer, len, 0, (struct sockaddr *)&addr, addr_len);

		/* 接收server端返回的字符串*/
		len = recvfrom(sockfd, buffer, sizeof(buffer), 0,
				       (struct sockaddr *)&addr, &addr_len);
		printf("Receive from server: %s\n", buffer);
	}

    return 0;
}

int run_server()
{
    int fd = create_udp_socket("127.0.0.1", 12345);

    while(1) {
		bzero(buffer, sizeof(buffer));
		len = recvfrom(sockfd, buffer, sizeof(buffer), 0,
					   (struct sockaddr *)&addr ,&addr_len);
		/* 显示client端的网络地址和收到的字符串消息 */
		printf("Received a string from client %s, string is: %s\n",
				inet_ntoa(addr.sin_addr), buffer);
		/* 将收到的字符串消息返回给client端 */
		sendto(sockfd,buffer, len, 0, (struct sockaddr *)&addr, addr_len);
	}

    return 0;
}

int main(int argc, char** argv)
{
    return 0;
}