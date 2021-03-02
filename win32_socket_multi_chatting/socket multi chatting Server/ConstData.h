#pragma once
#include <string>
using namespace std;

constexpr const int PORT =  4578;
constexpr const int PACKET_SIZE = 1024;
constexpr const int MAX_NAME_LENGTH = 100;
constexpr const int MAXIMUM_CLIENT = 32;
constexpr const int MAXIMUM_ROOM_CLIENT = 8;

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
	int price;		// ����
	string name;	// ��ǰ��
	float validity;	// ��ȿ�Ⱓ
} giftData_t;

enum SendMessageKind
{
	COMMON,
	GIFT,
};