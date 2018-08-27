#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <arpa/inet.h>

#define BUFSIZE 1024
#define PORT_UDP 8700
#define PORT_TCP 8701

/* 
 * error - wrapper for perror
 */
void error(char *msg) {
    perror(msg);
    exit(0);
}

int main(int argc, char **argv) {
    
	int sockfd_receive;				//udp(local->arduino)
	int sockfd_transfer;			//udp(arduino->local)
	int port_udp; 					//port to listen on 
	int clientlen; 					//byte size of client's address
	int serverlen;
	struct sockaddr_in serveraddr; 	//server's addr (local addr)
	struct sockaddr_in clientaddr; 	//client addr
	char buf[BUFSIZE]; 				//message buf
	char *hostaddrp; 				//dotted decimal host addr string
	int optval; 					//flag value for setsockopt
	int n; 							//message byte size
	char send_key[3];

    port_udp = PORT_UDP;
	send_key[0]='#';
	send_key[1]='#';
	send_key[2]='#';
	//socket: create the UDP socket 
	sockfd_receive = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd_receive < 0) error("ERROR opening socket");

  /* setsockopt: Handy debugging trick that lets 
   * us rerun the server immediately after we kill it; 
   * otherwise we have to wait about 20 secs. 
   * Eliminates "ERROR on binding: Address already in use" error. 
   */
   
	optval = 1;
	setsockopt(sockfd_receive, SOL_SOCKET, SO_REUSEADDR,(const void *)&optval , sizeof(int));

  
	//build the server's Internet address
	bzero((char *) &serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons((unsigned short)port_udp);

   
	//bind: associate the parent socket with a port 
	if (bind(sockfd_receive, (struct sockaddr *) &serveraddr, 
		sizeof(serveraddr)) < 0) 
		{
			error("ERROR on binding");
		}
 
	clientlen = sizeof(clientaddr);
	serverlen = sizeof(serveraddr);
	//get client ip: wait for a datagram
	while (1) {
    bzero(buf, BUFSIZE);
    n = recvfrom(sockfd_receive, buf, BUFSIZE, 0,
		 (struct sockaddr *) &clientaddr, &clientlen);
	
    if (n < 0)
      error("ERROR in recvfrom");


    hostaddrp = inet_ntoa(clientaddr.sin_addr);
    if (hostaddrp == NULL)
      error("ERROR on inet_ntoa\n");
    //printf("server received datagram from  (%s)\n", hostaddrp);
    //printf("server received %d/%d bytes: %s\n", strlen(buf), n, buf);
	if(buf[0]=='#'&&buf[1]=='#'&&buf[2]=='#')break;//getd key word out of loop
  }
  
  
    //create the socket return server info
    sockfd_transfer = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd_transfer < 0) 
        error("ERROR opening socket");


    /* 傳送資料(手動)*/
    //bzero(buf, BUFSIZE);
    //printf("Please enter msg: ");
    //fgets(buf, BUFSIZE, stdin);
	
	n = sendto(sockfd_transfer, send_key, strlen(send_key), 0, &serveraddr, serverlen);
	while (1) {
    bzero(buf, BUFSIZE);
    n = recvfrom(sockfd_receive, buf, BUFSIZE, 0,
	(struct sockaddr *) &clientaddr, &clientlen);
	
    if (n < 0)
      error("ERROR in recvfrom");


    hostaddrp = inet_ntoa(clientaddr.sin_addr);
    if (hostaddrp == NULL)
      error("ERROR on inet_ntoa\n");
    printf("server received datagram from  (%s)\n", hostaddrp);
    printf("server received %d/%d bytes: %s\n", strlen(buf), n, buf);
	if(buf[0]=='#'&&buf[1]=='#'&&buf[2]=='#')break;//getd key word out of loop
  }
	
	
	
	
    /* 傳送資料 */
    clientlen = sizeof(clientaddr);
	
	n = sendto(sockfd_transfer, send_key, strlen(send_key), 0, &clientaddr, clientlen);
	if (n < 0) 
      error("ERROR in sendto");
    n = sendto(sockfd_transfer, hostaddrp, strlen(hostaddrp), 0, &clientaddr, clientlen);
    if (n < 0) 
      error("ERROR in sendto");

    return 0;
}
