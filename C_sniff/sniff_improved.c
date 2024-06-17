#include <pcap.h>
#include <stdio.h>
#include <arpa/inet.h>

/* 以太网头部结构 */
struct ethheader {
  u_char  ether_dhost[6]; /* 目标主机地址 */
  u_char  ether_shost[6]; /* 源主机地址 */
  u_short ether_type;     /* 协议类型 (IP, ARP, RARP, etc) */
};

/* IP头部结构 */
struct ipheader {
  unsigned char      iph_ihl:4, //IP头部长度
                     iph_ver:4; //IP版本
  unsigned char      iph_tos; //服务类型
  unsigned short int iph_len; //IP数据包长度 (数据 + 头部)
  unsigned short int iph_ident; //标识
  unsigned short int iph_flag:3, //分片标志
                     iph_offset:13; //标志偏移
  unsigned char      iph_ttl; //生存时间
  unsigned char      iph_protocol; //协议类型
  unsigned short int iph_chksum; //IP数据报校验和
  struct  in_addr    iph_sourceip; //源IP地址
  struct  in_addr    iph_destip;   //目标IP地址
};

/* 处理捕获的数据包 */
void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
  struct ethheader *eth = (struct ethheader *)packet;

  if (ntohs(eth->ether_type) == 0x0800) { // 0x0800 表示IP协议
    struct ipheader * ip = (struct ipheader *)
                           (packet + sizeof(struct ethheader)); 

    printf("       From: %s\n", inet_ntoa(ip->iph_sourceip));   
    printf("         To: %s\n", inet_ntoa(ip->iph_destip));    

    /* 确定协议类型 */
    switch(ip->iph_protocol) {                                 
        case IPPROTO_TCP:
            printf("   Protocol: TCP\n");
            return;
        case IPPROTO_UDP:
            printf("   Protocol: UDP\n");
            return;
        case IPPROTO_ICMP:
            printf("   Protocol: ICMP\n");
            return;
        default:
            printf("   Protocol: others\n");
            return;
    }
  }
}

int main()
{
  pcap_t *handle;
  char errbuf[PCAP_ERRBUF_SIZE];
  struct bpf_program fp;
  char filter_exp[] = "ip proto icmp";
  bpf_u_int32 net;

  // 步骤1: 在名为enp0s3的网络接口上打开实时的pcap会话
  handle = pcap_open_live("enp0s3", BUFSIZ, 1, 1000, errbuf);

  // 步骤2: 将过滤表达式编译成BPF伪代码
  pcap_compile(handle, &fp, filter_exp, 0, net);
  pcap_setfilter(handle, &fp);

  // 步骤3: 捕获数据包
  pcap_loop(handle, -1, got_packet, NULL);

  pcap_close(handle);   // 关闭句柄
  return 0;
}