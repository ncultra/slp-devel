#include "lslp-windows.h"


void WindowsStartNetwork(void)
{
	WSADATA wsData;
	WSAStartup (  0x0002,   &wsData );
}	
