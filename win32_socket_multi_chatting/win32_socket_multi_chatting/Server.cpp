#include "Server.h"

//extern HWND g_hWnd;

Server::Server()
{
	Init();
}

Server::~Server()
{
	closesocket(serverSocket);
	closesocket(clientSocket);

	WSACleanup();
}

void Server::Init()
{
	WSADATA wsaData;
	wsaResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (wsaResult != 0)
	{
		ErrorMsg(("WSAStartup " + to_string(wsaResult)));
	}

	serverSocket = socket(PF_INET, SOCK_STREAM, 0);
	SOCKADDR_IN serverAddress = {};
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(SERVER_PORT);
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);

	bindResult = bind(serverSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress));
	if (bindResult != 0)
	{
		ErrorMsg(("bind " + to_string(bindResult)));
	}

	listenResult = listen(serverSocket, SOMAXCONN);
}

void Server::Run()
{
	/*SOCKADDR_IN clientAddress = {};
	int clientAddressSize = sizeof(clientAddress);
	clientSocket = accept(serverSocket, (SOCKADDR*)&clientAddress, &clientAddressSize);

	char recvBuffer[PACKET_SIZE] = {};
	recv(clientSocket, recvBuffer, PACKET_SIZE, 0);

	send(clientSocket, recvBuffer, PACKET_SIZE, 0);*/
}

void Server::ErrorMsg(const string str)
{
	/*string errMsg = "Error Message : ";
	MessageBox(g_hWnd, (errMsg + str).c_str(), "Error Message", MB_OK);
	exit(0)*/;
}
