#include <string>
#include <Windows.h>
#include "utilities.h"
#include "MySerial.h"

//默认参数
#define DefaultBufferSize 10
#define DefaultBaudRate 9600
#define DefaultDataBits 8
#define DefaultStopBits 1
#define DefaultParity 'N'
using namespace std;

typedef struct SerialParameter
{
	string portName; //串口名称
	int baudRate = 9600;    //波特率    
	int dataBits = 8;	 //数据位
	int stopBits = 1;	 //停止位
	char parity = 'N';     //奇偶校验位
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
	int GetBufferSize();       //获取缓冲区大小
	int GetDataCount();         //获取缓冲区中的数据个数
	DWORD GetErrorCode();
	
	bool SetPortName(string defPortName);
	bool SetBaudRate(int defBaudRate);
	bool SetDataBits(int defDataBits);
	bool SetStopBits(int defStopBits);
	bool SetParity(char defParity);
	bool SetParameter(SerialParameter* pDefParameter);
	void Display();               //测试用函数

public:
	bool InitCommunication();   //初始化
	bool StartCommunication();
	bool TerminateCommunication();

private:
	void CopyParameter(SerialParameter* pDefParameter);
	void ConvertParameter(SerialParameter* pSerParameter, PTCHAR tParameter, UINT count);  //把通信参数转化为标准格式
	static unsigned __stdcall RetrieveData(void* pArgs);            //从串口读取数据并处理
	float BytesInterpretor(char* recBytes, int bytesCount, int status);
	bool SaveData(float data);

private:
	SerialParameter* pSerParameter;    //通信参数
	SerialPort* pComPort;         //通信串口
	LinkQueue* pRecBuffer;        //数据接收缓冲区
	bool connectionFlag;          //是否已经连接上串口
	bool runningFlag;             //线程是否运行
	HANDLE hThread;               //线程句柄
	DWORD errorCode;              //错误码
};