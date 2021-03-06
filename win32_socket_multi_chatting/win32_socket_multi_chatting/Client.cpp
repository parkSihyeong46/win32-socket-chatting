#include "Client.h"

extern HWND g_hWnd;

Client::Client()
{
	name = "";
	roomNumber = -1;

	// test data
	giftData.price = 100;
	strcpy(giftData.productName, "초콜릿");
	giftData.validity = 10.5f;

	Init();
}

Client::~Client()
{
	closesocket(clientSocket);
	WSACleanup();
}

int Client::Init()
{
	WSADATA wsaData;
	int wsaResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (wsaResult != 0)
	{
		ErrorMsg("WSAStartup " + to_string(wsaResult));
		return -1;
	}

	clientSocket = socket(PF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN serverAddress = {};
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(SERVER_PORT);
	serverAddress.sin_addr.s_addr = inet_addr(SERVER_IP);

	connectResult = connect(clientSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress));
	if (connectResult != 0)
	{
		ErrorMsg("connect " + to_string(connectResult));
		return -2;
	}

	return 0;
}

void Client::SendDataToServer(int index, string chattingMsg)
{
	packet_t packet;
	packet = MakePacket(index);

	switch (index)
	{
	case SET_NAME:
		packet.header.dataSize = sizeof(name.size());
		memcpy(packet.data, name.c_str(), sizeof(name.size()));
		break;
	case CONNECT_ROOM:
		packet.header.dataSize = sizeof(int);
		memcpy(packet.data, (char*)&roomNumber, sizeof(int));
		break;
	case COMMON:
		packet.header.dataSize = chattingMsg.size();
		strcpy(packet.data, chattingMsg.c_str());
		break;
	case GIFT:
		packet.header.dataSize = sizeof(giftData_t);
		memcpy(packet.data, (char*)&giftData, sizeof(giftData_t));
		break;
	default:
		return;
	}

	if (send(clientSocket, (char*)&packet, PACKET_SIZE, 0) == -1)
		ErrorMsg("send Error ");
}

string Client::RecvMessageFromServer()
{
	string str;
	char recvMessage[PACKET_SIZE] = {};

	if (recv(clientSocket, recvMessage, PACKET_SIZE, 0) == -1)
	{
		ErrorMsg("recv Error ");
		closesocket(clientSocket);
		WSACleanup();
		return "";
	}

	return recvMessage;
}

void Client::SetName(const string name)
{
	this->name = name;
}

const string Client::GetName()
{
	return name;
}

void Client::SetRoomNumber(const int roomNumber)
{
	this->roomNumber = roomNumber;
}

giftData_t Client::GetGiftData()
{
	return giftData;
}

const int Client::GetConnectResult()
{
	return connectResult;
}

packet_t Client::MakePacket(const int kind)
{
	packet_t packet;
	packet.header.kind = kind;

	return packet;
}

void Client::ErrorMsg(const string errorMsg)
{
	string errMsg = "Error Message : ";
	MessageBox(g_hWnd, (errMsg + errorMsg).c_str(), "Error Message", MB_OK);
}
