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
	int kind;
	int dataSize;
} header_t;

typedef struct
{
	header_t header;
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
	SET_NAME,
	CONNECT_ROOM,
	COMMON,
	GIFT,
};

class Client
{
private:
	string name;
	int roomNumber;
	giftData_t giftData;

	SOCKET clientSocket;
	int connectResult;
public:
	Client();
	~Client();

	int Init();

	void SendDataToServer(int index, string chattingMsg = "");
	string RecvMessageFromServer();

	void SetName(const string name);
	void SetRoomNumber(const int roomNumber);
	const string GetName();
	giftData_t GetGiftData();

	const int GetConnectResult();

	packet_t MakePacket(const int kind);
	void ErrorMsg(const string errorMsg);
};

