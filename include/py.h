#include <xinu.h>

#define MAXMINER 250
#define MAXRECEIVER 250
uint32 machine_ips[100];
uint32 machine_ports[100];
uint32 machine_count = 0;
uint32 senderMoney = 100;    //一开始有100元，发送方每发送成功一次，减少10元，作为矿机和接收方的收入不计在其中
uint32 receiverMoney = 0;    //一开始有0元，接收方每接收成功一次，增加9元，作为矿机的收入不计在其中
uint32 minerMoney = 0;       //一开始有0元，矿机每帮助完成交易，增加1元