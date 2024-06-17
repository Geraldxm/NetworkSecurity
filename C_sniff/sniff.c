#include <pcap.h>
#include <stdio.h>
#include "util.c"

// 处理捕获到的数据包的函数
void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
  // 当捕获到数据包时打印一条消息
  printf("捕获到一个数据包\n");
  got_packet_edited(args, header, packet);
  // count_packet_number(args, header, packet);
  printf("\n\n");
}

int main()
{
  pcap_t *handle;                      // pcap 会话句柄
  char errbuf[PCAP_ERRBUF_SIZE];       // 错误信息缓冲区
  struct bpf_program fp;               // BPF (Berkeley Packet Filter) 程序结构
  char filter_exp[] = "ip proto icmp"; // 过滤表达式，用于捕获 ICMP 数据包
  bpf_u_int32 net;                     // 网络地址
  printf("step1\n");
  // 步骤1: 在名为 enp0s3 的网络接口上打开实时的 pcap 会话
  handle = pcap_open_live("eth0", BUFSIZ, 1, 1000, errbuf);
  // enp0s3: 网络接口名称
  // BUFSIZ: 缓冲区大小
  // 1: 混杂模式 (1 = 启用, 0 = 禁用)
  // 1000: 超时时间，单位为毫秒
  // errbuf: 错误信息缓冲区

  // 步骤2: 将过滤表达式编译成 BPF 伪代码
  printf("step2\n");
  printf("pcap_compile()\n");
  pcap_compile(handle, &fp, filter_exp, 0, net);
  // handle: pcap 会话句柄
  // &fp: BPF 程序结构
  // filter_exp: 过滤表达式
  // 0: 优化过滤器 (0 = 不优化)
  // net: 网络地址
  printf("pcap_setfilter()\n");
  pcap_setfilter(handle, &fp);
  // 设置编译后的过滤器为 pcap 会话的当前过滤器

  // 步骤3: 捕获数据包
  printf("step3\n");
  pcap_loop(handle, -1, got_packet, NULL);
  // handle: pcap 会话句柄
  // -1: 无限循环捕获 (-1 = 无限循环)
  // got_packet: 处理捕获数据包的回调函数
  // NULL: 用户指定的参数 (本例中未使用)

  pcap_close(handle); // 关闭句柄并释放资源
  return 0;
}
