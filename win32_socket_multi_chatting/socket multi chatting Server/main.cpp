#include <iostream>
#include <WinSock2.h>
#include <process.h>
#include <string>

using namespace std;

#pragma comment(lib, "ws2_32")

#define PORT		4578
#define PACKET_SIZE 1024
#define MAXIMUM_CLIENT 8

UINT WINAPI EchoThread(void* arg);

HANDLE hMutex;
HANDLE EchoThreadHandle;
SOCKET clientSockets[MAXIMUM_CLIENT];
int clientSocketsLastIndex = 0;
int connectClientNumber = 0;
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
		EchoThreadHandle = (HANDLE)_beginthreadex(NULL, 0, EchoThread, &clientSocket, 0, NULL);	// echo thread ����
		cout << "Connect Ip : " << clientAddress.sin_addr.s_addr << endl;
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

	while (recv(clientSocket, cBuffer, PACKET_SIZE, 0) != -1)
	{
		WaitForSingleObject(hMutex, INFINITE);

		for (int i = 0; i < connectClientNumber; i++)
		{
			send(clientSockets[i], cBuffer, sizeof(cBuffer), 0);	// recv ���� Ŭ���̾�Ʈ ��ü���� send
		}

		ReleaseMutex(hMutex);
	}

	WaitForSingleObject(hMutex, INFINITE);
	// recv -1 �̴� ������ ���� ���� ����
	// client ���� ó��
	for (int i = 0; i < connectClientNumber; i++)
	{
		if (clientSockets[i] == clientSocket)
		{
			while (i++ < connectClientNumber - 1)
			{
				clientSockets[i] = clientSockets[i + 1];
			}
			// ��ĭ�� ��ܿ� ������
		}
	}


	connectClientNumber--;
	closesocket(clientSocket);
	ReleaseMutex(hMutex);
	return 0;
}