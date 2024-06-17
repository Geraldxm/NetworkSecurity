#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>

void main()
{
    struct sockaddr_in server; // 服务器地址结构
    struct sockaddr_in client; // 客户端地址结构
    int clientlen; // 客户端地址长度
    char buf[1500]; // 缓冲区，用于存储接收的数据

    // 步骤1: 创建UDP套接字
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    // AF_INET: 使用IPv4协议
    // SOCK_DGRAM: 创建数据报套接字（UDP）
    // IPPROTO_UDP: 指定协议为UDP

    // 步骤2: 初始化服务器地址并绑定套接字
    memset((char *) &server, 0, sizeof(server));
    // 清空服务器地址结构
    server.sin_family = AF_INET;
    // 设置地址族为IPv4
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    // 监听所有本地接口
    server.sin_port = htons(9090);
    // 设置端口号为9090

    if (bind(sock, (struct sockaddr *) &server, sizeof(server)) < 0)
        perror("ERROR on binding");
    // 将套接字绑定到指定的地址和端口

    // 步骤3: 进入无限循环，接收并打印客户端发送的数据
    while (1) {
        bzero(buf, 1500);
        // 清空缓冲区
        recvfrom(sock, buf, 1500-1, 0,
                       (struct sockaddr *) &client, &clientlen);
        // 接收来自客户端的数据
        // sock: 套接字描述符
        // buf: 存储接收数据的缓冲区
        // 1500-1: 缓冲区大小减1，为数据保留空间
        // 0: 默认标志
        // &client: 客户端地址结构
        // &clientlen: 客户端地址长度
        printf("%s\n", buf);
        // 打印接收到的数据
    }
    close(sock); // 关闭套接字
}