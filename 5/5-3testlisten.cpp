#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h>

/**
 * 执行:
 * step 1: ./5-3testlisten 127.0.0.1 12345 5  // 三个参数：ip，端口号，backlog（backlog参数表示等待连接的最大数量）
 *
 * step 2: telnet 127.0.0.1 12345 // 使用 telnet 链接 ip，端口，重复5次以上
 *
 * step 3: netstat -nt | grep 12345 查看链接情况
 *
 * 结果：
 * 观察到前5个建立的链接状态为 ESTABLISHED，超过5个的部分的链接状态为 SYN_SENT，随后超时
 */

static bool stop = false;

static void handle_term(int sig) {
    stop = true;
}

int main(int argc, char *argv[]) {
    signal(SIGTERM, handle_term);

    // 参数需要大于等于三个
    if (argc <= 3) {
        printf("usage: %s ip_address port_number backlog\n", basename(argv[0]));
        return 1;
    }

    const char *ip = argv[1];
    int port = atoi(argv[2]);
    int backlog = atoi(argv[3]);

    // 创建 socket，PF_INET 表示 IPv4 地址族，SOCK_STREAM 表示使用TCP协议
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    assert(sock >= 0);

    struct sockaddr_in address;
    // bzero() 是一个函数，用于将一块内存区域中的内容清零
    // 需要注意的是，bzero() 函数在一些新的 C 标准中已经被标记为不推荐使用，可以考虑使用 memset() 函数来达到相同的效果
    bzero(&address, sizeof(address));

    // AF_INET 表示 IPv4 地址族
    address.sin_family = AF_INET;
    // 将字符串类型的 IP 地址 ip 转换为二进制格式，并将结果存储在 address.sin_addr 字段中
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);

    // 绑定套接字和地址
    int ret = bind(sock, (struct sockaddr *) &address, sizeof(address));
    assert(ret != -1);

    // 设置套接字为监听状态，backlog 参数表示等待连接的最大数量
    ret = listen(sock, backlog);
    assert(ret != -1);

    while (!stop) {
        sleep(1);
    }

    close(sock);
    return 0;
}
