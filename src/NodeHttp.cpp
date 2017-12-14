#include <iostream>
#include "../inc/NodeHttp.h"

using namespace std;

extern map<string, string> myhash;
extern multimap<string, struct hash_data> hash_table;
unsigned short in_http;
string filetype;
extern string sys_ip;

//To get requested file from peer
int  getfile(string hash)
{
	multimap<string, struct hash_data> :: iterator it;
	it = hash_table.find(hash);
	if(it == hash_table.end())    //if file not present in table returns - not published
	{
		return 0;
	}
	struct hash_data data = it->second;
	string ip(data.ip_addr);
	string filet(data.file_type);
	filetype = filet;
	if(ip != sys_ip)               // published by some peer - makes tcp connectiona and retrives the file
	{
		int port = data.port;
		port = in_http + 10;;
		int server_fd, new_socket, valread;
		struct sockaddr_in address;
		int addrlen = sizeof(address);
		char buff[100];
		bzero(buff, 100);
		strcpy(buff,hash.c_str());
		char buffer[1024] = {0};
		if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
		{
			perror("socket failed");
			exit(EXIT_FAILURE);
		}
		address.sin_family = AF_INET;
		if((inet_pton(AF_INET, ip.c_str() , &address.sin_addr))<=0)
			cout << "error in inet_pton" << endl;
		address.sin_port = htons(port);

		if ((new_socket = connect(server_fd, (struct sockaddr *)&address, sizeof(address)))<0)
		{
			perror("connect");
			exit(EXIT_FAILURE);
		}
		cout << "connected to peer for file" << endl;
		if(send(server_fd,buff,sizeof(buff),0)<=0){
			cout << "send failed for file" << endl;
			perror("send");
		}

		char send_str [256];
		int f; 
		ssize_t sent_bytes, rcvd_bytes, rcvd_file_size;
		int recv_count; 
		char recv_str[256]; 
		remove("recv.html");
		if ( (f = open("recv.html", O_WRONLY|O_CREAT, 0644)) < 0 )
		{
			perror("error creating file");
			return -1;
		}

		recv_count = 0; 
		rcvd_file_size = 0; 
		if(fcntl(server_fd, F_SETFL, fcntl(server_fd, F_GETFL) | O_NONBLOCK) < 0)
			cout << " nonblocking fail " << endl;
		sleep(2);
		while ( (rcvd_bytes = recv(server_fd, recv_str, 256, 0)) > 0 )
		{
			recv_count++;
			rcvd_file_size += rcvd_bytes;

			if (write(f, recv_str, rcvd_bytes) < 0 )
			{
				perror("error writing to file");
				return -1;
			}
		}
		close(f); 
		printf("Client Received: %d bytes in %d recv(s)\n", rcvd_file_size,  recv_count);
		shutdown(server_fd, SHUT_RD);
		close(server_fd);
	}
	else        //published by the node itself
	{	
		FILE *fptr1, *fptr2;
		map<string, string> :: iterator it;
		string filename;
		for(it=myhash.begin(); it!=myhash.end(); it++){
			if(it->second == hash)
				filename = it->first;
		}
		char filen[100];
		strcpy(filen,filename.c_str());
		char  c;
		fptr1 = fopen(filen, "r");
		if (fptr1 == NULL)
		{
			printf("Cannot open file %s \n", filen);
			exit(0);
		}
		remove("recv.html");
		fptr2 = fopen("recv.html", "w");
		if (fptr2 == NULL)
		{
			printf("Cannot open file recv.html\n");
			exit(0);
		}
		int size = 256;
		char buf[size];
		while( (size = fread(buf, 1, sizeof(buf), fptr1) ) > 0)
			fwrite(buf, 1, size, fptr2);

		fclose(fptr1);
		fclose(fptr2);
	}
	return 1;;
}

void * nodeHttp(void * args){
	unsigned short *temp;
	temp = (unsigned short *)args;
	in_http = *temp;
	int http_fd, new_socket, valread;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char buffer[1024] = {0};

	if ((http_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( in_http );

	if (bind(http_fd, (struct sockaddr *)&address,
				sizeof(address))<0)
	{
		cout << "Bind failed for http thread" << endl;
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if (listen(http_fd, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}

	int file_fd;
	struct sockaddr_in address1;
	if ((file_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{       
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	address1.sin_family = AF_INET;
	address1.sin_addr.s_addr = INADDR_ANY;
	address1.sin_port = htons( in_http + 10 );

	if (bind(file_fd, (struct sockaddr *)&address1,
				sizeof(address1))<0)
	{
		cout << "Bind failed for http thread" << endl;
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if (listen(file_fd, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}

	while(true)
	{
		fd_set readfds;
		int maxfd, fd;
		int status;

		FD_ZERO(&readfds);
		if(http_fd > file_fd)
			maxfd = http_fd;
		else
			maxfd = file_fd;
		FD_SET(http_fd, &readfds);
		FD_SET(file_fd, &readfds);
		status = select(maxfd + 1, &readfds, NULL, NULL, NULL);    //listens for http request and for file requests from peers

		if(FD_ISSET(http_fd, &readfds))
		{
			if ((new_socket = accept(http_fd, (struct sockaddr *)&address,
							(socklen_t*)&addrlen))<0)
			{
				perror("accept");
				exit(EXIT_FAILURE);
			}
			cout << "connected to client" << endl;
			string ip = inet_ntoa(address.sin_addr);
			int port = ntohs(address.sin_port);
			cout << "client ip and port " << ip << " : " << port << endl;
			bzero(buffer,1024);
			valread = read( new_socket , buffer, 1024);
			if(valread > 0)
			{
				cout << buffer << endl;
				string line = strtok(buffer, "\n");
				string hash = line.substr(5, line.length()-15);
				const char* filename = hash.c_str();

				int check = getfile(filename);     //get requested file

				if(check == 1)                     //if file recieved successfully parse it into http
				{
					string       text;
					stringstream stream;       	
					FILE *sendFile = fopen("recv.html", "r");
					if (sendFile == NULL) 
					{
						cout << "cannot open file" << endl;
						perror("error");
						continue;
					}
					fseek(sendFile, 0L, SEEK_END);
					stream << "HTTP/1.1 200 OK\nContent-length: " << ftell(sendFile) << "\n";    //sending http status and content type
					fseek(sendFile, 0L, SEEK_SET);
					text = stream.str();
					send(new_socket, text.c_str(), text.length(), 0);

					std::cout << "Sent : " <<  text << std::endl;
					if(filetype == "html")
						text = "Content-Type: text/html\n\n";
					else
						text = "Content-Type: image/png\n\n";
					send(new_socket, text.c_str(), text.length(), 0);

					std::cout << "Sent : %s" << text << std::endl;
					while (feof(sendFile) == 0)          // sending through http socket
					{
						int  numread;
						char sendBuffer[500];

						numread = fread(sendBuffer, sizeof(unsigned char), 300, sendFile);
						if (numread > 0)
						{
							char *sendBuffer_ptr;

							sendBuffer_ptr = sendBuffer;
							do {
								fd_set  wfd;
								timeval tm;
								FD_ZERO(&wfd);
								FD_SET(new_socket, &wfd);

								tm.tv_sec  = 10;
								tm.tv_usec = 0;
								if (select(1 + new_socket, NULL, &wfd, NULL, &tm) > 0)
								{
									int numsent;

									numsent = send(new_socket, sendBuffer_ptr, numread, 0);
									if (numsent == -1)
										return 0;
									sendBuffer_ptr += numsent;
									numread        -= numsent;
								}
							} while (numread > 0);
						}
					}
					fclose(sendFile);
				}
				else          //If file was not recieved - send 404 not found status
				{
					string       text;
					stringstream stream;
					FILE *sendFile = fopen("recv1.html", "w");
					if (sendFile == NULL) 
					{
						cout << "cannot open file" << endl;
						perror("error");
						continue;
					}
					fseek(sendFile, 0L, SEEK_END);
					stream << "HTTP/1.1 404 Not Found\nContent-length: " << ftell(sendFile) << "\n";
					fseek(sendFile, 0L, SEEK_SET);
					text = stream.str();
					send(new_socket, text.c_str(), text.length(), 0);

					std::cout << "Sent : " <<  text << std::endl;

					text = "Content-Type: text/html\n\n";
					send(new_socket, text.c_str(), text.length(), 0);

					std::cout << "Sent : %s" << text << std::endl;
					fclose(sendFile);
				}
			}

		}
		if(FD_ISSET(file_fd, &readfds))       // Peer is requesting for file - sends the file to peer
		{
			int send_sock;
			if ((send_sock = accept(file_fd, (struct sockaddr *)&address1,(socklen_t*)&addrlen))<0)
			{
				perror("accept");
				exit(EXIT_FAILURE);
			}
			cout << "connected to client for sending file" << endl;

			char hash[100];
			if(recv(send_sock,hash,sizeof(hash),0) == FAILURE)
			{
				perror("\nNODE SERVER: ERROR RECV\n");
				return NULL;
			}
			string hashname(hash);
			string filename;
			map<string, string> :: iterator it;
			for(it=myhash.begin(); it!=myhash.end(); it++){
				if(it->second == hashname)
					filename = it->first;
				else
					cout << "hashname not found" << endl;
			}
			char filen[100];
			strcpy(filen, filename.c_str());
			int sent_count;
			ssize_t read_bytes, sent_bytes, sent_file_size;
			char send_buf[256];
			char errmsg_notfound[20];
			strcpy(errmsg_notfound,"File not found\n");
			int f;
			sent_count = 0;
			sent_file_size = 0;
			if( (f = open(filen, O_RDONLY)) < 0)
			{
				perror(filen);
				if( (sent_bytes = send(send_sock, errmsg_notfound , strlen(errmsg_notfound), 0)) < 0 )
				{
					perror("send error");
					return NULL;
				}
			}
			else
			{
				cout << "sending file " << filen << endl;
				while( (read_bytes = read(f, send_buf, 256)) > 0 )
				{
					if( (sent_bytes = send(send_sock, send_buf, read_bytes, 0))
							< read_bytes )
					{
						perror("send error");
						return NULL;
					}
					sent_count++;
					sent_file_size += sent_bytes;
				}
				close(f);
			}

			printf("Done with this client. Sent %d bytes in %d send(s)\n\n",
					sent_file_size, sent_count);
			shutdown(send_sock, SHUT_WR);
			close(send_sock);
		}	
	}
	return NULL;
}
