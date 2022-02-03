#include "MySerial.h"
#define SleepInterval 10
SerialPort::SerialPort() : hCom(INVALID_HANDLE_VALUE)
{
}

bool SerialPort::OpenPort(LPCSTR portName, LPCWSTR pParameter, LPDWORD pErrorCode)
{
	hCom = CreateFileA(portName, 
					  GENERIC_READ|GENERIC_WRITE,  // 访问模式
		              0,             // 共享模式
		              NULL,             // 安全性设置
		              OPEN_EXISTING,      
					  0,
					  0);

	if (hCom == INVALID_HANDLE_VALUE) //打开串口失败
	{
		*pErrorCode = GetLastError();
		return false;       
	}

	LPDCB pDCB = new DCB;
	if (GetCommState(hCom, pDCB) && BuildCommDCB(pParameter, pDCB) && SetCommState(hCom, pDCB))  //配置串口参数
	{
		delete pDCB;
		return true;
	}
	else
	{
		delete pDCB;
		*pErrorCode = GetLastError();
		return false;
	}

}

bool SerialPort::ClosePort(LPDWORD pErrorCode)
{
	if (CloseHandle(hCom))   //成功关闭串口
	{
		return true;
	}
	else
	{
		*pErrorCode = GetLastError();
		return false;
	}
}

bool SerialPort::WriteData(char* pDataToSend, int byteToSend, LPDWORD pErrorCode)
{
	DWORD byteSent = 0;     //成功写入的字节数
	do
	{
		if (WriteFile(hCom, pDataToSend, byteToSend, &byteSent, NULL))   //成功写入
		{
			byteToSend -= byteSent;
			pDataToSend += byteSent;
		}
		else
		{
			*pErrorCode = GetLastError();
			return false;
		}
	} while (byteToSend > 0);     //循环直至所有数据全部发送完毕
	return true;
}

bool SerialPort::ReadData(char* pDataToReceive, int byteToReceive, LPDWORD pErrorCode)
{
	DWORD byteReceived;   //收到的字节数
	while(1)  //循环直至收到指定数量的数据
	{
		if (ReadFile(hCom, pDataToReceive, byteToReceive, &byteReceived, NULL))
		{
			byteToReceive -= byteReceived;
			pDataToReceive += byteReceived;
		}
		else
		{
			*pErrorCode = GetLastError();
			return false;
		}
		if (byteToReceive > 0)     //如果还有字节未收到
		{
			Sleep(SleepInterval);       //等待数据到来
		}
		else
		{
			break;
		}
	}	
	return true;
}