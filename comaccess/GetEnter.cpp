#include "comaccess.h"

BOOL GetEnter()
{
	if (console_input != INVALID_HANDLE_VALUE)
	{
		// エラー表示
		PrintString("Press Enter to exit...\n");
		char simpleBuffer;
		DWORD tmp;
		// キー入力を待つ
		ReadConsoleA(console_input, &simpleBuffer, 1, &tmp, NULL);
		return TRUE;
	}
	return FALSE;
}
