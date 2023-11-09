#pragma once
#ifndef _INTERNETSOCKET_H
#define _INTERNETSOCKET_H
#include<iostream>
#include<mutex>
#include<WinSock2.h>
#include<thread>
#include<WS2tcpip.h>
#include<list>
#include<algorithm>
#include <condition_variable>
#include<fstream>
#include<iterator>
#include<sstream>
#include<Windows.h>
#include<shared_mutex>
#pragma comment(lib,"ws2_32.lib")
class MyMutex
{
public:
	std::mutex mtx_push;
	std::mutex mtx_save;
	std::shared_timed_mutex smtx;
	std::condition_variable_any cv_signal_add;
	std::condition_variable cv_signal_save;
};
class MyNet
{
public:
	MyNet();
	SOCKADDR_IN ClientSockAddr;
	SOCKET ClientScoket;
};
class MyDefined
{
public:
	void netStart();
private:
	virtual void Main() = 0;
	std::thread th_;
};
class RecvData
{
public:
	void setDataChars(char*);
	std::string getDataChars();
private:
	std::string data_char_;
};
class Server :public MyDefined
{
public:
	void networkInit();
	void networkConnect(int);
	void serverStop();
private:
	void Main()override;
	MyNet netSock;
};
using Shared_Ptr_RecvData = std::shared_ptr<RecvData>;
using list_data = std::list<Shared_Ptr_RecvData>;
extern int clientNum;
#endif