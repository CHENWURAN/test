#include <iostream>
#include<WinSock2.h>
#include<WS2tcpip.h>
#pragma comment(lib,"ws2_32.lib")
using namespace std;
int main()
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		cerr << "网络初始化失败：" << WSAGetLastError() << endl;
		return -1;
	}
	SOCKADDR_IN sockaddr;
	memset(&sockaddr, 0, sizeof(SOCKADDR));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(16195);
	inet_pton(AF_INET, "115.236.153.172", &sockaddr.sin_addr);
	SOCKET soc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (soc == SOCKET_ERROR)
	{
		cerr << "创建套接字失败：" << WSAGetLastError() << endl;
		return -1;
	}
	if (connect(soc, (SOCKADDR*)&sockaddr, sizeof(sockaddr)) == SOCKET_ERROR)
	{
		cerr << "创建连接失败：" << WSAGetLastError() << endl;
		return -1;
	}
	else
		cout << "已连接上服务器，请开始发数据吧！" << endl;
	char buf[1024] = { 0 };
	while (1)
	{
		cin.getline(buf, 1024);
		if (send(soc, buf, sizeof(buf), 0) == SOCKET_ERROR)
		{
			cerr << "数据发送失败:" << WSAGetLastError() << endl;
			return -1;
		}
		if (recv(soc, buf, sizeof(buf), 0) == SOCKET_ERROR)
		{
			cerr << "数据接收失败:" << WSAGetLastError() << endl;
			return -1;
		}
		cout << buf << endl;
	}
	closesocket(soc);
	WSACleanup();
	return 0;
}

