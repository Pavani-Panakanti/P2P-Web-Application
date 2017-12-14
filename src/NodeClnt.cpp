/* Clnt thread takes care of publish and unpublish of metadata info from its peers */

#include"NodeClnt.h"

extern multimap<string, struct hash_data> hash_table;
extern int client_sockets[MAX_NEIGHBORS];
extern map<string, int> send_peer;
extern map<int, string> recv_peer;
extern string sys_ip;
extern std::map <string, string> myhash;

//initializes fd array to 0
void initClSockfds(int * sockfds)
{
	int i;
	for(i=0;i<MAX_NEIGHBORS;i++)
		sockfds[i] = 0;
}

//establishes connection with peer
void estblshConn(char* ip, unsigned short port, int * sockfds)
{
	int i;
	for(i=0;i<MAX_NEIGHBORS;i++)
	{
		if(sockfds[i] == 0)
			break;
	}
	sockfds[i] = nodeConnect(ip,port);
	char buff[1024];
	bzero(buff,1024);
	int recv_bytes = recv(sockfds[i], buff, sizeof(buff),0);   //after connection receiving the peers metadata for syncronization
	int size;
	sscanf(buff,"%d", &size);
	bzero(buff, 1024);
	for(int i=0; i<size; i++){
		recv(sockfds[i],buff,sizeof(buff),0);
		string hash(buff);
		struct hash_data hd;
		recv(sockfds[i],(void*)&hd,sizeof(struct hash_data),0);
		hash_table.insert(pair<string, struct hash_data>(hash,hd));
	}
	string ip_addr(ip);
	if(sockfds[i] == 0)
		printf("\nNODE CLIENT: CONNECTION TO PEER %s AT PORT %d FAILED\n",ip,(int)port);
	else{
		send_peer.insert(std::pair<string,int>(ip_addr, sockfds[i]));   //storing peer information
	}
}

//Recieves published metadata from server, stores it and publishes to peers
void PublishPeer(int * sockfds,struct node_mssg * pmssg)
{
	int i = 0;
	string ipaddr(pmssg->ip_addr);
	map<string, struct hash_data> :: iterator itr;
	itr = hash_table.find(pmssg->hash);
	if(itr != hash_table.end()){            // publishes only if its not already present in its table - to avoid loop
		struct hash_data hd = itr->second;
		string ip(hd.ip_addr);
		if((ip == ipaddr) || (ip == sys_ip)){
			return;
		}
	}
	addentry(pmssg);    // stores the metadata in its table
	if(pmssg->internal == 1){    //if published by the node itself, sends to all its peers
		pmssg->internal = 0;
		pmssg->external = 1;
		while(sockfds[i] != 0)
		{
			if(nodeSend(sockfds[i],pmssg) == 0)
			{
				printf("\nNODE CLIENT: SEND TO PEER FAILED\n");
			}
			else{
				i++;
			}
		}
	}
	else if(pmssg->external == 1){    //if published by external peer, sends to all peers except the one from which it has recieved.
		while(sockfds[i] != 0)
		{      
			map<int,string>:: iterator it;
			it = recv_peer.find(pmssg->recv_fd);
			if(it != recv_peer.end()){
				map<string,int>:: iterator it1;
				it1 = send_peer.find(it->second);
				if(it1 != send_peer.end()){
					if(sockfds[i] != it1->second){
						if(nodeSend(sockfds[i],pmssg) == 0)
						{       
							printf("\nNODE CLIENT: SEND TO PEER FAILED\n");
						}
					}
				}		
			}
			i++;	
		}	
	}	
}	

//Recieves the unpublish msg from server, deletes from table and sends to its peers
void unPublishPeer(int * sockfds,struct node_mssg * pmssg)
{
	int i = 0;
	string ipaddr(pmssg->ip_addr);
	map<string, struct hash_data> :: iterator itr;
	itr = hash_table.find(pmssg->hash);
	if(itr != hash_table.end()){         //sends it to its peers only if it hash metadata to unpublish
		struct hash_data hd = itr->second;
		string ip(hd.ip_addr);
		if((ip == ipaddr) || (ip == sys_ip)){
			delentry(pmssg);
			if(pmssg->internal == 1){          //If unpublish is by the node itself, sends to all its peers
				pmssg->internal = 0;
				pmssg->external = 1;
				while(sockfds[i] != 0)
				{
					if(nodeSend(sockfds[i],pmssg) == 0)
					{
						printf("\nNODE CLIENT: SEND TO PEER FAILED\n");
					}
					else{
						i++;
					}
				}
			}
			else if(pmssg->external == 1){     //If msg is from external peer, sends to all peers except from the one which it recieved the unpublish msg
				while(sockfds[i] != 0)
				{
					map<int,string>:: iterator it;
					it = recv_peer.find(pmssg->recv_fd);
					if(it != recv_peer.end()){
						map<string,int>:: iterator it1;
						it1 = send_peer.find(it->second);
						if(it1 != send_peer.end()){
							if(sockfds[i] != it1->second){
								if(nodeSend(sockfds[i],pmssg) == 0)
								{
									printf("\nNODE CLIENT: SEND TO PEER FAILED\n");
								}
							}
						}
					}
					i++;
				}
			}
		}
	}
}

//Main function in Client thread
void * nodeClnt(void * args)
{
	struct node_mssg mssg;
	int sockfds[MAX_NEIGHBORS];
	int infd;
	unsigned short *temp,in_clnt; 
	temp = (unsigned short *)args;
	in_clnt = *temp;
	initClSockfds(sockfds);

	do
	{
		sleep(3);
		infd = nodeConnect(sys_ip.c_str(),in_clnt);

		if(infd == FAILURE)
		{
			printf("\nNODE CLIENT: INTERNAL CONNECT FAILED\n");
			return NULL;
		}

		sleep(1);
	}while(infd == FAILURE);
	while(true)
	{
		fd_set readfds;
		FD_ZERO(&readfds);
		FD_SET(infd,&readfds);
		int result = select(infd+1, &readfds, NULL, NULL, NULL);    //listens for the msgs from the server
		if(result > 0 && FD_ISSET(infd, &readfds)){
			if(nodeRecv(infd,&mssg) == SUCCESS)      //Calls the functions to handle based on the data in mssg
			{
				switch(mssg.data)
				{
					case CONNECT_PEER: 
						{
							estblshConn(mssg.ip_addr,mssg.port,sockfds);
							break;
						}
					case PUBLISH_HASH:
						{
							PublishPeer(sockfds,&mssg);
							break;
						}
					case UNPUBLISH_HASH:
						{
							unPublishPeer(sockfds,&mssg);
							break;
						}
				}
			}
			else
				printf("\nNODE CLIENT: RECEIVE FAILED\n");
		}
	}
	return NULL;
}
