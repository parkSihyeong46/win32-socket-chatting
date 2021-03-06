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
	// Winsock2를 사용환경 설정
	// 0 리턴하면 정상 종료

	if (err != 0)
	{
		cout << "WSAStartup failed with error : " << err << endl;
		return 1;
	}

	SOCKET serverSocket;
	serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);	// 통신 프로토콜 설정

	SOCKADDR_IN serverAddress = {};
	serverAddress.sin_family = AF_INET;
	// sin_familly : 소켓 주소 체계
	serverAddress.sin_port = htons(PORT);
	// sin_port : 포트
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);	// s_addr : IPv4 의미, INADDR_ANY : 현재 동작되는 컴퓨터의 IP주소
	// sin_addr : IP 주소

	if (-1 == bind(serverSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress))) // 서버 측의 소켓에 IP와 포트를 할당하여 네트워크 인터페이스와 묶일 수 있도록 함
	{
		cout << "bind error : " << endl;
		return 1;
	}
	
	listen(serverSocket, SOMAXCONN);	// SOMAXCONN : 한꺼번에 요청 가능한 최대 접속 승인 수
	// 클라이언트로 부터 연결 요청을 기다린다.

	SOCKADDR_IN clientAddress = {};
	// 클라이언트의 주소 정보를 받을 SOCKADDR_IN 생성

	hMutex = CreateMutex(NULL, false, NULL);	// 뮤텍스 생성, false라서 mutex 바로 실행 X
	while (true)
	{
		int clientAddressSize = sizeof(clientAddress);
		clientSocket = accept(serverSocket, (SOCKADDR*)&clientAddress, &clientAddressSize);	// accept() 를 이용해 클라이언트의 연결을 수락한다.
		WaitForSingleObject(hMutex, INFINITE);									// 뮤텍스 실행
		clientSockets[clientSocketsLastIndex++] = clientSocket;

		ReleaseMutex(hMutex);															// 뮤텍스 종료
		_beginthreadex(NULL, 0, EchoThread, &clientSocket, 0, NULL);	// echo thread 실행
		cout << "Connect Ip : " << inet_ntoa(clientAddress.sin_addr) << endl;
	}

	closesocket(serverSocket);	
	// socket() 이용 했으니 close 처리

	WSACleanup();
	// Winsock2를 사용환경 종료

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

			cout << userName << " 님이 " << roomNumber+1 << "번 방에 접속 했습니다." << endl;
			cout << roomNumber << "번 방 접속 인원 : " << chatRoom[roomNumber].GetClientSocketsLastIndex() << endl << endl;
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
			
			chatRoom[roomNumber].SendMessageToClient(userName + " 님이 " + to_string(giftData->price) + " 가격의 " +
				giftData->productName + "을 보냈습니다! (만료기간 : " + to_string(giftData->validity) +
				" 입니다.)");
			break;
		default:
			cout << packet->header.kind << " 수신오류 확인바람" << endl;
			continue;
		}

		delete[] tempChar;

		memset(cBuffer, NULL, PACKET_SIZE);
		ReleaseMutex(hMutex);
	}

	WaitForSingleObject(hMutex, INFINITE);
	// recv -1 이니 서버와 연결 종료 상태
	// client 제거 처리

	if (roomNumber != -1)
	{
		chatRoom[roomNumber].ExitRoom(clientSocket);
		cout << roomNumber << "번 방 접속 인원 : " << chatRoom[roomNumber].GetClientSocketsLastIndex() << endl;
	}

	cout << userName << " 님이 " << roomNumber << "번 방을 나갔습니다." << endl << endl;
	for (int i = 0; i < clientSocketsLastIndex; i++)
	{
		if (clientSockets[i] == clientSocket)
		{
			while (i++ < clientSocketsLastIndex - 1)
			{
				clientSockets[i] = clientSockets[i + 1];
			}
			// 한칸씩 당겨와 재정렬
		}
	}

	clientSocketsLastIndex--;
	ReleaseMutex(hMutex);
	closesocket(clientSocket);
	return 0;
}
