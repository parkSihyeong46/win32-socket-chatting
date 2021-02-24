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

	isConnect = true;
}

Client::~Client()
{
	closesocket(clientSocket);
	WSACleanup();
}

void Client::Run()
{
	// recv thread
}

void Client::SendMessageToServer(string chattingMsg)
{
	if (!isConnect)
		return;

	int sendResult = 0;
	sendResult = send(clientSocket, chattingMsg.c_str(), chattingMsg.size(), 0);
}

string Client::RecvMessageFromServer()
{
	if (!isConnect)
	{
		return "Server Connect Lost\r\n";
	}

	char recvMessage[PACKET_SIZE] = {};
	int strLen = 0;
	strLen = recv(clientSocket, recvMessage, PACKET_SIZE,0);

	if (strLen == -1)
	{
		isConnect = false;

		ErrorMsg("recv Error ");
		closesocket(clientSocket);
		WSACleanup();
		return "";
	}

	return recvMessage;
}

void Client::ErrorMsg(const string errorMsg)
{
	string errMsg = "Error Message : ";
	MessageBox(g_hWnd, (errMsg + errorMsg).c_str(), "Error Message", MB_OK);
}

const bool Client::IsConnect()
{
	return isConnect;
}
