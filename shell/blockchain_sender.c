// #include <xinu.h>
// #include <stdio.h>
// #include "shprototypes.h"

// shellcmd listening(){
//     uint32 localip,remoteip;
//     int32 slot;
//     int retval;
//     localip = getlocalip();
//     char	buf[150];
//     int32	delay	= 2000;		/* reception delay in ms    */
//     remoteip  = NetData.ipbcast;
//     slot = udp_register(remoteip, 988, 987);
//     while(1)
//     {
//         uint32	remip,remport;
//         retval = udp_recvaddr(slot, &remip, &remport, buf, sizeof(buf), delay);
//         if (retval != SYSERR && retval != TIMEOUT && strcmp(buf,"HELLO") == 0)
//         {
//             kprintf("\n\nFound one!!\n\n");
//             strcpy(buf, "ALIVE");
//             udp_sendto(slot,remip,remport,buf,strlen(buf));
//         }
//     }
// }

// shellcmd sender(int nargs, char* args[]){
//     int	i;			/* index into buffer		*/
// 	int	retval;			/* return value			*/
// 	char	msg[] = "Xinu testing UDP echo"; /* message to send	*/
// 	char	inbuf[1500];		/* buffer for incoming reply	*/
// 	int32	slot;			/* UDP slot to use		*/
// 	int32	msglen;			/* length of outgoing message	*/
// 	uint32	remoteip;		/* remote IP address to use	*/
// 	uint32	localip;		/* local IP address to use	*/
// 	uint16	echoport= 7;		/* port number for UDP echo	*/
// 	uint16	locport	= 52743;	/* local port to use		*/
// 	int32	retries	= 3;		/* number of retries		*/
// 	int32	delay	= 2000;		/* reception delay in ms    */

//     uint32 machine_ips[100];
//     uint32 machine_ports[100];
//     uint32 machine_count = 0;

//     int balance = 100;

//     localip = getlocalip();
//     resume(create((void *)listening, INITSTK, INITPRIO + 1,
// 					"BLC Listener", 0, NULL));
//     remoteip  = NetData.ipbcast;
//     slot = udp_register(remoteip, echoport, locport);

//     strcpy(msg,"HELLO");
//     msglen = strnlen(msg, 1200);
// 	for (i=0; i<retries; i++) {
// 		retval = udp_send(slot, msg, msglen);
// 		if (retval == SYSERR) {
// 			fprintf(stderr, "%s: error sending UDP \n",
// 				args[0]);
//             return 1;
// 		}

//         uint32	remip,remport;
// 		retval = udp_recvaddr(slot, &remip, &remport, inbuf, sizeof(inbuf), delay);
// 		if (retval == TIMEOUT) {
// 			fprintf(stderr, "%s: timeout...\n", args[0]);
// 			continue;
// 		} else if (retval == SYSERR) {
// 			fprintf(stderr, "%s: error from udp_recv \n",
// 				args[0]);
// 			udp_release(slot);
// 			return 1;
// 		}

//         if(strcmp(msg,"ALIVE") == 0)
//         {
//             kprintf("\n\nFound one!!\n\n");
//             int flag = 1;
//             for(i = 0;i < machine_count;i++)
//             {
//                 if(machine_ips[i] == remip)
//                     flag = 0;
//             }
//             if(flag == 1)
//             {
//                 machine_count++;
//                 machine_ips[machine_count] = remip;
//                 machine_ports[machine_count] = remport;
//             }
//         }
// 	}
//     sprintf(msg,"%08x_1_1",localip);
//     msglen = strlen(msg);
//     for(i = 0;i < machine_count;i++){
//         udp_sendto(slot,machine_ips[i],machine_ports[i],msg,msglen);
//     }

//     while(TRUE){
//         uint32	remip,remport;
//         int length = udp_recvaddr(slot, &remip, &remport, inbuf, sizeof(inbuf), delay);
//         if(length > 18 && inbuf[length-1]=='4')
//             balance -= 1;
//         kprintf("%s\n",inbuf);
//     }

//     udp_release(slot);
// }


#include <py.h>
#include <stdio.h>


struct RecvState {
    uint32 senderip;

    int state;

    /*
        state:
            0       empty
            1       waiting
    */
};
struct RecvState recevier[MAXRECEIVER];


struct MinerState {
    uint32 senderip;
    uint32 receiverip;

    int state;
    /*
        state:
            0       empty
            1       waiting
    */
};
struct MinerState  miner[MAXMINER];
uint32 senderIP;
uint16 senderPORT;

shellcmd sender(int nargs, char* args[]) {
    int	i;			/* index into buffer		*/
	int	retval;			/* return value			*/
	char	msg[] = "Xinu testing UDP echo"; /* message to send	*/
	char	inbuf[1500];		/* buffer for incoming reply	*/
	int32	slot;			/* UDP slot to use		*/
	int32	msglen;			/* length of outgoing message	*/
	uint32	remoteip;		/* remote IP address to use	*/
	uint32	localip;		/* local IP address to use	*/
	uint16	echoport= 7;		/* port number for UDP echo	*/
	uint16	locport	= 2306;	/* local port to use		*/
	int32	retries	= 3;		/* number of retries		*/
	int32	delay	= 5000;		/* reception delay in ms    */
    uint32 ip1,ip2;
    int op;
    localip = getlocalip();

    for(i = 0;i < MAXRECEIVER; i++){
        recevier[i].state = 0;
    }
    
    char pingbuf[20];
    sprintf(msg,"%d_1_1",localip);
    for(i = 0;i < 254;i++){
        uint32 ipaddr = NetData.ipprefix + i;
        slot = icmp_register(ipaddr);
        if (slot == SYSERR) {
            fprintf(stderr,"%d: ICMP registration failed\n", ipaddr);
            continue;
        }
        memset(pingbuf,0,sizeof(pingbuf));
        retval = icmp_send(ipaddr, ICMP_ECHOREQST, slot,
					0, pingbuf, sizeof(pingbuf));
        if (retval == SYSERR) {
            fprintf(stderr, "%d: no response from host %d\n", localip, ipaddr);
            icmp_release(slot);
            continue;
	    }
        retval = icmp_recv(slot, pingbuf, sizeof(pingbuf), 1000);
        icmp_release(slot);
        if (retval == TIMEOUT) {
            fprintf(stderr, "%d: no response from host %d\n", localip,
                        ipaddr);
            continue;
        }
        printf("IP %d found!\n",ipaddr);
        machine_ips[machine_count] = ipaddr;
        machine_ports[machine_count] = locport;
        machine_count++;
    }

    // machine_ips[0] = 3232235842;
    // machine_ips[1] = 3232235991;
    // machine_ips[2] = 3232235993;
    // machine_ports[0] = locport;
    // machine_ports[1] = locport;
    // machine_ports[2] = locport;
    // machine_count = 3;
    slot = udp_register(0, 0, locport);
    for(i = 0;i < machine_count;i++){
        udp_sendto(slot,machine_ips[i],machine_ports[i],msg,sizeof(msg));
    }
    while (1) {
        
        // slot = udp_register(remoteip, 988, 987);
        /*
            接收消息
        */

        char msg[150];
        memset(msg, 0, sizeof(msg));
        msglen = udp_recvaddr(slot, &senderIP, &senderPORT, msg, sizeof(msg), delay);
        kprintf("%d\n",msglen);
        if(msglen == -1) break;
        // recv(msg);
       

       /*
            分析消息，消息形如
            ip1_ip2_4
            ip_1_1
       */
        uint32 ip1;
        uint32 ip2;
        uint32 op;
        char* tmp = msg;
        ip1 = atoi(tmp);
        while(*tmp != '_') tmp++; tmp++;
        ip2 = atoi(tmp);
        while(*tmp != '_') tmp++; tmp++;
        op = atoi(tmp);
        kprintf("%u %u %d\n",ip1,ip2,op);
        // sscanf(msg, "%u_%u_%d", &ip1, &ip2, &op);
        
        
        if (op == 1) {
            /*
                作为交易接收方：

                新建一个状态ip1，并向所有IP列表上的设备发送一个合约请求，然后等待某台矿机返回确认合约
            */
            int flag = 0;
            for (i = 0; i < MAXRECEIVER; i++) {
                if (recevier[i].state == 0) {

                    recevier[i].state = 1;
                    recevier[i].senderip = ip1;
                    flag = 1;
                    break;
                }
            }

            if (flag == 0)
                continue;

            char newmsg[150];
            memset(newmsg, 0, sizeof(newmsg));

            sprintf(newmsg,"%u_%u_2",ip1,localip);
            // itoa(ip1, newmsg, 10);      //ip1
            // strcat(newmsg, "_");        //ip1_
            // itoa(localip, tmp, 10);     
            // strcat(newmsg, tmp);        //ip1_ip2
            // strcat(newmsg, "_2");        //ip1_ip2_2

            for (i = 0; i < machine_count; i++) {
                udp_sendto(slot,machine_ips[i], machine_ports[i], newmsg, strlen(newmsg));
            }
        }
        if (op == 2) {
            /*
                作为矿机：

                收到交易请求，新建一个状态，返回消息给交易收到方交易成功
                新建状态是为了防止交易收到方在发送ip1_ip2_2前发送了ip1_ip2_4
            */

            int flag = 0;
            for (i = 0; i < MAXRECEIVER; i++) {
                if (miner[i].state == 0) {
                    miner[i].state = 1;
                    miner[i].senderip = ip1;
                    miner[i].receiverip = ip2;
                    flag = 1;
                    break;
                }
            }
            if (flag == 0)
                continue;
            
            char newmsg[150];
            memset(newmsg, 0, sizeof(newmsg));

            strcpy(newmsg, msg);    //ip1_ip2_2
            newmsg[strlen(newmsg) - 1] = '3';   //ip1_ip2_3

            udp_sendto(slot,ip2, 2306, newmsg, strlen(newmsg));
        }


        if (op == 3) {
            /*
                作为交易接收方：

                如果收到某个矿机返回确认合约，检查所有已存在的状态，如果有对应的状态
                自己增加10 * 0.9块钱并增加一条合约，并且返回给合约机（矿机）交易成功
            */
            int flag = 0;
            for (i = 0; i < MAXRECEIVER; i++) {
                if (recevier[i].state == 1 && recevier[i].senderip == ip1 && localip == ip2) {

                    recevier[i].state = 0;      //交♂易完成
                    flag = 1;
                    break;
                }
            }
            if (flag == 0)
                continue;
            
            char newmsg[150];
            memset(newmsg, 0, sizeof(newmsg));

            strcpy(newmsg, msg);    //ip1_ip2_3
            newmsg[strlen(newmsg) - 1] = '4';   //ip1_ip2_4
            udp_sendto(slot,senderIP, senderPORT, newmsg, strlen(newmsg));
            // UDP_SEND(senderIP, senderPORT, newmsg, strlen(newmsg));

            printf("recevier record: ip1 : %u ip2 : %u\n", ip1, ip2);
            receiverMoney += 9;
        }

        if (op == 4) {

            if (ip1 == localip) {
                /*
                    作为接收方：

                    收到合约机（矿机）交易达成消息，自己总账减少10块钱。
                    
                */
                printf("sender record: ip1 : %u ip2 : %u\n", ip1, ip2);
                senderMoney -= 10;
            }
            else {
                /*
                    作为矿机：

                    如果收到某个接收方的交易成功，检查所有已存在的状态，如果有对应的状态
                    自己增加10 * 0.1块钱并增加一条合约，并且发送给交易发起方交易完成
                */
                int flag = 0;
                for (i = 0; i < MAXRECEIVER; i++) {
                    if (miner[i].state == 1 && miner[i].senderip == ip1 && miner[i].receiverip == ip2) {

                        miner[i].state = 0;      //交♂易完成
                        flag = 1;
                        break;
                    }
                }
                if (flag == 0)
                    continue;
                
                char newmsg[150];
                memset(newmsg, 0, sizeof(newmsg));

                strcpy(newmsg, msg);    //ip1_ip2_4
                udp_sendto(slot,ip1, 2306, newmsg, strlen(newmsg));
                // UDP_SEND(ip1, senderPORT, newmsg, strlen(newmsg));

                printf("miner record: ip1 : %u ip2 : %u\n", ip1, ip2);
                minerMoney += 1;
            }
            
        }
        
    }
    udp_release(slot);
}


