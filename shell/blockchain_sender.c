#include <xinu.h>
#include <stdio.h>
#include "shprototypes.h"

shellcmd listening(){
    uint32 localip,remoteip;
    int32 slot;
    int retval;
    localip = getlocalip();
    char	buf[150];
    int32	delay	= 2000;		/* reception delay in ms    */
    remoteip  = NetData.ipbcast;
    slot = udp_register(remoteip, 988, 987);
    while(1)
    {
        uint32	remip,remport;
        retval = udp_recvaddr(slot, &remip, &remport, buf, sizeof(buf), delay);
        if (retval != SYSERR && retval != TIMEOUT && strcmp(buf,"HELLO") == 0)
        {
            kprintf("\n\nFound one!!\n\n");
            strcpy(buf, "ALIVE");
            udp_sendto(slot,remip,remport,buf,strlen(buf));
        }
    }
}

shellcmd sender(int nargs, char* args[]){
    int	i;			/* index into buffer		*/
	int	retval;			/* return value			*/
	char	msg[] = "Xinu testing UDP echo"; /* message to send	*/
	char	inbuf[1500];		/* buffer for incoming reply	*/
	int32	slot;			/* UDP slot to use		*/
	int32	msglen;			/* length of outgoing message	*/
	uint32	remoteip;		/* remote IP address to use	*/
	uint32	localip;		/* local IP address to use	*/
	uint16	echoport= 7;		/* port number for UDP echo	*/
	uint16	locport	= 52743;	/* local port to use		*/
	int32	retries	= 3;		/* number of retries		*/
	int32	delay	= 2000;		/* reception delay in ms    */

    uint32 machine_ips[100];
    uint32 machine_ports[100];
    uint32 machine_count = 0;

    int balance = 100;

    localip = getlocalip();
    resume(create((void *)listening, INITSTK, INITPRIO + 1,
					"BLC Listener", 0, NULL));
    remoteip  = NetData.ipbcast;
    slot = udp_register(remoteip, echoport, locport);

    strcpy(msg,"HELLO");
    msglen = strnlen(msg, 1200);
	for (i=0; i<retries; i++) {
		retval = udp_send(slot, msg, msglen);
		if (retval == SYSERR) {
			fprintf(stderr, "%s: error sending UDP \n",
				args[0]);
            return 1;
		}

        uint32	remip,remport;
		retval = udp_recvaddr(slot, &remip, &remport, inbuf, sizeof(inbuf), delay);
		if (retval == TIMEOUT) {
			fprintf(stderr, "%s: timeout...\n", args[0]);
			continue;
		} else if (retval == SYSERR) {
			fprintf(stderr, "%s: error from udp_recv \n",
				args[0]);
			udp_release(slot);
			return 1;
		}

        if(strcmp(msg,"ALIVE") == 0)
        {
            kprintf("\n\nFound one!!\n\n");
            int flag = 1;
            for(i = 0;i < machine_count;i++)
            {
                if(machine_ips[i] == remip)
                    flag = 0;
            }
            if(flag == 1)
            {
                machine_count++;
                machine_ips[machine_count] = remip;
                machine_ports[machine_count] = remport;
            }
        }
	}
    sprintf(msg,"%08x_1_1",localip);
    msglen = strlen(msg);
    for(i = 0;i < machine_count;i++){
        udp_sendto(slot,machine_ips[i],machine_ports[i],msg,msglen);
    }

    while(TRUE){
        uint32	remip,remport;
        int length = udp_recvaddr(slot, &remip, &remport, inbuf, sizeof(inbuf), delay);
        if(length > 18 && inbuf[length-1]=='4')
            balance -= 1;
        kprintf("%s\n",inbuf);
    }

    udp_release(slot);
}