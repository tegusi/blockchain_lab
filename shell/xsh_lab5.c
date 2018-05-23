/* xsh_netinfo.c - xsh_netinfo */

#include <xinu.h>
#include <string.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * xsh_netinfo - obtain and print the IP address, subnet mask, default
 *			router address and other network information
 *------------------------------------------------------------------------
 */

process p1(int localip)
{
	char msg[] = "I'm p1!\n";
	// uint32 localip = getlocalip();
	int slot = udp_register(localip, 300, 200);
	int msglen = strlen(msg);
	int i;
	for(i = 0;i < 3;i++)
	{
		udp_send(slot,msg,msglen);
		sleep(1);
	}
	udp_recv(slot,msg,sizeof(msg),3000);
	kprintf("This message is from P2\n");
	return 0;
}

process p2(int localip)
{
	char msg[100];
	// uint32 localip = getlocalip();
	int slot = udp_register(localip, 200, 300);
	// int msglen = strlen(msg);
	udp_recv(slot,msg,sizeof(msg),4000);
	kprintf("This message is from P1\n");
	udp_send(slot,msg,10);
	return 0;
}

shellcmd lab5_1(int nargs, char *args[]) {
	uint32 localip = getlocalip();
	resume(create(p1,INITSTK, INITPRIO,"P1", 1, localip));
	resume(create(p2,INITSTK, INITPRIO,"P2", 1, localip));
	return OK;
}
