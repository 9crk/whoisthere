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


#define MAX_CLIENT_NUM 256

#include"libscan.h"

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
	if(deviceFile > 0)fclose(deviceFile);
	//exit(0);
	//pthread_exit(NULL);
}


int query_for_devices(const char* dev_name_like_eth0,IPList** list,int *num,int time_out_ms,char* name)
{
    socklen_t addr_len;
	int ret,socketfd;
    char buf[64];
    struct sockaddr_in *sin;
    struct ifreq ifr;
	pthread_t wait_thread, tmp_thread;
	int cnt = 0;
	*list = (IPList*)malloc(MAX_CLIENT_NUM*sizeof(IPList));
	//printf("malloc = %p\n",*list);
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
    server_addr.sin_port = htons(LISTEN_PORT);
    addr_len=sizeof(server_addr);
//send
	if(sendto(socketfd, name, strlen(name),0,(struct sockaddr*)&server_addr,addr_len) < 0){
		perror("sendErr");
		return -1;
	}
//set socket timeout
	struct timeval tv_out;
    tv_out.tv_sec = 1;//等待10秒
    tv_out.tv_usec = 0;
	setsockopt(socketfd,SOL_SOCKET,SO_RCVTIMEO,&tv_out, sizeof(tv_out));
//wait for timeout
	ms_to_wait = time_out_ms;
	wait_flag = 0;
	if (pthread_create(&wait_thread, NULL, thread_to_wait,NULL)!=0) {
        printf("Create thread error!\n");
        return -1;
    }
//wait for devices echo
	while(wait_flag == 0){
		memset(buf,0,20);
		ret = recvfrom(socketfd, buf, 64, 0, (struct sockaddr*)&server_addr, &addr_len);
		if (ret < 0){
            break;
			//perror("recvErr");
            //return -1;
        }else{
			if(strcmp(name,buf) == 0){
			//	printf("recv from: %s\n", inet_ntoa(server_addr.sin_addr));
				list[cnt]->ip = (char*)malloc(16);
				//printf("malloc = %p\n",list[cnt]->ip);
				sprintf(list[cnt]->ip,"%s",inet_ntoa(server_addr.sin_addr));
				cnt++;
			}
		}
	}
	//printf("I'v wait %d ms, program done!\n", ms_to_wait);
	close(socketfd);
	*num = cnt;
	return 0;
}

void de_query_for_devices(IPList*list,int num)
{
	int i;
	for(i=0;i<num;i++){
		//printf("de malloc = %p\n",list[i].ip);
		free(list[i].ip);
	}
	//printf("de malloc = %p\n",list);
	free(list);
}
#if 0
int main(int argc, char* argv[])
{
	IPList *ipListHead;
	int i,num;
	
	query_for_devices("eth0",&ipListHead,&num,2000,"ENC");
	printf("cnt = %d\n",num);	
	for(i=0;i<num;i++){
		printf("IP:%s\n",ipListHead[i].ip);
	}
	de_query_for_devices(ipListHead,num);
}
#endif
