#include "Client.h"

extern HWND g_hWnd;

Client::Client(string name)
{
	this->name = name;

	// test data
	giftData.price = 100;
	giftData.name = "초콜릿";
	giftData.validity = 10.5f;

	WSADATA wsaData;
	int wsaResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (wsaResult != 0)
	{
		ErrorMsg("WSAStartup " + to_string(wsaResult));
		return;
	}

	clientSocket = socket(PF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN serverAddress = {};
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(SERVER_PORT);
	serverAddress.sin_addr.s_addr = inet_addr(SERVER_IP);

	int connectResult = connect(clientSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress));
	if (connectResult != 0)
	{
		ErrorMsg("connect " + to_string(connectResult));
		return;
	}
}

Client::~Client()
{
	closesocket(clientSocket);
	WSACleanup();
}

void Client::SendMessageToServer(string chattingMsg)
{
	packet_t packet;
	packet.header.kind = '0';
	packet.header.dataSize = chattingMsg.size();
	strcpy(packet.data, chattingMsg.c_str());

	if(send(clientSocket, (char*)&packet, PACKET_SIZE, 0) == -1)
		ErrorMsg("send Error ");
}

void Client::SendMessageToServer()
{
	packet_t packet;
	packet.header.kind = '1';
	packet.header.dataSize = sizeof(giftData_t);
	strcpy(packet.data, (char*)&giftData);

	if (send(clientSocket, (char*)&packet, PACKET_SIZE, 0) == -1)
		ErrorMsg("send Error ");
}

string Client::RecvMessageFromServer()
{
	string str;
	char recvMessage[PACKET_SIZE + MAX_NAME_LENGTH] = {};

	if (recv(clientSocket, recvMessage, PACKET_SIZE + MAX_NAME_LENGTH, 0) == -1)
	{
		ErrorMsg("recv Error ");
		closesocket(clientSocket);
		WSACleanup();
		return "";
	}

	return recvMessage;
}

const string Client::GetName()
{
	return name;
}

giftData_t Client::GetGiftData()
{
	return giftData;
}

void Client::ErrorMsg(const string errorMsg)
{
	string errMsg = "Error Message : ";
	MessageBox(g_hWnd, (errMsg + errorMsg).c_str(), "Error Message", MB_OK);
}
