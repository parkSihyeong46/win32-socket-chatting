#pragma once
#include <WinSock2.h>
#include <string>
#include <list>
#include "Resource.h"

#pragma comment(lib, "ws2_32")
using namespace std;

constexpr const int SERVER_PORT = 4578;
constexpr const char* SERVER_IP = "192.168.123.101";

typedef struct GiftDatas
{
	int price;		// 가격
	string name;	// 상품명
	float validity;	// 유효기간
} GiftData;

enum SendMessageKind
{
	COMMON,
	GIFT,
};

class Client
{
private:
	string name;
	GiftDatas giftData;

	SOCKET clientSocket;
public:
	Client(string name);
	~Client();

	void SendMessageToServer(string chattingMsg);
	void SendMessageToServer();
	string RecvMessageFromServer();

	const string GetName();
	GiftDatas GetGiftData();

	void ErrorMsg(const string errorMsg);
};

