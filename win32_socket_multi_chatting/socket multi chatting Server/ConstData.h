#pragma once
#include <string>
using namespace std;

constexpr const int PORT =  4578;
constexpr const int PACKET_SIZE = 1024;
constexpr const int MAXIMUM_CLIENT = 32;
constexpr const int MAXIMUM_ROOM_CLIENT = 8;
constexpr const int USER_NAME_SIZE = 30;
constexpr const int PRODUCT_NAME_SIZE = 30;

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
	int price;		// ����
	char productName[PRODUCT_NAME_SIZE];	// ��ǰ��
	float validity;	// ��ȿ�Ⱓ
} giftData_t;

enum SendMessageKind
{
	SET_NAME,
	CONNECT_ROOM,
	COMMON,
	GIFT,
};