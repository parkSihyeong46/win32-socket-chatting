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
	char head[MAX_DATA_HEADER_LENGTH] = "0 ";
	char chatMessage[PACKET_SIZE + MAX_NAME_LENGTH];
	strcpy(chatMessage, head);
	strcat(chatMessage, name.c_str());
	strcat(chatMessage, " : ");
	strcat(chatMessage, chattingMsg.c_str());

	if(send(clientSocket, chatMessage, PACKET_SIZE + MAX_NAME_LENGTH, 0) == -1)
		ErrorMsg("send Error ");
}

void Client::SendMessageToServer()
{
	char head[MAX_DATA_HEADER_LENGTH] = "0 ";
	char sendData[PACKET_SIZE + MAX_NAME_LENGTH];
	strcpy(sendData, head);
	strcat(sendData, (char*)&giftData);

	if (send(clientSocket, sendData, PACKET_SIZE + MAX_NAME_LENGTH, 0) == -1)
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
	
	// switch문으로 구분

	/*GiftData* recvGiftData = (GiftData*)recvMessage;

	str += to_string(recvGiftData->price) + "가격의 " +
		recvGiftData->name + "을 보냈습니다!" + "\r\n" +
		"(만료기간은 " + to_string(recvGiftData->validity) + " 입니다.)";*/


	return recvMessage;
}

const string Client::GetName()
{
	return name;
}

GiftDatas Client::GetGiftData()
{
	return giftData;
}

void Client::ErrorMsg(const string errorMsg)
{
	string errMsg = "Error Message : ";
	MessageBox(g_hWnd, (errMsg + errorMsg).c_str(), "Error Message", MB_OK);
}
