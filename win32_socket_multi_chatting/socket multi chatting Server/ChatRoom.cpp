#include "ChatRoom.h"

void ChatRoom::ConnectRoom(SOCKET clientSocket)
{
	clientSockets[clientSocketsLastIndex++] = clientSocket;
}

void ChatRoom::ExitRoom(SOCKET clientSocket)
{
	int clientIndex = -1;

	for (int i = 0; i < clientSocketsLastIndex; i++)
	{
		if (clientSockets[i] == clientSocket)
			clientIndex = i;
	}

	if (-1 == clientIndex)
		return;

	clientSocketsLastIndex--;

	for (int i = clientIndex; i < clientSocketsLastIndex; i++)
	{
		clientSockets[clientIndex] = clientSockets[clientIndex + 1];
	}
}

const int ChatRoom::GetClientSocketsLastIndex()
{
	return clientSocketsLastIndex;
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