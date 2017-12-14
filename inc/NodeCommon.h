#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <map>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <openssl/sha.h>

using namespace std;

#define FAILURE 0
#define SUCCESS 1
#define TRUE 1
#define MAX_NEIGHBORS 10
#define CONNECT_PEER 100
#define PUBLISH_HASH 200
#define UNPUBLISH_HASH 300
#define MAX_COMMANDS 3

//struc of mssg to pass between peers
struct node_mssg
{
	int internal;
	int external;
	int data;
	int recv_fd;
	char ip_addr[20];
	int port;
	char  hash[100];
	char filetype[10];
};

//struct of metadata
struct hash_data
{
	char ip_addr[20];
	int port;
	char file_type[10];
};

int nodeBind(char * ipaddr, int port);
int nodeRecv(int sockfd, struct node_mssg * pmssg);
int nodeConnect(const char* ipaddr, unsigned short port);
int nodeSend(int sockfd, struct node_mssg * pmssg);

void addentry(struct node_mssg* pmssg);
void delentry(struct node_mssg* pmssg);
void getIP();
void hashFile(char * file, unsigned char * hash);
