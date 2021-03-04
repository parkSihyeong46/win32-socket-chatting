#pragma once
#include <WinSock2.h>
#include <string>
#include <list>
#include "Resource.h"

#pragma comment(lib, "ws2_32")
using namespace std;

constexpr const int SERVER_PORT = 4578;
constexpr const char* SERVER_IP = "192.168.123.101";

typedef struct
{
	char kind;
	int dataSize;
} header_t;

typedef struct
{
	header_t header;
	char userName[USER_NAME_SIZE];
	char data[PACKET_SIZE - sizeof(header_t) - USER_NAME_SIZE];
} packet_t;

typedef struct
{
	int price;		// 가격
	char productName[PRODUCT_NAME_SIZE];	// 상품명
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

