#pragma once
#include <WinSock2.h>
#include <string>

#pragma comment(lib, "ws2_32")
using namespace std;

constexpr const int SERVER_PORT = 4567;
constexpr const int PACKET_SIZE = 1024;

class Server
{
private:
	int wsaResult = 0;
	int bindResult = 0;
	int listenResult = 0;

	SOCKET serverSocket;
	SOCKET clientSocket;
public:
	Server();
	~Server();

	void Init();
	void Run();



	void ErrorMsg(const string str);
};

