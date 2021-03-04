#include "ChatRoom.h"

void ChatRoom::ConnectRoom(SOCKET clientSocket)
{
	clientSockets[clientSocketsLastIndex++] = clientSocket;
}

void ChatRoom::SendMessageToClient(string msg)
{
	for (int i = 0; i < MAXIMUM_ROOM_CLIENT; i++)
	{
		if (clientSockets[i] == NULL)
			break;
		send(clientSockets[i], msg.c_str(), msg.size(), 0);
	}
}