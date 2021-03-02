#include "ChatRoom.h"

void ChatRoom::ConnectRoom(SOCKET clientSocket)
{
	clientSockets[clientSocketsLastIndex++] = clientSocket;
}

SOCKET* ChatRoom::GetClientSockets()
{
	return clientSockets;
}
