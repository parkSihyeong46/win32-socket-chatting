#pragma once
#include <WinSock2.h>
#include "ConstData.h"

class ChatRoom
{
private:
	SOCKET clientSockets[MAXIMUM_ROOM_CLIENT] = {NULL};
	int clientSocketsLastIndex = 0;

public:
	void ConnectRoom(SOCKET clientSocket);

	void SendMessageToClient(string msg);
};

