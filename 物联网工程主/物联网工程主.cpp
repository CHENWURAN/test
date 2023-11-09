#include"mydefined.h"
using namespace std;
int main()
{
	Server s;
	cout << "服务器启动\n";
	s.netStart();
	while (1)Sleep(10000);
	s.serverStop();
	cin.get();
	return 0;
}
