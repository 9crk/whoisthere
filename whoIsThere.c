/* Auth:9crk	mail:admin@9crk.com 2015/3/3 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <linux/sockios.h>
#include <ifaddrs.h>

#define PASSWORD "9crk"
#define LISTEN_PORT	888

int ms_to_wait;
int wait_flag;
struct sockaddr_in server_addr;
char* deviceList;
FILE* deviceFile;

void *thread_to_wait(void *arg)
{
	usleep(ms_to_wait*1000);
	wait_flag = 1;
	printf("wait done\n");
	fclose(deviceFile);
	exit(0);
	//pthread_exit(NULL);
}
/*
void *thread_to_process(void *arg)
{
	printf("recv from: %s\n", inet_ntoa(server_addr.sin_addr));
	fputs(inet_ntoa(server_addr.sin_addr), deviceFile);
	fputc('\n', deviceFile);
	//inet_ntoa(server_addr.sin_addr),ntohs(server_addr.sin_port)
	pthread_exit(NULL);
}*/
int query_for_devices(int port, const char* dev_name_like_eth0, int time_out_ms)
{
    socklen_t addr_len;
	int socketfd;
    char buf[64];
    struct sockaddr_in *sin;
    struct ifreq ifr;
	pthread_t wait_thread, tmp_thread;
//open List file
	deviceFile = fopen(deviceList, "w");
//open handle
    if((socketfd = socket(AF_INET,SOCK_DGRAM,0)) < 0){
        perror("socket");
        return -1;
    }
//get broadcast IP
	strcpy(ifr.ifr_name, dev_name_like_eth0);
    if(ioctl(socketfd, SIOCGIFBRDADDR/*SIOCGIFADDR*/,&ifr) < 0){
        perror("ioctl error\n");
        return -1;
    }
    sin = (struct sockaddr_in *)&(ifr.ifr_addr);
//set supporting broadcast
	int i=1;
    socklen_t len = sizeof(i);
	setsockopt(socketfd,SOL_SOCKET,SO_BROADCAST,&i,len);
//initial sendding
	memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr = sin->sin_addr;
    server_addr.sin_port = htons(port);
    addr_len=sizeof(server_addr);
//send
	if(sendto(socketfd, PASSWORD, sizeof(PASSWORD),0,(struct sockaddr*)&server_addr,addr_len) < 0){
		perror("sendErr");
		return -1;
	}
//wait for timeout
	ms_to_wait = time_out_ms;
	wait_flag = 0;
	if (pthread_create(&wait_thread, NULL, thread_to_wait,NULL)!=0) {
        printf("Create thread error!\n");
        return -1;
    }
//wait for devices echo
	while(wait_flag == 0){
		
		if (recvfrom(socketfd, buf, 64, 0, (struct sockaddr*)&server_addr, &addr_len) < 0){
            perror("recvErr");
            return -1;
        }else{	 
			printf("recv from: %s\n", inet_ntoa(server_addr.sin_addr));
			fputs(inet_ntoa(server_addr.sin_addr), deviceFile);
			fputc('\n', deviceFile);
			//if (pthread_create(&tmp_thread, NULL, thread_to_process, NULL) != 0) {
			//	printf("Create thread error!\n");
			//	return -1;
			//}
		}
	}
	printf("I'v wait %d ms, program done!\n", ms_to_wait);
	close(socketfd);
	return 0;
}
int main(int argc, char* argv[])
{
	if(argc != 3){
		printf("sample usage: %s eth0 file.txt\n", argv[0]);
		return 0;
	}
	deviceList = argv[2];
	query_for_devices(LISTEN_PORT, argv[1], 2000);
}

