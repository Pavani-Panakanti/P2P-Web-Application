# P2P-Web-Application
Developed a p2pweb application for content addressable peer-to-peer hypermedia sharing. A command line interface was developed for the protocol. Any p2pweb node has three logical components Content Provider, Content Tracker and Client Gateway. Content Provider publishes metadata of content into p2pweb and employs mechanisms to avoid loops. Content Tracker keeps track of the content being published into the p2pweb. Client gateway takes HTTP request from a client, and delivers the requested content from p2pweb to that client. Peers were connected using TCP Sockets and SHA-1 is used as message digest algorithm

INTERPLANETARY FILE SYSTEM
  * NODE is a multi-threaded application consisting of teh following threads
  * Server Thread accepts connection from other NODE
  * HTTP thread handles the request from client gateway
  * CLI thread accepts command from the user
  * Client Thread connects to other Nodes.

COMPILATION
  * MAKEFILE is included in the parent directory.
     * make - To Compile
     * make clean - To clean all object files
     * Node.o takes two arguments
     * ./Node.o [port to listen for peers] [port to listen for http] - To run

USAGE
  The following commads can be provided in the NODE command prompt
  * peer <host_name> <port_number> : Connect to a Node
  * show_peers                     : Shows the peer list
  * publish <file>                 : publish file to peers
  * unpublish <file>               : unpublish file
  * show metadata                  : shows the content publised
  * show published                 : shows the content published by publisher

PROJECT TREE

/NODE <br/>
| <br/>
|__/src <br/>
|  | <br/>
|  |__NodeServ.c <br/>
|  |__NodeCli.c <br/>
|  |__NodeClnt.c <br/>
|  |__NodeHttp.c <br/>
|  |__NodeCommon.c <br/>
|  |__NodeApi.c <br/>
| <br/>
<br/>
|__/inc <br/>
|  | <br/>
|  |__NodeCommon.h <br/>
|  |__NodeServ.h <br/>
|  |__NodeCli.h <br/>
|  |__NodeClnt.h <br/>
|  |__NodeHttp.h <br/>
| <br/>
|__/obj <br/>
|   | <br/>
|   |__NodeCommon.o <br/>
|   |__NodeServ.o <br/>
|   |__NodeCli.o <br/>
|   |__NodeClnt.o <br/>
|   |__NodeHttp.o <br/>
|   |__NodeApi.o <br/>
| <br/>
|__NODE.o <br/>
