#include "Channel.h"
#include <iostream>
using namespace std;
int main()
{
	SerialParameter testParameter;
	testParameter.baudRate = 9600;
	testParameter.dataBits = 8;
	testParameter.parity = 'N';
	testParameter.portName = "COM3";
	testParameter.stopBits = 1;
	SerialChannel testChannel(&testParameter, 10);
	if (testChannel.InitCommunication())
	{
		cout << "Initialization Success" << endl;
	}
	else
	{
		cout << "Initialization Fail " << endl << "Error Code: " << testChannel.GetErrorCode() << endl;
	}

	if (testChannel.StartCommunication())
	{
		cout << "Communication Start" << endl;
	}
	else
	{
		cout << "Fail to Start Communication " << endl << "Error Code: " << testChannel.GetErrorCode() << endl << endl;
	}

	
	for (int i = 0; i < 10; i++)
	{
		cout << "Display No. " << i+1 << endl;
		testChannel.Display();
		cout << endl;
		Sleep(5000);
	}


	if (testChannel.TerminateCommunication())
	{
		cout << "Terminate Success" << endl;
	}
	else
	{
		cout << endl << "Terminate Fail " << endl << "Error Code: " << testChannel.GetErrorCode() << endl;
	}
}
/* 串口测试程序，OpenPort函数的参数列表已改变，请自行修改
	SerialPort mySerialPort;
	char PortName[] = "COM3";   //串口名称
	DWORD ErrorCode = -1;
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
*/

/*  链表队列测试程序
	bool status;

	LinkQueue* testQ = new LinkQueue(3);
	cout << "Init  " << "   Current length: " << testQ->GetCurrentLength() << endl;
	testQ->DisplayQueue();

	status = testQ->AddElement(1.1);
	cout << "Add  " << status << "   Current length: " << testQ->GetCurrentLength() << endl;
	testQ->DisplayQueue();

	status = testQ->AddElement(2.2);
	cout << "Add  " << status << "   Current length: " << testQ->GetCurrentLength() << endl;
	testQ->DisplayQueue();

	status = testQ->AddElement(3.3);
	cout << "Add  " << status << "   Current length: " << testQ->GetCurrentLength() << endl;
	testQ->DisplayQueue();

	status = testQ->AddElement(4.4);
	cout << "Add  " << status << "   Current length: " << testQ->GetCurrentLength() << endl;
	testQ->DisplayQueue();

	status = testQ->Delete();
	cout << "Del  " << status << "   Current length: " << testQ->GetCurrentLength() << endl;
	testQ->DisplayQueue();

	status = testQ->Delete();
	cout << "Del  " << status << "   Current length: " << testQ->GetCurrentLength() << endl;
	testQ->DisplayQueue();

	status = testQ->Delete();
	cout << "Del  " << status << "   Current length: " << testQ->GetCurrentLength() << endl;
	testQ->DisplayQueue();

	status = testQ->Delete();
	cout << "Del  " << status << "   Current length: " << testQ->GetCurrentLength() << endl;
	testQ->DisplayQueue();

	delete testQ;
*/