#pragma once
#include <WinSock2.h>
#include "ConstData.h"

class ChatRoom
{
private:
	SOCKET clientSockets[MAXIMUM_ROOM_CLIENT];
	int clientSocketsLastIndex = 0;

public:
	void ConnectRoom(SOCKET clientSocket);

	SOCKET* GetClientSockets();

};

