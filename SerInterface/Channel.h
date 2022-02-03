#include <string>
#include <Windows.h>
#include "utilities.h"
#include "MySerial.h"

//Ĭ�ϲ���
#define DefaultBufferSize 10
#define DefaultBaudRate 9600
#define DefaultDataBits 8
#define DefaultStopBits 1
#define DefaultParity 'N'
using namespace std;

typedef struct SerialParameter
{
	string portName; //��������
	int baudRate = 9600;    //������    
	int dataBits = 8;	 //����λ
	int stopBits = 1;	 //ֹͣλ
	char parity = 'N';     //��żУ��λ
}SerialParameter;

class SerialChannel
{
public:
	SerialChannel(SerialParameter* pDefParameter, int defBufferSize);
	SerialChannel(string defPortName, int defBaudRate=DefaultBaudRate, int defDataBits=DefaultDataBits, int defStopBits=DefaultStopBits, char defParity=DefaultParity, int defBufferSize=DefaultBufferSize);
	~SerialChannel();

public:
	string GetPortName();
	int GetBaudRate();
	int GetDataBits();
	int GetStopBits();
	char GetParity();
	SerialParameter* GetParameter();
	int GetBufferSize();       //��ȡ��������С
	int GetDataCount();         //��ȡ�������е����ݸ���
	DWORD GetErrorCode();
	
	bool SetPortName(string defPortName);
	bool SetBaudRate(int defBaudRate);
	bool SetDataBits(int defDataBits);
	bool SetStopBits(int defStopBits);
	bool SetParity(char defParity);
	bool SetParameter(SerialParameter* pDefParameter);
	void Display();               //�����ú���

public:
	bool InitCommunication();   //��ʼ��
	bool StartCommunication();
	bool TerminateCommunication();

private:
	void CopyParameter(SerialParameter* pDefParameter);
	void ConvertParameter(SerialParameter* pSerParameter, PTCHAR tParameter, UINT count);  //��ͨ�Ų���ת��Ϊ��׼��ʽ
	static unsigned __stdcall RetrieveData(void* pArgs);            //�Ӵ��ڶ�ȡ���ݲ�����
	float BytesInterpretor(char* recBytes, int bytesCount, int status);
	bool SaveData(float data);

private:
	SerialParameter* pSerParameter;    //ͨ�Ų���
	SerialPort* pComPort;         //ͨ�Ŵ���
	LinkQueue* pRecBuffer;        //���ݽ��ջ�����
	bool connectionFlag;          //�Ƿ��Ѿ������ϴ���
	bool runningFlag;             //�߳��Ƿ�����
	HANDLE hThread;               //�߳̾��
	DWORD errorCode;              //������
};