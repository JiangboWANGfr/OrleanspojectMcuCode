#include "Channel.h"
#include <process.h>
#include <iostream>
using namespace std;
#define SleepTime 100
SerialChannel::SerialChannel(SerialParameter* pDefParameter, int defBufferSize)
{
	pSerParameter = new SerialParameter;
	CopyParameter(pDefParameter);

	pComPort = new SerialPort();

	pRecBuffer = new LinkQueue(defBufferSize);

	connectionFlag = 0;
	runningFlag = 0;
	hThread = INVALID_HANDLE_VALUE;
	errorCode = -1;
}

SerialChannel::SerialChannel(string defPortName, int defBaudRate, int defDataBits, int defStopBits, char defParity, int defBufferSize)
{
	pSerParameter = new SerialParameter;
	pSerParameter->portName = defPortName;
	pSerParameter->baudRate = defBaudRate;
	pSerParameter->dataBits = defDataBits;
	pSerParameter->stopBits = defStopBits;
	pSerParameter->parity = defParity;

	pComPort = new SerialPort();

	pRecBuffer = new LinkQueue(defBufferSize);

	connectionFlag = 0;
	runningFlag = 0;
	hThread = INVALID_HANDLE_VALUE;
	errorCode = -1;
}

SerialChannel::~SerialChannel()
{
	if(runningFlag)         //如果线程还在运行则结束线程
	{
		runningFlag = 0;
		WaitForSingleObject(hThread, INFINITE);
		CloseHandle(hThread);
	}
	if (connectionFlag)     //如果串口未关闭则关闭串口
	{
		pComPort->ClosePort(&errorCode);    //错误无法处理
		connectionFlag = 0;
	}
	delete pSerParameter;
	delete pComPort;
	delete pRecBuffer;
	pSerParameter = NULL;
	pComPort = NULL;
	pRecBuffer = NULL;
}

string SerialChannel::GetPortName()
{
	return pSerParameter->portName;
}

int SerialChannel::GetBaudRate()
{
	return pSerParameter->baudRate;
}

int SerialChannel::GetDataBits()
{
	return pSerParameter->dataBits;
}

int SerialChannel::GetStopBits()
{
	return pSerParameter->stopBits;
}

char SerialChannel::GetParity()
{
	return pSerParameter->parity;
}

SerialParameter* SerialChannel::GetParameter()
{
	return pSerParameter;
}

int SerialChannel::GetBufferSize()
{
	return pRecBuffer->GetMaxLength();
}

int SerialChannel::GetDataCount()
{
	return pRecBuffer->GetCurrentLength();
}

DWORD SerialChannel::GetErrorCode()
{
	return errorCode;
}

bool SerialChannel::SetPortName(string defPortName)
{
	if (!connectionFlag)
	{
		pSerParameter->portName = defPortName;
		return true;
	}
	else
	{
		return false;
	}
}

bool SerialChannel::SetBaudRate(int defBaudRate)
{
	if (!connectionFlag)
	{
		pSerParameter->baudRate = defBaudRate;
		return true;
	}
	else
	{
		return false;
	}
}

bool SerialChannel::SetDataBits(int defDataBits)
{
	if (!connectionFlag)
	{
		pSerParameter->dataBits = defDataBits;
		return true;
	}
	else
	{
		return false;
	}
}

bool SerialChannel::SetStopBits(int defStopBits)
{
	if (!connectionFlag)
	{
		pSerParameter->stopBits = defStopBits;
		return true;
	}
	else
	{
		return false;
	}
}

bool SerialChannel::SetParity(char defParity)
{
	if (!connectionFlag)
	{
		pSerParameter->parity = defParity;
		return true;
	}
	else
	{
		return false;
	}
}

bool SerialChannel::SetParameter(SerialParameter* pDefParameter)
{
	if (!connectionFlag)
	{
		CopyParameter(pDefParameter);
		return true;
	}
	else
	{
		return false;
	}
}

void SerialChannel::Display()
{
	pRecBuffer->DisplayQueue();
}

void SerialChannel::CopyParameter(SerialParameter* pDefParameter)
{
	pSerParameter->baudRate = pDefParameter->baudRate;
	pSerParameter->dataBits = pDefParameter->dataBits;
	pSerParameter->parity = pDefParameter->parity;
	pSerParameter->portName = pDefParameter->portName;
	pSerParameter->stopBits = pDefParameter->stopBits;
}

void SerialChannel::ConvertParameter(SerialParameter* pSerParameter, PTCHAR tParameter, UINT count)
{
	swprintf(tParameter, count, L"baud=%d parity=%c data=%d stop=%d", 
						 pSerParameter->baudRate, 
						 pSerParameter->parity, 
						 pSerParameter->dataBits,
						 pSerParameter->stopBits);
}

bool SerialChannel::InitCommunication()
{
	TCHAR dcbParameter[50];
	ConvertParameter(pSerParameter, dcbParameter, 50);

	if (pComPort->OpenPort(pSerParameter->portName.c_str(), dcbParameter, &errorCode))
	{
		connectionFlag = true;
		return true;
	}
	else
	{
		return false;
	}
}

bool SerialChannel::StartCommunication()
{
	if (hThread == INVALID_HANDLE_VALUE)
	{	
		runningFlag = 1;
		hThread = (HANDLE)_beginthreadex(NULL, 0, &RetrieveData, this, 0, NULL);
		if (hThread == 0)    //创建线程失败
		{
			_get_errno((int*)&errorCode);   //待测试的强制类型转换
			runningFlag = 0;
			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		errorCode = -2;       //非运行错误
		return false;
	}
}

bool SerialChannel::TerminateCommunication()
{
	//结束线程
	runningFlag = 0;
	WaitForSingleObject(hThread, INFINITE);
	if (!CloseHandle(hThread))
	{
		errorCode = GetLastError();
		return false;
	}
	//关闭串口
	if (pComPort->ClosePort(&errorCode))
	{
		connectionFlag = 0;
		return true;
	}
	else
	{
		return false;
	}
}

unsigned __stdcall SerialChannel::RetrieveData(void* pArgs)
{
	SerialChannel* pChannel = reinterpret_cast<SerialChannel*>(pArgs);   //转换为SerialChannel类指针
	int dataLength = 1;
	int status = 1;
	float data = 0;
	while (pChannel->runningFlag == 1)            //详细说明见文档
	{
		switch (status)
		{
		case 1:
		{
			char* recTemp = new char[dataLength];
			if (pChannel->pComPort->ReadData(recTemp, dataLength, &pChannel->errorCode))
			{
				if (recTemp[0] == (char)0xFF)     //初始化和结束指令
				{
					dataLength = 1;
					status = 1;
				}
				else if ((recTemp[0] & 0x80) == 0x80)       //最高位为1
				{
					dataLength = recTemp[0] & 0x7F;
					status = 2;
				}
				else if ((recTemp[0] & 0x80) == 0x00)       //最高位为0
				{
					dataLength = recTemp[0] & 0x7F;
					status = 3;
				}
				delete[] recTemp;
				break;
			}
			else
			{
				delete[] recTemp;
				//结束线程
				pChannel->runningFlag = false;
				pChannel->errorCode = -2;
				return 0;
			}
		}
		case 2:       //读整数部分
		{
			char* recTemp = new char[dataLength];
			if (pChannel->pComPort->ReadData(recTemp, dataLength, &pChannel->errorCode))
			{
				data += pChannel->BytesInterpretor(recTemp, dataLength, status);
				dataLength = 1;
				status = 1;
				delete[] recTemp;
				break;
			}
			else
			{
				delete[] recTemp;
				//结束线程
				pChannel->runningFlag = false;
				pChannel->errorCode = -2;
				return 0;
			}
		}
		case 3:    //读小数部分
		{
			char* recTemp = new char[dataLength];
			if (pChannel->pComPort->ReadData(recTemp, dataLength, &pChannel->errorCode))
			{
				data += pChannel->BytesInterpretor(recTemp, dataLength, status);
				if (!pChannel->SaveData(data)) goto L;    //保存失败
				data = 0;
				dataLength = 1;
				status = 1;
				delete[] recTemp;
				break;
			}
			else
			{
			L: delete[] recTemp;
				//结束线程
				pChannel->runningFlag = false;
				pChannel->errorCode = -2;
				return 0;
			}
		}
		default:        //为了完备性而加，实际上不可能运行到此处
		{
			pChannel->runningFlag = false;
			pChannel->errorCode = -2;
			return 0;
		}
		}
		Sleep(SleepTime);
		cout << "Thread Running" << endl;
	}       
	cout << "Thread End" << endl;
	return 0;
}

float SerialChannel::BytesInterpretor(char* recBytes, int bytesCount, int status)
{
	float retVal = 0;
	if (status == 2)
	{
		int scale = 1;
		for (int i = bytesCount-1; i >= 0; i--)
		{
			retVal += scale * (recBytes[i] - 48);
			scale = scale * 10;
		}
		return retVal;
	}
	else if (status == 3)
	{
		float scale = (float)0.1;
		for (int i = 0; i < bytesCount; i++)
		{
			retVal += scale * (recBytes[i] - 48);
			scale = scale / 10;
		}
		return retVal;
	}
	else
	{
		return retVal;
	}
}

bool SerialChannel::SaveData(float data)
{
	if (pRecBuffer->AddElement(data))
	{
		return true;
	}
	else
	{
		return false;
	}
}