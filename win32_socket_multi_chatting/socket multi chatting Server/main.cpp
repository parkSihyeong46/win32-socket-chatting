#include <iostream>
#include <WinSock2.h>
#include <process.h>
#include <string>

using namespace std;

#pragma comment(lib, "ws2_32")

#define PORT		4578
#define PACKET_SIZE 1024

void SendMsg(string msg, int len);
string RecvMsg();

SOCKET clientSocket;
int strLen = 0;
int main()
{
	WSADATA wsaData;

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
	serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN serverAddress = {};
	serverAddress.sin_family = AF_INET;
	// sin_familly : ���� �ּ� ü��
	serverAddress.sin_port = htons(PORT);
	// sin_port : ��Ʈ
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);	// s_addr : IPv4 �ǹ�, INADDR_ANY : ���� ���۵Ǵ� ��ǻ���� IP�ּ�
	// sin_addr : IP �ּ�

	if (-1 == bind(serverSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)))
	{
		cout << "bind error : " << endl;
		return 1;
	}
	// ���� ���� ���Ͽ� IP�� ��Ʈ�� �Ҵ��Ͽ� ��Ʈ��ũ �������̽��� ���� �� �ֵ��� ��
	listen(serverSocket, SOMAXCONN);	// SOMAXCONN : �Ѳ����� ��û ������ �ִ� ���� ���� ��
	// Ŭ���̾�Ʈ�� ���� ���� ��û�� ��ٸ���.

	SOCKADDR_IN clientAddress = {};
	// Ŭ���̾�Ʈ�� �ּ� ������ ���� SOCKADDR_IN ����
	int clientAddressSize = sizeof(clientAddress);
	clientSocket = accept(serverSocket, (SOCKADDR*)&clientAddress, &clientAddressSize);
	// accept() �� �̿��� Ŭ���̾�Ʈ�� ������ �����Ѵ�.

	//char cBuffer[PACKET_SIZE] = {};
	//recv(clientSocket, cBuffer, PACKET_SIZE, 0);
	//// clientSocket ������ ����
	//cout << "Recv Msg : " << cBuffer << endl;

	//char cMsg[] = "Server Send";
	//send(clientSocket, cMsg, strlen(cMsg), 0);
	//// clientSocket ������ �߽�

	while (true)
	{
		string str = RecvMsg();
		if (strLen != 0)
		{
			if (str == "q" || strLen == -1)
				break;

			SendMsg("Server Send : " + str + "\r\n", sizeof("Server Send : ") + strLen + sizeof("\r\n"));
		}
		else if (-1 == strLen)
			break;
	}

	closesocket(clientSocket);
	closesocket(serverSocket);

	WSACleanup();
	// Winsock2�� ���ȯ�� ����

	return 0;
}

void SendMsg(string msg, int len)
{
	send(clientSocket, msg.c_str(), len, 0);
}

string RecvMsg()
{
	char cBuffer[PACKET_SIZE] = {};
	strLen = 0;
	strLen = recv(clientSocket, cBuffer, PACKET_SIZE, 0);
	// ������ ���� �� -1 return �Ǹ� �ش� clientSocket�� ������ �������ٴ� �Ҹ�, ���� ó�� �������
	cout << strLen << endl;

	if (strLen == -1)
		strLen = -1;

	return cBuffer;
}