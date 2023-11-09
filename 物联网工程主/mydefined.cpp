#include "mydefined.h"
int clientNum = 1;
list_data lista;
MyMutex thread_control;
SOCKADDR_IN socaddr, MainSockAddr;
SOCKET soc;
MyNet::MyNet()
{
	memset(&ClientSockAddr, 0, sizeof(SOCKADDR));
}
void RecvData::setDataChars(char* data)
{
	this->data_char_ = data;
}
std::string RecvData::getDataChars()
{
	return this->data_char_.c_str();
}
void MyDefined::netStart()
{
	th_ = std::thread(&MyDefined::Main, this);
	th_.detach();
}
void pushData(std::string data)
{
	std::unique_lock<std::mutex>lck(thread_control.mtx_push);
	RecvData* red = new RecvData;
	red->setDataChars(const_cast<char*>(data.c_str()));
	Shared_Ptr_RecvData spr(red);
	lista.emplace_back(spr);
	if (lista.size() > 2)
	{
		lck.unlock();
		thread_control.cv_signal_save.notify_one();
	}
}
void saveData()
{
	std::unique_lock<std::mutex>lck(thread_control.mtx_save);
	thread_control.cv_signal_save.wait(lck);
	std::fstream fs("C:/Users/chenwuran/Desktop/save_data.txt", std::ios::out | std::ios::app);
	if (!fs)
	{
		std::cout << "\n文件打开失败！\n";
		exit(0);
	}
	else
		std::cout << "\n文件写入进程启动：文件保存位置C:/Users/chenwuran/Desktop/save_data.txt\n";
	while (lista.size() != 0)
	{
		if (lista.front()->getDataChars() != "")
			fs << lista.front()->getDataChars() << '\n';
		lista.pop_front();
	}
	std::cout << "文件保存完毕！\n\n";
	fs.close();
	std::thread(saveData).detach();
}
void Server::networkInit()
{
	try {
		WSADATA wsa;
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		{
			std::cerr << "服务器网络初始化失败：" << WSAGetLastError() << std::endl;
			throw 1;
		}
		if (std::move(soc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == SOCKET_ERROR)
		{
			std::cerr << "套接字初始化失败：" << WSAGetLastError() << std::endl;
			throw 2;
		}
		memset(&MainSockAddr, 0, sizeof(SOCKADDR_IN));
		MainSockAddr.sin_family = AF_INET;
		MainSockAddr.sin_port = htons(8888);
		MainSockAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
		if (bind(soc, (SOCKADDR*)&MainSockAddr, sizeof(SOCKADDR)) == SOCKET_ERROR)
		{
			std::cerr << "服务器数据绑定失败：" << WSAGetLastError() << std::endl;
			throw 3;
		}
		if (listen(soc, 20) == SOCKET_ERROR)
		{
			std::cerr << "监听服务创建失败：" << WSAGetLastError() << std::endl;
			throw 4;
		}
	}
	catch (int)
	{
		std::cout << "程序已退出！\n";
		exit(0);
	}
}
void Server::networkConnect(int i)
{
	try {
		int sockAddr_len = sizeof(SOCKADDR);
		memset(&this->netSock.ClientSockAddr, 0, sizeof(SOCKADDR));
		this->netSock.ClientSockAddr.sin_port = htons(8889);
		if ((this->netSock.ClientScoket = accept(soc, (SOCKADDR*)&this->netSock.ClientSockAddr, &sockAddr_len)) == SOCKET_ERROR)
		{
			std::cerr << "连接失败：" << WSAGetLastError() << std::endl;
			throw '1';
		}
		else
			std::cout << i << "号客户已连接\n";
		//schar sip[32];
		//inet_ntop(AF_INET, &this->netSock.ClientSockAddr.sin_addr, sip, sizeof(sip));
		//std::cout << "它的ip：" << sip << std::endl;
		Server* server = new Server;
		std::shared_ptr<Server> p(server);
		p->netStart();
		char recvbuf[1024] = { 0 };
		while (1)
		{
			if (recv(this->netSock.ClientScoket, recvbuf, sizeof(recvbuf), 0) == SOCKET_ERROR)
			{
				std::cerr << i << "号用户已断开连接：" << WSAGetLastError() << std::endl;
				break;
				//throw '2'
			}
			std::cout << "已收到" << i << "号客户端发来的数据：" << recvbuf << std::endl;
			char buf[1024] = { 0 };
			strcat_s(buf, strlen(buf) + 21, "已收到您发来的数据：");
			strcat_s(buf, strlen(buf) + strlen(recvbuf) + 1, recvbuf);
			strcat_s(buf, strlen(buf) + 37, "\n亲爱的客户，请再多多给我发数据吧！\n");
			std::thread(pushData, recvbuf).detach();
			if (send(this->netSock.ClientScoket, buf, sizeof(buf), 0) == SOCKET_ERROR)
			{
				std::cerr << i << "号用户已断开连接!" << std::endl;
				break;
				//throw '3';
			}
		}
		closesocket(this->netSock.ClientScoket);
	}
	catch (char)
	{
		std::cout << "程序已退出！\n";
		exit(0);
	}
}
void Server::Main()
{
	thread_control.smtx.lock_shared();
	if (clientNum == 1)
	{
		this->networkInit();
		std::thread(saveData).detach();
	}
	if (clientNum % 4 == 0)
		std::thread(saveData).detach();
	this->networkConnect(clientNum++);
}
void Server::serverStop()
{
	thread_control.smtx.lock_shared();
	thread_control.smtx.unlock_shared();
	Sleep(10000);
	thread_control.smtx.lock();
	std::cout << "\n服务器关闭，程序结束！\n";
	WSACleanup();
	exit(0);
}
