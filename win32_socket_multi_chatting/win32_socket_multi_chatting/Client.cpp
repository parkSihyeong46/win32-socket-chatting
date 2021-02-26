#include "Client.h"

extern HWND g_hWnd;

Client::Client()
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

Client::~Client()
{
	closesocket(clientSocket);
	WSACleanup();
}

void Client::SendMessageToServer(string chattingMsg)
{
	if(send(clientSocket, chattingMsg.c_str(), chattingMsg.size(), 0) == -1)
		ErrorMsg("send Error ");
}

void Client::SendMessageToServer(GiftData giftData)
{
	if (send(clientSocket, (char*)&giftData, sizeof(giftData), 0) == -1)
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
	
	// switch문으로 구분

	GiftData* recvGiftData = (GiftData*)recvMessage;

	str += to_string(recvGiftData->price) + "가격의 " +
		recvGiftData->name + "을 보냈습니다!" + "\r\n" +
		"(만료기간은 " + to_string(recvGiftData->validity) + " 입니다.)";


	return recvMessage;
}

void Client::ErrorMsg(const string errorMsg)
{
	string errMsg = "Error Message : ";
	MessageBox(g_hWnd, (errMsg + errorMsg).c_str(), "Error Message", MB_OK);
}
