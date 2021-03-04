﻿#include "Client.h"

extern HWND g_hWnd;

Client::Client()
{
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

void Client::Init()
{
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

void Client::SendConnectRoomMessageToServer(int roomNumber)
{
	packet_t packet = MakePacket('0');
	packet.header.dataSize = sizeof(int);
	memcpy(packet.data, (char*)roomNumber, sizeof(roomNumber));

	if (send(clientSocket, (char*)&packet, PACKET_SIZE, 0) == -1)
		ErrorMsg("send Error ");
}

void Client::SendMessageToServer(string chattingMsg)
{
	packet_t packet = MakePacket('1');
	packet.header.dataSize = chattingMsg.size();
	strcpy(packet.data, chattingMsg.c_str());

	if(send(clientSocket, (char*)&packet, PACKET_SIZE, 0) == -1)
		ErrorMsg("send Error ");
}

void Client::SendGiftDataToServer()
{
	packet_t packet = MakePacket('2');
	packet.header.dataSize = sizeof(giftData_t);
	memcpy(packet.data, (char*)&giftData, sizeof(giftData_t));

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

giftData_t Client::GetGiftData()
{
	return giftData;
}

packet_t Client::MakePacket(const char kind)
{
	packet_t packet;
	packet.header.kind = kind;
	strcpy(packet.userName, name.c_str());

	return packet;
}

void Client::ErrorMsg(const string errorMsg)
{
	string errMsg = "Error Message : ";
	MessageBox(g_hWnd, (errMsg + errorMsg).c_str(), "Error Message", MB_OK);
}
