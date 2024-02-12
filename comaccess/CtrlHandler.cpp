#include "comaccess.h"

BOOL WINAPI CtrlHandler(DWORD dwCtrlType)
{
    if (comport != INVALID_HANDLE_VALUE)
    {
		return CloseHandle(comport);
    }
    return TRUE;
}
