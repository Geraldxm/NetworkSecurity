#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

void main()
{
    struct sockaddr_in dest_info; // 定义一个sockaddr_in结构体，用于存储目标地址和端口信息
    char *data = "UDP message\n"; // 要发送的数据

    // Step 1: Create a network socket
    // 创建一个网络套接字，参数AF_INET表示使用IPv4协议，SOCK_DGRAM表示使用UDP协议，IPPROTO_UDP表示使用UDP协议
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    // Step 2: Provide information about destination.
    // 设置目标地址和端口信息
    memset((char *) &dest_info, 0, sizeof(dest_info)); // 将dest_info结构体清零
    dest_info.sin_family = AF_INET; // 设置地址族为AF_INET，即IPv4
    dest_info.sin_addr.s_addr = inet_addr("10.0.2.69"); // 设置目标IP地址，inet_addr函数将字符串形式的IP地址转换为网络字节序的整数形式
    dest_info.sin_port = htons(9090); // 设置目标端口，htons函数将主机字节序的端口号转换为网络字节序

    // Step 3: Send out the packet.
    // 发送数据包，参数分别为套接字描述符，要发送的数据，数据长度，其他选项（这里为0），目标地址信息，目标地址信息长度
    sendto(sock, data, strlen(data), 0,
                 (struct sockaddr *)&dest_info, sizeof(dest_info));
    close(sock); // 关闭套接字
}