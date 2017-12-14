/* Main function creates the threads - Cli, Clnt, Server and Http  */

#include "NodeMain.h"
#include "NodeCommon.h"

extern unsigned short ex_port;

int main(int argc, char * argv[])
{
	pthread_t thserver;
	pthread_t thclient;
	pthread_t thcli;
	pthread_t thhttp;

	unsigned short in_cli;
	unsigned short in_clnt;
	unsigned short in_http;

	ex_port = atoi(argv[1]);
	in_clnt = ex_port+1;
	in_cli = in_clnt+1;
	in_http = atoi(argv[2]);

	getIP();

	//Creates for threads for Server, client, cli and http

	if(pthread_create(&thcli,NULL,nodeCli,&in_cli) != 0)
	{
		perror("\nNODE MAIN: CLI THREAD CREATION FAIL\n");
		return FAILURE;
	}

	if(pthread_create(&thserver,NULL,nodeServ,&ex_port) != 0)
	{
		perror("\nNODE MAIN: SERVER THREAD CREATION FAIL\n");
		return FAILURE;
	}

	if(pthread_create(&thclient,NULL,nodeClnt,&in_clnt) != 0)
	{
		perror("\nNODE MAIN: CLNT THREAD CREATION FAIL\n");
		return FAILURE;
	}

	if(pthread_create(&thhttp, NULL, nodeHttp, &in_http) != 0)
	{
		perror("\nNODE MAIN: HTTP THREAD CREATION FAILED\n");
		return FAILURE;
	}

	pthread_join(thcli,NULL);
	pthread_join(thclient,NULL);
	pthread_join(thserver,NULL);
	pthread_join(thhttp, NULL);

	return SUCCESS;
}
