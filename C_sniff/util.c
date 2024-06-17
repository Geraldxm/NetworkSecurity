#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <pcap.h>

/* 以太网头部结构 */
struct ethheader {
    u_char  ether_dhost[6]; /* 目标以太网地址 */
    u_char  ether_shost[6]; /* 源以太网地址 */
    u_short ether_type;     /* 协议类型 (IP, ARP, RARP, etc) */
};

/* IP头部结构 */
struct ipheader {
    unsigned char      iph_ihl:4, // IP头部长度
                       iph_ver:4; // IP版本
    unsigned char      iph_tos; // 服务类型
    unsigned short int iph_len; // IP数据包长度 (数据 + 头部)
    unsigned short int iph_ident; // 标识
    unsigned short int iph_flag:3, // 分片标志
                       iph_offset:13; // 标志偏移
    unsigned char      iph_ttl; // 生存时间
    unsigned char      iph_protocol; // 协议类型
    unsigned short int iph_chksum; // IP数据报校验和
    struct  in_addr    iph_sourceip; // 源IP地址
    struct  in_addr    iph_destip;   // 目标IP地址
};

void got_packet_edited(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
    struct ethheader *eth = (struct ethheader *)packet;

    if (ntohs(eth->ether_type) == 0x0800) { // 0x0800 表示IP协议
        
        // 跳过以太网头部，获取IP头部
        struct ipheader *ip = (struct ipheader *)(packet + sizeof(struct ethheader));

        // 打印以太网头部信息
        printf("以太网源地址: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n",
               eth->ether_shost[0], eth->ether_shost[1], eth->ether_shost[2],
               eth->ether_shost[3], eth->ether_shost[4], eth->ether_shost[5]);
        printf("以太网目标地址: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n",
               eth->ether_dhost[0], eth->ether_dhost[1], eth->ether_dhost[2],
               eth->ether_dhost[3], eth->ether_dhost[4], eth->ether_dhost[5]);

        // 打印IP头部信息
        printf("源IP地址: %s\n", inet_ntoa(ip->iph_sourceip));
        printf("目标IP地址: %s\n", inet_ntoa(ip->iph_destip));
        printf("IP协议: %d\n", ip->iph_protocol);
        printf("IP数据包长度: %d\n", ntohs(ip->iph_len));

        // 打印IP数据包内容
        int data_len = ntohs(ip->iph_len) - (ip->iph_ihl * 4); // 计算数据长度
        printf("IP数据包内容: ");
        for (int i = 0; i < data_len; i++) {
            printf("%02x ", packet[sizeof(struct ethheader) + sizeof(struct ipheader) + i]);
        }
        printf("\n");
    }
}


static int eth_type_count[0xFFFF] = {0}; // 以太网类型计数数组


// 统计各种以太网帧的数量
void count_packet_number(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {

    // 获取以太网头部
    const struct ethheader *eth = (const struct ethheader *)packet;

    // 获取以太网类型
    uint16_t eth_type = ntohs(eth->ether_type);

    // 更新以太网类型计数
    eth_type_count[eth_type]++;

    // 输出统计结果
    printf("Ethernet Type: 0x%04X, Count: %d\n", eth_type, eth_type_count[eth_type]);
}


// 打印sockaddr结构体内部数据的函数
void print_sockaddr(struct sockaddr *saddr, socklen_t addrlen) {
    // 检查地址长度以确定地址类型
    if (addrlen == sizeof(struct sockaddr_in)) {
        // 如果地址长度与IPv4地址结构体大小相同，则将其转换为sockaddr_in指针
        struct sockaddr_in *sin = (struct sockaddr_in *)saddr;
        char ip[INET_ADDRSTRLEN]; // 用于存储转换后的IP地址字符串
        // 将网络字节序的IP地址转换为点分十进制字符串
        inet_ntop(AF_INET, &(sin->sin_addr), ip, INET_ADDRSTRLEN);
        printf("Family: AF_INET\n"); // 打印地址族
        printf("IP Address: %s\n", ip); // 打印IP地址
        printf("Port: %d\n", ntohs(sin->sin_port)); // 打印端口号（从网络字节序转换为主机字节序）
    } else if (addrlen == sizeof(struct sockaddr_in6)) {
        // 如果地址长度与IPv6地址结构体大小相同，则将其转换为sockaddr_in6指针
        struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)saddr;
        char ip[INET6_ADDRSTRLEN]; // 用于存储转换后的IP地址字符串
        // 将网络字节序的IP地址转换为冒号分隔的十六进制字符串
        inet_ntop(AF_INET6, &(sin6->sin6_addr), ip, INET6_ADDRSTRLEN);
        printf("Family: AF_INET6\n"); // 打印地址族
        printf("IP Address: %s\n", ip); // 打印IP地址
        printf("Port: %d\n", ntohs(sin6->sin6_port)); // 打印端口号（从网络字节序转换为主机字节序）
    } else {
        // 如果地址长度与已知的地址结构体大小不匹配，则打印未知地址族
        printf("Unknown address family\n");
    }
}