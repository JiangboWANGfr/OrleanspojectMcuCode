#include <Windows.h>
class SerialPort
{
public:
	SerialPort();

public:
	bool OpenPort(LPCSTR pPortName, LPCWSTR pParameter, LPDWORD pErrorCode);
	bool ClosePort(LPDWORD pErrorCode);
	bool WriteData(char* pDataToSend, int byteToSend, LPDWORD pErrorCode);
	bool ReadData(char* pDataToReceive, int byteToReceive, LPDWORD pErrorCode);

private:
	HANDLE  hCom;
};