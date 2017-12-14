/* CLI Thread takes user input from Node CLI and performs necessary actions based on the commands given */

#include "NodeCommon.h"
#include "NodeCli.h"

#define CLI_DELIMIT " \n"
#define CLI_SIZE 1024

extern unsigned short ex_port;
extern std::map <string, int> send_peer;
extern std::map <int, string> recv_peer;
extern std::multimap <string, struct hash_data> hash_table;
extern std::map <string, int> peers;
extern string sys_ip;
extern std::map <string, string> myhash;

int node_connect(char **args);
int node_hi(char **args);
int node_exit(char **args);
int node_publish(char **args);
int node_unpublish(char **args);
int node_show_peers(char** args);
int node_show_metadata(char** args);
int node_show_published(char** args);

//stores the commands handled by NODE CLI
string builtin_str[6] = {
	"peer",
	"publish",
	"unpublish",
	"show_peers",
	"show_metadata",
	"show_published"
};

//Functions which handle the above NODE CLI commands
int (*builtin_func[]) (char **) = {
	&node_connect,
	&node_publish,
	&node_unpublish,
	&node_show_peers,
	&node_show_metadata,
	&node_show_published
};

// Handles the peer command, parses info into struct mssg and sends it to server
int node_connect(char **args)
{
	if(args[1] == NULL || args[2] == NULL)
	{	
		cout << "Incorrect Input Please Try again !" << endl;
		return SUCCESS;
	}
	struct node_mssg mssg;
	mssg.internal = 1;	//For the server to know that msg was from internal thread
	mssg.external = 0;
	struct hostent * record = gethostbyname(args[1]);
	if(record == NULL)
	{
		perror("gethostbyname");
		printf("%s is unavailable\n", args[1]);
		exit(1);
	}
	struct in_addr * address = (in_addr * )record->h_addr;
	string ip_address = inet_ntoa(* address);
	strcpy(mssg.ip_addr,ip_address.c_str());
	mssg.data = CONNECT_PEER;		//msg type
	mssg.port = atoi(args[2]);
	string ip(mssg.ip_addr);
	peers.insert(pair<string, int>(ip,mssg.port));		// inserting peer info into map
	if(nodeSend(gsockfd,(struct node_mssg *)&mssg) == FAILURE)   // sending the info to server
		printf("\nNODE CLI: SEND TO SERVER FAILED\n");
	return SUCCESS;
}

// Handles the publish command, parses info into struct mssg and sends it to server
int node_publish(char **args)
{
	if(args[1] == NULL)
	{
		cout << "Incorrect Input Please Try again !" << endl;
		return SUCCESS;
	}
	struct node_mssg mssg;
	strcpy(mssg.hash, args[1]);
	mssg.hash[strlen(mssg.hash)] = '\0';
	string filename(mssg.hash);
	char type[100];
	char type1[100];
	strcpy(type,mssg.hash);
	strcpy(type, strtok(type, "."));
	strcpy(type,strtok(NULL, "."));
	strcpy(mssg.filetype ,type);
	char file[50];
	strcpy(file,mssg.hash);
	unsigned char hash[100];
	hashFile(file,hash);    // converting file to be published to hash
	strcpy(mssg.hash, (char *)hash);
	string hashname(mssg.hash);
	myhash.insert(pair<string, string>(filename,hashname));      
	mssg.internal = 1;
	mssg.external = 0;
	mssg.data = PUBLISH_HASH;
	mssg.port = ex_port;
	strcpy(mssg.ip_addr, sys_ip.c_str());  //storing systems ip address in struct and publishing it
	multimap<string,struct hash_data>:: iterator it;
	it = hash_table.find(hashname);
	if(it == hash_table.end() || ((it->second).ip_addr != mssg.ip_addr)){	//sending it to server to publish only if it is not published before
		if(nodeSend(gsockfd,(struct node_mssg *)&mssg) == FAILURE)
			printf("\nNODE CLI: SEND TO SERVER FAILED\n");
		return SUCCESS;
	}
}

// Handles the unpublish command, parses info into struct mssg and sends it to server
int node_unpublish(char **args)
{
	if(args[1] == NULL)
	{
		cout << "Incorrect Input Please Try again !" << endl;
		return SUCCESS;
	}
	struct node_mssg mssg;
	strcpy(mssg.hash, args[1]);
	strcpy(mssg.ip_addr, sys_ip.c_str()); 
	mssg.internal = 1;
	mssg.external = 0;
	mssg.data = UNPUBLISH_HASH;
	string filename(mssg.hash);
	string hashname;
	map<string, string> :: iterator itr;
	itr = myhash.find(filename);
	if(itr != myhash.end())      
		hashname = itr->second;
	else{
		cout << "not present in table to unpublish " << endl;
		return 1;
	}
	strcpy(mssg.hash, hashname.c_str());
	multimap<string,struct hash_data>:: iterator it;
	it = hash_table.find(hashname);    // checking if its published before to unpublish
	if(it != hash_table.end()){		
		if(nodeSend(gsockfd, (struct node_mssg *)&mssg) == FAILURE)
			printf("\nNODE CLI: SEND TO SERVER FAILED\n");
		return SUCCESS;
	}
}

// Handles the show_peers command
int node_show_peers(char** args)
{
	cout << "IP Address" << "    " << "Port" << endl;
	map<string, int>:: iterator it;        //prints the peer info from the map
	for(it=peers.begin(); it!=peers.end(); it++)
	{
		cout << it->first << "    " << it->second << endl;
	}
	return SUCCESS;
}

// Handles the show_metadata command
int node_show_metadata(char** args)
{
	cout << "Hash" << "                          " << "Filetype" << "    " << "IP Address" << "    " << "Port" << endl;
	multimap<string, struct hash_data>:: iterator it;    // prints metadata stored in hash_table. if ip add is 0 then it is published by this node, if not 0 then published by some other node in p2pweb
	for(it=hash_table.begin(); it!=hash_table.end(); it++)
	{
		string ip((it->second).ip_addr);
		if(ip!= sys_ip)
			cout << it->first << "    " << (it->second).file_type << "    " << (it->second).ip_addr << "    " << (it->second).port << endl;
	}
	return SUCCESS;
}

// Handles the show_published command
int node_show_published(char** args)
{
	cout << "Filename" << "    " << "Hash" << "                          " << "FileType" <<  endl;
	multimap<string, struct hash_data>:: iterator it;
	map<string,string> :: iterator it1;
	for(it=hash_table.begin(); it!=hash_table.end(); it++)
	{
		string ip((it->second).ip_addr);
		if(ip == sys_ip){
			string hashname = it->first;
			string filename;
			for(it1=myhash.begin(); it1!=myhash.end(); it1++){
				if(it1->second == hashname)
					filename = it1->first;
			}
			cout << filename << "    " << hashname << "    " << (it->second).file_type << endl;
		}
	}
	return SUCCESS;
}

//Reads each line typed in NODE CLI
char * nodeReadLine()
{
	char *line  = NULL;
	size_t cli_size = 0;
	getline(&line, &cli_size, stdin);
	return line;
}

//Splits each input read line into tokens of each word
char** nodeSplitLine(char * line)
{
	char** tokens = (char **)malloc(CLI_SIZE * sizeof(char *));
	char *token;
	int positions = 0;
	token = strtok(line,CLI_DELIMIT);
	tokens[0] = NULL;
	while(token != NULL)
	{
		tokens[positions] = token;
		positions++; 
		token = strtok(NULL,CLI_DELIMIT);
	}
	return tokens;
}

//Executes for each input line typed in NODE CLI
int nodeExecute(char **args)
{
	int i;
	if (args[0] == NULL) {
		// An empty command was entered.
		return 1;
	}
	string command = args[0];
	for (i = 0; i < 6; i++) {
		if (command == builtin_str[i]) {
			return (*builtin_func[i])(args);
		}
	}
	return 1;
}

//Handles the NODE CLI
void nodeCliLoop(void)
{
	char *line;
	char **args;
	int status;

	do
	{
		printf("NODE>");
		line = nodeReadLine();
		args = (char **)malloc(CLI_SIZE * sizeof(char *));
		args = nodeSplitLine(line);
		status = nodeExecute(args);

		free(line);
		free(args);
	}while(status);
}

// Creates a connection between cli thread and server thread
void connServTh(const char * ip, unsigned short port)
{
	do
	{
		sleep(3);
		gsockfd = nodeConnect(ip,port);

		if(gsockfd == FAILURE)
			printf("\nNODE CLI : CONNECTION TO SERV THRD FAILED\n");

		sleep(1);
	}while(gsockfd == FAILURE);
}

//Main function in cli thread
void * nodeCli(void * args)
{
	unsigned short *temp;
	unsigned short in_cli;
	temp = (unsigned short *)args;
	in_cli = *temp;
	connServTh(sys_ip.c_str(),in_cli);
	nodeCliLoop();
	return NULL;
}


