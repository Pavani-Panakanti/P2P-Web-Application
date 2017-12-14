/* Functions used by the CLI Thread */

#include "NodeCommon.h"

extern int client_sockets[MAX_NEIGHBORS];
extern std::map <string, int> send_peer;
extern std::map <int, string> recv_peer;
extern std::multimap <string, struct hash_data> hash_table;

// Functions to send msgs from server to client

void nodeConPeer(int * client_sockets, struct node_mssg * mssg)
{
	if((nodeSend(client_sockets[0],(struct node_mssg *)mssg)) == FAILURE)
	{
		printf("\nNODE API: CONNECION REQUEST TO CLNT THRD FAILED\n");
		return;
	}
	return;
}

void nodePublishHash(int * client_sockets, struct node_mssg * mssg)
{
	if((nodeSend(client_sockets[0],(struct node_mssg *)mssg)) == FAILURE)
	{
		printf("\nNODE API: CONNECION REQUEST TO CLNT THRD FAILED\n");
		return;
	}
	return;
}

void nodeunPublishHash(int * client_sockets, struct node_mssg * mssg)
{
	if((nodeSend(client_sockets[0],(struct node_mssg *)mssg)) == FAILURE)
	{
		printf("\nNODE API: CONNECION REQUEST TO CLNT THRD FAILED\n");
		return;
	}
	return;
}

void nodeSendTerm(int * client_sockets, struct node_mssg * mssg)
{
	if((nodeSend(client_sockets[0],(struct node_mssg *)mssg)) == FAILURE)
	{
		printf("\nNODE API: CONNECION REQUEST TO CLNT THRD FAILED\n");
		return;
	}
	return;
}

void nodeSendFile(int * client_sockets, struct node_mssg * mssg)
{
	if((nodeSend(client_sockets[0],(struct node_mssg *)mssg)) == FAILURE)
	{
		printf("\nNODE API: CONNECION REQUEST TO CLNT THRD FAILED\n");
		return;
	}
	return;
}

