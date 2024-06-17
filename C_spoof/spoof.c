#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#include "myheader.h" // 包含自定义头文件，可能定义了ipheader结构体

/*************************************************************
  Given an IP packet, send it out using a raw socket.
**************************************************************/
void send_raw_ip_packet(struct ipheader* ip)
{
    struct sockaddr_in dest_info; // 定义一个sockaddr_in结构体，用于存储目标地址信息
    int enable = 1; // 用于设置套接字选项的标志

    // Step 1: Create a raw network socket.
    // 创建一个原始网络套接字，参数AF_INET表示使用IPv4协议，SOCK_RAW表示使用原始套接字，IPPROTO_RAW表示使用原始IP协议
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);

    // Step 2: Set socket option.
    // 设置套接字选项，这里设置IP层头部包含在数据包中
    setsockopt(sock, IPPROTO_IP, IP_HDRINCL,
                     &enable, sizeof(enable));

    // Step 3: Provide needed information about destination.
    // 设置目标地址信息
    dest_info.sin_family = AF_INET; // 设置地址族为AF_INET，即IPv4
    dest_info.sin_addr = ip->iph_destip; // 从ipheader结构体中获取目标IP地址

    // Step 4: Send the packet out.
    // 发送数据包，参数分别为套接字描述符，要发送的数据（这里是ipheader结构体），数据长度，其他选项（这里为0），目标地址信息，目标地址信息长度
    sendto(sock, ip, ntohs(ip->iph_len), 0,
           (struct sockaddr *)&dest_info, sizeof(dest_info));
    close(sock); // 关闭套接字
}