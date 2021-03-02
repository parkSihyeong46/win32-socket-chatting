#pragma once
#include <WinSock2.h>
#include <string>
#include <list>
#include "Resource.h"

#pragma comment(lib, "ws2_32")
using namespace std;

constexpr const int SERVER_PORT = 4578;
constexpr const char* SERVER_IP = "203.232.193.171";

typedef struct
{
	char kind;
	string name;
	int dataSize;
} header_t;

typedef struct
{
	header_t header;
	char data[PACKET_SIZE - sizeof(header_t)];
} packet_t;

typedef struct
{
	int price;		// 가격
	string name;	// 상품명
	float validity;	// 유효기간
} giftData_t;

enum SendMessageKind
{
	COMMON,
	GIFT,
};

class Client
{
private:
	string name;
	giftData_t giftData;

	SOCKET clientSocket;
public:
	Client(string name);
	~Client();

	void SendMessageToServer(string chattingMsg);
	void SendMessageToServer();
	string RecvMessageFromServer();

	const string GetName();
	giftData_t GetGiftData();

	void ErrorMsg(const string errorMsg);
};

