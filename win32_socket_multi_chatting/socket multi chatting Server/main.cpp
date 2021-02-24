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
	// Winsock2를 사용환경 설정
	// 0 리턴하면 정상 종료

	if (err != 0)
	{
		cout << "WSAStartup failed with error : " << err << endl;
		return 1;
	}

	SOCKET serverSocket;
	serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN serverAddress = {};
	serverAddress.sin_family = AF_INET;
	// sin_familly : 소켓 주소 체계
	serverAddress.sin_port = htons(PORT);
	// sin_port : 포트
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);	// s_addr : IPv4 의미, INADDR_ANY : 현재 동작되는 컴퓨터의 IP주소
	// sin_addr : IP 주소

	if (-1 == bind(serverSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)))
	{
		cout << "bind error : " << endl;
		return 1;
	}
	// 서버 측의 소켓에 IP와 포트를 할당하여 네트워크 인터페이스와 묶일 수 있도록 함
	listen(serverSocket, SOMAXCONN);	// SOMAXCONN : 한꺼번에 요청 가능한 최대 접속 승인 수
	// 클라이언트로 부터 연결 요청을 기다린다.

	SOCKADDR_IN clientAddress = {};
	// 클라이언트의 주소 정보를 받을 SOCKADDR_IN 생성
	int clientAddressSize = sizeof(clientAddress);
	clientSocket = accept(serverSocket, (SOCKADDR*)&clientAddress, &clientAddressSize);
	// accept() 를 이용해 클라이언트의 연결을 수락한다.

	//char cBuffer[PACKET_SIZE] = {};
	//recv(clientSocket, cBuffer, PACKET_SIZE, 0);
	//// clientSocket 데이터 수신
	//cout << "Recv Msg : " << cBuffer << endl;

	//char cMsg[] = "Server Send";
	//send(clientSocket, cMsg, strlen(cMsg), 0);
	//// clientSocket 데이터 발신

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
	// Winsock2를 사용환경 종료

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
	// 데이터 수신 시 -1 return 되면 해당 clientSocket과 연결이 끊어졌다는 소리, 따라서 처리 해줘야함
	cout << strLen << endl;

	if (strLen == -1)
		strLen = -1;

	return cBuffer;
}