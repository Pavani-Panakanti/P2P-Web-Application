#include "NodeCommon.h"

void nodeConPeer(int *, struct node_mssg *);
void nodePublishHash(int *, struct node_mssg *);
void nodeunPublishHash(int *, struct node_mssg *);
void nodeSendTerm(int *, struct node_mssg *);

//To handle commands
int cmd_id[MAX_COMMANDS]=
{
	CONNECT_PEER,
	PUBLISH_HASH,
	UNPUBLISH_HASH,
};

void (*cmd_handler[MAX_COMMANDS])(int *,struct node_mssg *)=
{
	&nodeConPeer,
	&nodePublishHash,
	&nodeunPublishHash,
};
