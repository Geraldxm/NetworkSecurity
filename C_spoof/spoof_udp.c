#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#include "myheader.h" // 包含自定义头文件，定义了ipheader和udpheader结构

// 声明函数send_raw_ip_packet，参数为ipheader结构的指针
void send_raw_ip_packet(struct ipheader* ip);

/*
这段C代码的目的是创建一个伪造的UDP数据包。
并使用任意的源IP地址和端口。伪造的数据包将被发送到目标IP地址和端口。
*/

/******************************************************************
  Spoof a UDP packet using an arbitrary source IP Address and port
*******************************************************************/
int main() {
   char buffer[1500]; // 创建一个缓冲区来存储伪造的数据包

   // 将缓冲区初始化为0
   memset(buffer, 0, 1500);

   // 创建一个指向缓冲区中IP头的指针
   struct ipheader *ip = (struct ipheader *) buffer;

   // 创建一个指向缓冲区中UDP头的指针
   struct udpheader *udp = (struct udpheader *) (buffer + sizeof(struct ipheader));

   /*********************************************************
      Step 1: Fill in the UDP data field.
    ********************************************************/
   // 创建一个指向UDP数据的指针
   char *data = buffer + sizeof(struct ipheader) + sizeof(struct udpheader);

   // 设置UDP数据为字符串"Hello Server!\n"
   const char *msg = "Hello Server!\n";
   int data_len = strlen(msg);
   strncpy(data, msg, data_len);

   /*********************************************************
      Step 2: Fill in the UDP header.
    ********************************************************/
   // 设置UDP源端口为12345
   udp->udp_sport = htons(12345);

   // 设置UDP目标端口为9090
   udp->udp_dport = htons(9090);

   // 设置UDP长度为UDP头大小+数据长度
   udp->udp_ulen = htons(sizeof(struct udpheader) + data_len);

   // 设置UDP校验和为0，因为许多操作系统会忽略这个字段
   udp->udp_sum = 0;

   /*********************************************************
      Step 3: Fill in the IP header.
    ********************************************************/
   // 设置IP版本为4
   ip->iph_ver = 4;

   // 设置IP头长度为5
   ip->iph_ihl = 5;

   // 设置IP生存时间为20
   ip->iph_ttl = 20;

   // 设置源IP地址为1.2.3.4
   ip->iph_sourceip.s_addr = inet_addr("1.2.3.4");

   // 设置目标IP地址为10.0.2.69
   ip->iph_destip.s_addr = inet_addr("10.0.2.69");

   // 设置协议类型为UDP（值为17）
   ip->iph_protocol = IPPROTO_UDP;

   // 设置IP数据包总长度为IP头大小+UDP头大小+数据长度
   ip->iph_len = htons(sizeof(struct ipheader) + sizeof(struct udpheader) + data_len);

   /*********************************************************
      Step 4: Finally, send the spoofed packet
    ********************************************************/
   // 发送伪造的IP数据包
   send_raw_ip_packet(ip);

   return 0;
}