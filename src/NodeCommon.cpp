#include "NodeCommon.h"

extern int client_sockets[MAX_NEIGHBORS];
extern std::map <string, int> send_peer;
extern std::map <int, string> recv_peer;
extern std::multimap <string, struct hash_data> hash_table;
extern string sys_ip;

//connect call
int nodeConnect(const char* ipaddr, unsigned short port)
{
	int sockfd;
	struct sockaddr_in servaddr;

	sockfd = socket(AF_INET,SOCK_STREAM,0);
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);

	if(inet_pton(AF_INET,ipaddr,&servaddr.sin_addr)<=0)
	{
		printf("\nNODE COMMMON: INET_PTON FAIL FOR IPADDRESS : %s\n",ipaddr);
		return FAILURE;
	}

	if(connect(sockfd, (struct sockaddr*)&servaddr,sizeof(struct sockaddr_in))<0)
	{
		printf("\nNODE COMMON: CONNECT FAIL FOR IPADDRESS: %s, PORT : %d\n",ipaddr,(int)port);
		perror("");
		return FAILURE;
	}

	return sockfd;
}

//send call
int nodeSend(int sockfd, struct node_mssg * pmssg)
{
	if(send(sockfd,(void *)pmssg,sizeof(struct node_mssg),0) < 0)
	{  
		printf("\nNODE COMMON:  SEND ERROR:");
		perror("");
		return FAILURE;
	}

	return SUCCESS;
}

//Recv call
int nodeRecv(int sockfd, struct node_mssg * pmssg)
{
	if(recv(sockfd,(void *)pmssg,sizeof(struct node_mssg),0) < 0)
	{
		printf("\nNODE COMMON:  RECV ERROR:");
		perror("");
		return FAILURE;
	}

	return SUCCESS;
}

//Bind call
int nodeBind(char * ipaddr, int port)
{
	struct sockaddr_in servaddr;
	int listenfd;
	int enablesock = 0;

	listenfd = socket(AF_INET,SOCK_STREAM,0);

	if(listenfd < 0)
	{
		printf("\nNODE BIND SOCKET CREATION FAIL\n");
		return FAILURE;
	}

	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &enablesock, sizeof(int)) < 0)
		printf("\nNODE BIND SOCKOPT FAIL\n");

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);

	if(inet_pton(AF_INET,ipaddr,&servaddr.sin_addr)<=0)
	{
		printf("\nNODE BIND INET_PTON FAIL\n");
		perror("");
		return FAILURE;
	}

	if ( (bind(listenfd,(struct sockaddr *)&servaddr, sizeof(servaddr)))<0)
	{
		printf("\nNODE BIND FAIL:");
		perror("");
		return FAILURE;
	}

	if((listen(listenfd,100))<0)
	{
		printf("\nNODE BIND LISTEN FAIL\n");
		return FAILURE;
	}

	return listenfd;
}

//To add entry into hash_table - metadata
void addentry(struct node_mssg* pmssg)
{
	struct hash_data my_data;
	strcpy(my_data.file_type,pmssg->filetype);
	if(pmssg->external == 1)
	{
		strcpy(my_data.ip_addr,pmssg->ip_addr);
		my_data.port = pmssg->port;
	}
	else if(pmssg->internal == 1)
	{
		string ip = sys_ip;
		strcpy(my_data.ip_addr,ip.c_str());
		my_data.port = 0;
	}
	string hash(pmssg->hash);
	hash_table.insert(pair<string,struct hash_data>(hash,my_data));
}

//To delete entry from hash_table
void delentry(struct node_mssg* pmssg)
{
	string hash(pmssg->hash);
	hash_table.erase(hash);
}

//To get system ip
void getIP()
{
	int fd;
	struct ifreq ifr;
	string ip;
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name, "eno16777728", IFNAMSIZ-1);
	ioctl(fd, SIOCGIFADDR, &ifr);
	close(fd);
	sys_ip.assign(inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
}

//Converts file to hash
void hashFile(char * file, unsigned char * hash)
{
	FILE *fptr;
	unsigned char data[11] = {0};
	unsigned char hashhex[20];
	char temp[50];
	int numread=0;
	int i;
	SHA1(data,10, hash);
	SHA_CTX ctx;
	SHA1_Init(&ctx);

	strcpy(temp,"");
	fptr = fopen(file,"r");

	while(feof(fptr) == 0)
	{
		numread =  fread(data, sizeof(unsigned char), 10, fptr);
		if(numread > 0)
		{
			SHA1_Update(&ctx,data, 10);
		}
	}
	fclose(fptr);
	SHA1_Final(hashhex, &ctx);
	for(i=0;i<20;i++)
		sprintf(temp+strlen(temp),"%x",hashhex[i]);
	strcpy((char *)hash,temp);
}

