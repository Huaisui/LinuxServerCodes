#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <libgen.h>

/**
 * @param sockfd 需要监听的服务器套接字描述符
 * @param addr struct sockaddr 类型的指针，用于存储连接的客户端地址信息的指针
 * @param addrlen 一个指向整数的指针，指示addr 缓冲区的长度
 * @return 接受来自客户端的 socket，返回值唯一标识被接受的链接
 *
 * int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
 *
 * 在 sleep 的 30 秒之中，打开并关闭 telnet，accept 依然正常返回;
 *
 * accept 只是从监听队列中取出连接，而不论连接处于何种状态（如 ESTABLISHED 状态和 CLOSE_WAIT 状态），更不关心任何网络状况的变化。
 */

int main(int argc, char *argv[]) {
    if (argc <= 2) {
        printf("usage: %s ip_address port_number\n", basename(argv[0]));
        return 1;
    }
    const char *ip = argv[1];
    int port = atoi(argv[2]);

    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);

    // 创建服务端 socket
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    assert(sock >= 0);

    int ret = bind(sock, (struct sockaddr *) &address, sizeof(address));
    assert(ret != -1);

    ret = listen(sock, 5);
    assert(ret != -1);

    sleep(30);

    struct sockaddr_in client;
    socklen_t client_addrlength = sizeof(client);
    // 接受来自客户端的 socket，返回值唯一标识被接受的链接
    int connfd = accept(sock, (struct sockaddr *) &client, &client_addrlength);
    if (connfd < 0) {
        printf("errno is: %d\n", errno);
    } else {
        // INET_ADDRSTRLEN 宏定义，用于表示一个 IPv4 地址的字符串表示的最大长度
        // 该宏的值通常是 16，因为最长的 IPv4 地址（"255.255.255.255"）有 15 个字符，加上结尾的空字符\0，总共是 16 个字符。
        char remote[INET_ADDRSTRLEN];
        printf("connected with ip: %s and port: %d\n",
               inet_ntop(AF_INET, &client.sin_addr, remote, INET_ADDRSTRLEN), ntohs(client.sin_port));
        close(connfd);
    }

    close(sock);
    return 0;
}

