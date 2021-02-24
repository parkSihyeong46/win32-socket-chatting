#pragma once
#include <WinSock2.h>
#include <string>

#pragma comment(lib, "ws2_32")
using namespace std;

constexpr const int SERVER_PORT = 4578;
constexpr const char* SERVER_IP = "192.168.123.101";
constexpr const int PACKET_SIZE = 1024;

class Client
{
private:
	bool isConnect = false;

	SOCKET clientSocket;

public:
	Client();
	~Client();

	void Run();
	void SendMessageToServer(string chattingMsg);
	string RecvMessageFromServer();

	void ErrorMsg(const string errorMsg);

	const bool IsConnect();
};

