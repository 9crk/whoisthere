typedef struct IPList{
	char *ip;
}IPList;

#define LISTEN_PORT	9521
void startUdpEcho(char* devName,char* echoContent);
void de_query_for_devices(IPList*list,int num);
int query_for_devices(int port, const char* dev_name_like_eth0,IPList** list,int *num,int time_out_ms,char* name);