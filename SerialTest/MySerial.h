#include "afx.h"
class SerialPort
{
public:
	SerialPort();

public:
	bool OpenPort(char* pPortName, LPDWORD pErrorCode);
	bool ClosePort(LPDWORD pErrorCode);
	bool WriteData(char* pDataToSend, int byteToSend, LPDWORD pErrorCode);
	bool ReadData(char* pDataToReceive, int byteToReceive, LPDWORD pErrorCode);

private:
	HANDLE  hCom;
};