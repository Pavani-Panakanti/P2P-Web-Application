#include <iostream>
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include <map>
#include <string.h>

using namespace std;

#define SUCCESS 1
#define FAILURE 0


unsigned short ex_port;
int client_sockets[10];
map <string, int> send_peer;   //map to store peer info
map <int, string> recv_peer;   //map to store peer info
multimap <string, struct hash_data> hash_table;   //map to store metadata
map <string, int> peers;       //map to store peers
string sys_ip;                 //system ip
map <string, string> myhash;   //map to store hash of file

void * nodeCli(void*);
void * nodeServ(void *);
void * nodeClnt(void *);
void * nodeHttp(void *);
