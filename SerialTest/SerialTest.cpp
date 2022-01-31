// SerialTest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。

#include "MySerial.h"
#include <iostream>
using namespace std;
int main()
{
	SerialPort mySerialPort;
	char PortName[] = "COM3";   //串口名称
	DWORD ErrorCode = 0;
	LPDWORD pErrorCode = &ErrorCode;

	if (!mySerialPort.OpenPort(PortName,pErrorCode))
	{
		cout << "Open port fail ! Error code " << ErrorCode<<endl;
	}
	else
	{
		cout << "Open port success !" << std::endl;
	}

	char* temp = new char[8];
	int length = 8;

	temp[0] = 03;
	temp[1] = 85;
	temp[2] = 04;
	temp[3] = 00;
	temp[4] = 00;
	temp[5] = 00;
	temp[6] = 11;
	temp[7] = 03;

	cout << "Start sending data" << endl;
	if (!mySerialPort.WriteData(temp, length, pErrorCode))
	{
		cout << "Send fail ! Error code " << ErrorCode << endl;
	}
	else
	{
		cout << "Send success !" << endl;
	}
	delete[] temp;

	char data;
	for (int i = 0; i < 10; i++)
	{
		if (!mySerialPort.ReadData(&data, 1, pErrorCode))
		{
			cout << "Receive fail ! Error code " << ErrorCode << endl;
		}
		else
		{
			cout << "Receive success !   Byte received: " << data << endl;
		}
		Sleep(2000);
	}

	if (!mySerialPort.ClosePort(pErrorCode))
	{
		cout << "Close port fail ! Error code " << ErrorCode << endl;
	}
	else
	{
		cout << "Close port success !" << std::endl;
	}
	system("pause");
	return 0;
}
// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
