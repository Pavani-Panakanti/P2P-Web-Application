/* Server listens for the incoming peer connections, already connected peers and from CLI */

#include "NodeServ.h"

extern int client_sockets[MAX_NEIGHBORS];
extern std::map <string, int> send_peer;
extern std::map <int, string> recv_peer;
extern std::multimap <string, struct hash_data> hash_table;
extern string sys_ip;

//initialises the fd array to 0
void initSerSockfds(int * client_sockets)
{
	int i;
	for(i=0;i<MAX_NEIGHBORS;i++)
		client_sockets[i] = 0;
	return;
}

//Main function in server thread
void * nodeServ(void * args)
{
	struct node_mssg mssg;
	int i,j;
	int in_clnt_fd,in_cli_fd,exfd,acceptfd,sd,max_sd,activity;
	struct sockaddr_in cliaddr;
	socklen_t clilen=0;
	fd_set readfds;
	unsigned short *temp;
	unsigned short in_clnt;
	unsigned short in_cli;
	unsigned short ex_port;

	temp = (unsigned short *)args;

	ex_port = *temp;
	in_clnt = ex_port+1;
	in_cli = in_clnt+1;
	initSerSockfds(client_sockets);

	char ip_addr[20];
	strcpy(ip_addr,sys_ip.c_str()); 
	in_clnt_fd = nodeBind(ip_addr, in_clnt);
	if(in_clnt_fd == FAILURE)
	{
		printf("\nNODE SERVER: BIND FAIL\n");
		return NULL;
	}

	in_cli_fd = nodeBind(ip_addr,in_cli);
	if(in_cli_fd == FAILURE)
	{
		printf("\nNODE SERVER: BIND FAIL FOR PORT:%d\n",in_cli);
		return NULL;
	}

	//TCP connection between server-cli and server-client for communication
	in_clnt_fd = accept(in_clnt_fd,(struct sockaddr *)&cliaddr,&clilen);
	if(in_clnt_fd < 0)
	{
		printf("\nNODE SERV: CLIENT THRD ACCEPT FAILED\n");
		return FAILURE;
	}
	client_sockets[0] = in_clnt_fd;  

	in_cli_fd = accept(in_cli_fd,(struct sockaddr *)&cliaddr,&clilen);
	if(in_cli_fd < 0)
	{
		printf("\nNODE SERV: CLI THRD ACCEPT FAILED\n");
		return FAILURE;
	}
	client_sockets[1] = in_cli_fd;

	exfd = nodeBind(ip_addr,ex_port);
	if(exfd == FAILURE)
	{
		printf("\nNODE SERVER: BIND FAIL FOR PORT:%d\n",ex_port);
		return NULL;
	}

	while(TRUE)
	{
		FD_ZERO(&readfds);

		FD_SET(exfd,&readfds);
		max_sd = exfd;

		for(i=0;i<MAX_NEIGHBORS;i++)
		{
			sd = client_sockets[i];

			if(sd > 0)
				FD_SET(sd,&readfds);

			if(sd > max_sd)
				max_sd = sd;
		}

		activity = select(max_sd + 1 , &readfds , NULL , NULL , NULL);  //listens for new connections, from existimg peer connections and CLI thread 
		if(activity < 0)
		{
			if(errno == EINTR)
				continue;
			perror("\nNODE SERVER: SELECT FAIL");
			return FAILURE;
		}

		if(FD_ISSET(exfd,&readfds))
		{
			acceptfd = accept(exfd,(struct sockaddr *)&cliaddr,&clilen);     //accepts new connections from peers
			if(acceptfd < 0)
			{
				printf("\nNODE SERVER ACCEPT FAILED\n");
				return NULL;
			}

			struct sockaddr* peeraddr = ((struct sockaddr *)&cliaddr);       // sends its metadata content for synchronization
			char* addr = inet_ntoa(((struct sockaddr_in*)peeraddr)->sin_addr);
			string peerip(addr);
			recv_peer.insert(pair<int,string>(acceptfd,peerip));
			multimap<string, struct hash_data> :: iterator it;
			char buff[1024];
			sprintf(buff,"%d",hash_table.size());
			send(acceptfd, buff, sizeof(buff),0);
			bzero(buff, 1024);
			for(it=hash_table.begin(); it != hash_table.end(); it++){
				strcpy(buff, (it->first).c_str());
				send(acceptfd, buff, sizeof(buff),0);
				struct hash_data hd = it->second;
				send(acceptfd, (void*)&hd, sizeof(struct hash_data),0);
			}

			for(i=0;i<MAX_NEIGHBORS;i++)
			{
				if(client_sockets[i] == 0)
				{
					client_sockets[i] = acceptfd;
					break;
				}
			}
		}

		for(i=0;i<MAX_NEIGHBORS;i++)
		{
			sd = client_sockets[i];

			if(FD_ISSET(sd,&readfds))      // Recieved msg from some peer
			{
				if(nodeRecv(sd,&mssg) == FAILURE)
				{
					perror("\nNODE SERVER: ERROR RECV\n");
					return NULL;
				}
				else
				{
					mssg.recv_fd = sd;
					for(j=0;j<MAX_COMMANDS;j++)
					{
						if(mssg.data == cmd_id[j]){
							(*cmd_handler[j])(client_sockets,&mssg);   //calls the function to handle based on data in msg
							break;
						}
					}           
				}
			}
		}
	}

	return NULL;

}
