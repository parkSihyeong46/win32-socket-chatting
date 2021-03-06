#include <iostream>
#include <WinSock2.h>
#include <process.h>
#include <list>
#include "ConstData.h"
#include "ChatRoom.h"

#pragma comment(lib, "ws2_32")

UINT WINAPI EchoThread(void* arg);

HANDLE hMutex;
SOCKET clientSockets[MAXIMUM_CLIENT];

ChatRoom chatRoom[2];
int clientSocketsLastIndex = 0;
int strLen = 0;
int main()
{
	WSADATA wsaData;
	SOCKET clientSocket;

	int err;
	err = WSAStartup(MAKEWORD(2, 2), &wsaData);
	// Winsock2�� ���ȯ�� ����
	// 0 �����ϸ� ���� ����

	if (err != 0)
	{
		cout << "WSAStartup failed with error : " << err << endl;
		return 1;
	}

	SOCKET serverSocket;
	serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);	// ��� �������� ����

	SOCKADDR_IN serverAddress = {};
	serverAddress.sin_family = AF_INET;
	// sin_familly : ���� �ּ� ü��
	serverAddress.sin_port = htons(PORT);
	// sin_port : ��Ʈ
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);	// s_addr : IPv4 �ǹ�, INADDR_ANY : ���� ���۵Ǵ� ��ǻ���� IP�ּ�
	// sin_addr : IP �ּ�

	if (-1 == bind(serverSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress))) // ���� ���� ���Ͽ� IP�� ��Ʈ�� �Ҵ��Ͽ� ��Ʈ��ũ �������̽��� ���� �� �ֵ��� ��
	{
		cout << "bind error : " << endl;
		return 1;
	}
	
	listen(serverSocket, SOMAXCONN);	// SOMAXCONN : �Ѳ����� ��û ������ �ִ� ���� ���� ��
	// Ŭ���̾�Ʈ�� ���� ���� ��û�� ��ٸ���.

	SOCKADDR_IN clientAddress = {};
	// Ŭ���̾�Ʈ�� �ּ� ������ ���� SOCKADDR_IN ����

	hMutex = CreateMutex(NULL, false, NULL);	// ���ؽ� ����, false�� mutex �ٷ� ���� X
	while (true)
	{
		int clientAddressSize = sizeof(clientAddress);
		clientSocket = accept(serverSocket, (SOCKADDR*)&clientAddress, &clientAddressSize);	// accept() �� �̿��� Ŭ���̾�Ʈ�� ������ �����Ѵ�.
		WaitForSingleObject(hMutex, INFINITE);									// ���ؽ� ����
		clientSockets[clientSocketsLastIndex++] = clientSocket;

		ReleaseMutex(hMutex);															// ���ؽ� ����
		_beginthreadex(NULL, 0, EchoThread, &clientSocket, 0, NULL);	// echo thread ����
		cout << "Connect Ip : " << inet_ntoa(clientAddress.sin_addr) << endl;
	}

	closesocket(serverSocket);	
	// socket() �̿� ������ close ó��

	WSACleanup();
	// Winsock2�� ���ȯ�� ����

	return 0;
}

UINT WINAPI EchoThread(void* arg)
{
	SOCKET clientSocket = *(SOCKET*)arg;
	char cBuffer[PACKET_SIZE] = {};
	giftData_t* giftData = nullptr;
	packet_t* packet = nullptr;
	char* tempChar = nullptr;
	string userName = "null";
	int roomNumber = -1;

	while (recv(clientSocket, cBuffer, PACKET_SIZE, 0) != -1 )
	{	
		WaitForSingleObject(hMutex, INFINITE);

		packet = (packet_t*)cBuffer;

		tempChar = new char[packet->header.dataSize];

		switch (packet->header.kind)
		{
		case SET_NAME:
			strcpy(tempChar, packet->data);
			userName = tempChar;
			break;
		case CONNECT_ROOM:
			memcpy(tempChar, packet->data, packet->header.dataSize);
			roomNumber = *((int*)tempChar);
			chatRoom[*((int*)tempChar)].ConnectRoom(clientSocket);

			cout << userName << " ���� " << roomNumber+1 << "�� �濡 ���� �߽��ϴ�." << endl;
			cout << roomNumber << "�� �� ���� �ο� : " << chatRoom[roomNumber].GetClientSocketsLastIndex() << endl << endl;
			break;
		case COMMON:
			if (roomNumber < 0)
				break;

			cout << packet->data << endl;
			chatRoom[roomNumber].SendMessageToClient(userName + " : " + packet->data);
			break;
		case GIFT:
			if (roomNumber < 0)
				break;

			memcpy(tempChar, packet->data, packet->header.dataSize);
			giftData = (giftData_t*)tempChar;
			
			chatRoom[roomNumber].SendMessageToClient(userName + " ���� " + to_string(giftData->price) + " ������ " +
				giftData->productName + "�� ���½��ϴ�! (����Ⱓ : " + to_string(giftData->validity) +
				" �Դϴ�.)");
			break;
		default:
			cout << packet->header.kind << " ���ſ��� Ȯ�ιٶ�" << endl;
			continue;
		}

		delete[] tempChar;

		memset(cBuffer, NULL, PACKET_SIZE);
		ReleaseMutex(hMutex);
	}

	WaitForSingleObject(hMutex, INFINITE);
	// recv -1 �̴� ������ ���� ���� ����
	// client ���� ó��

	if (roomNumber != -1)
	{
		chatRoom[roomNumber].ExitRoom(clientSocket);
		cout << roomNumber << "�� �� ���� �ο� : " << chatRoom[roomNumber].GetClientSocketsLastIndex() << endl;
	}

	cout << userName << " ���� " << roomNumber << "�� ���� �������ϴ�." << endl << endl;
	for (int i = 0; i < clientSocketsLastIndex; i++)
	{
		if (clientSockets[i] == clientSocket)
		{
			while (i++ < clientSocketsLastIndex - 1)
			{
				clientSockets[i] = clientSockets[i + 1];
			}
			// ��ĭ�� ��ܿ� ������
		}
	}

	clientSocketsLastIndex--;
	ReleaseMutex(hMutex);
	closesocket(clientSocket);
	return 0;
}
