#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include "util.c"

int main() {
    int PACKET_LEN = 512; // 定义数据包长度
    char buffer[PACKET_LEN]; // 用于存储捕获的数据包的缓冲区
    struct sockaddr saddr; // 存储数据包来源地址的结构体
    struct packet_mreq mr; // 用于设置数据包组选项的结构体

    // 创建原始套接字
    int sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL)); 
    // AF_PACKET: 套接字类型为数据包套接字
    // SOCK_RAW: 原始套接字类型
    // ETH_P_ALL: 捕获所有类型的以太网数据包

    // 开启混杂模式
    mr.mr_type = PACKET_MR_PROMISC;  
    // PACKET_MR_PROMISC: 设置为混杂模式
    setsockopt(sock, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mr, sizeof(mr));
    // SOL_PACKET: 套接字级别为数据包级别
    // PACKET_ADD_MEMBERSHIP: 添加数据包组
    // &mr: 指向 packet_mreq 结构体的指针
    // sizeof(mr): packet_mreq 结构体的大小

    // 捕获数据包
    while (1) {
        int data_size=recvfrom(sock, buffer, PACKET_LEN, 0, 
                         &saddr, (socklen_t*)sizeof(saddr));
        // sock: 套接字描述符
        // buffer: 存储捕获数据包的缓冲区
        // PACKET_LEN: 缓冲区大小
        // 0: 默认标志
        // &saddr: 存储数据包来源地址的结构体
        // (socklen_t*)sizeof(saddr): saddr 结构体的大小
        if(data_size) {
            printf("捕获到一个数据包\n");
            print_sockaddr(&saddr, sizeof(saddr));
        }
    }

    close(sock); // 关闭套接字
    return 0;
}