#include <iostream>
#include <WinSock2.h>
#include <process.h>
#include <list>
#include "ConstData.h"
#include "ChatRoom.h"

#pragma comment(lib, "ws2_32")

UINT WINAPI EchoThread(void* arg);
void SendClientMessage(string msg);
void SendServerMessage(string msg);

HANDLE hMutex;
SOCKET clientSockets[MAXIMUM_CLIENT];

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
	char cBuffer[PACKET_SIZE + MAX_NAME_LENGTH] = {};
	giftData_t* giftData = nullptr;
	packet_t* packet = nullptr;
	char* tempChar = nullptr;

	while ((recv(clientSocket, cBuffer, PACKET_SIZE, 0)) != -1 )
	{	
		WaitForSingleObject(hMutex, INFINITE);

		packet = (packet_t*)cBuffer;
		cout << packet->data << endl;

		switch ((int)packet->header.kind - 48)
		{
		case COMMON:
			SendClientMessage(packet->header.name + " : " + packet->data);
			break;
		case GIFT:
			tempChar = new char[packet->header.dataSize];

			strncpy(tempChar, packet->data, packet->header.dataSize);
			giftData = (giftData_t*)tempChar;

			SendServerMessage(packet->header.name + " ���� " + to_string(giftData->price) + " ������ " +
				giftData->name + "�� ���½��ϴ�! (����Ⱓ : " + to_string(giftData->validity) +
				" �Դϴ�.)");

			delete[] tempChar;
			break;
		default:
			cout << packet->header.kind << " ���ſ��� Ȯ�ιٶ�" << endl;
			continue;
		}

		memset(cBuffer, NULL, PACKET_SIZE);
		ReleaseMutex(hMutex);
	}

	WaitForSingleObject(hMutex, INFINITE);
	// recv -1 �̴� ������ ���� ���� ����
	// client ���� ó��
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
void SendClientMessage(string msg)
{
	for (int i = 0; i < clientSocketsLastIndex; i++)
	{
		send(clientSockets[i], msg.c_str(), msg.size(), 0);
	}
}

void SendServerMessage(string msg)
{
	for (int i = 0; i < clientSocketsLastIndex; i++)
	{
		send(clientSockets[i], msg.c_str(), msg.size(), 0);
	}
}
