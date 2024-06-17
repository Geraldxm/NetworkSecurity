#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#include "myheader.h" // 包含自定义头文件，可能定义了一些结构体和函数

// 函数声明，计算校验和
unsigned short in_cksum (unsigned short *buf, int length);
// 函数声明，发送原始IP数据包
void send_raw_ip_packet(struct ipheader* ip);

/*
用来发送一个伪造的ICMP回显请求的。type = 8
它使用了原始套接字（raw socket）来发送IP数据包，这样就可以绕过操作系统的网络栈，直接发送数据包。
*/

/******************************************************************
  Spoof an ICMP echo request using an arbitrary source IP Address
*******************************************************************/
int main() {
   char buffer[1500]; // 创建一个缓冲区，用于存储IP数据包

   memset(buffer, 0, 1500); // 将缓冲区初始化为0

   /*********************************************************
      Step 1: Fill in the ICMP header.
    ********************************************************/
   // 跳过IP头部，填充ICMP头部
   struct icmpheader *icmp = (struct icmpheader *)
                             (buffer + sizeof(struct ipheader));
   icmp->icmp_type = 8; // ICMP Type: 8 is request, 0 is reply.

   // 计算ICMP头部的校验和
   icmp->icmp_chksum = 0;
   icmp->icmp_chksum = in_cksum((unsigned short *)icmp,
                                 sizeof(struct icmpheader));

   /*********************************************************
      Step 2: Fill in the IP header.
    ********************************************************/
   struct ipheader *ip = (struct ipheader *) buffer;
   ip->iph_ver = 4; // IP版本号，IPv4
   ip->iph_ihl = 5; // IP头部长度，通常为5
   ip->iph_ttl = 20; // TTL（Time to Live），数据包的生命周期
   ip->iph_sourceip.s_addr = inet_addr("1.2.3.4"); // 源IP地址
   ip->iph_destip.s_addr = inet_addr("10.0.2.69"); // 目标IP地址
   ip->iph_protocol = IPPROTO_ICMP; // 协议类型，这里是ICMP
   ip->iph_len = htons(sizeof(struct ipheader) +
                       sizeof(struct icmpheader)); // IP数据包总长度

   /*********************************************************
      Step 3: Finally, send the spoofed packet
    ********************************************************/
   send_raw_ip_packet (ip); // 发送伪造的IP数据包

   return 0;
}