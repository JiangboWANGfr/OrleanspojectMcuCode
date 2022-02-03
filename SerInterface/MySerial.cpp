#include "MySerial.h"
#define SleepInterval 10
SerialPort::SerialPort() : hCom(INVALID_HANDLE_VALUE)
{
}

bool SerialPort::OpenPort(LPCSTR portName, LPCWSTR pParameter, LPDWORD pErrorCode)
{
	hCom = CreateFileA(portName, 
					  GENERIC_READ|GENERIC_WRITE,  // ����ģʽ
		              0,             // ����ģʽ
		              NULL,             // ��ȫ������
		              OPEN_EXISTING,      
					  0,
					  0);

	if (hCom == INVALID_HANDLE_VALUE) //�򿪴���ʧ��
	{
		*pErrorCode = GetLastError();
		return false;       
	}

	LPDCB pDCB = new DCB;
	if (GetCommState(hCom, pDCB) && BuildCommDCB(pParameter, pDCB) && SetCommState(hCom, pDCB))  //���ô��ڲ���
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
	if (CloseHandle(hCom))   //�ɹ��رմ���
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
	DWORD byteSent = 0;     //�ɹ�д����ֽ���
	do
	{
		if (WriteFile(hCom, pDataToSend, byteToSend, &byteSent, NULL))   //�ɹ�д��
		{
			byteToSend -= byteSent;
			pDataToSend += byteSent;
		}
		else
		{
			*pErrorCode = GetLastError();
			return false;
		}
	} while (byteToSend > 0);     //ѭ��ֱ����������ȫ���������
	return true;
}

bool SerialPort::ReadData(char* pDataToReceive, int byteToReceive, LPDWORD pErrorCode)
{
	DWORD byteReceived;   //�յ����ֽ���
	while(1)  //ѭ��ֱ���յ�ָ������������
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
		if (byteToReceive > 0)     //��������ֽ�δ�յ�
		{
			Sleep(SleepInterval);       //�ȴ����ݵ���
		}
		else
		{
			break;
		}
	}	
	return true;
}