#include "comaccess.h"

BOOL GetEnter()
{
	if (console_input != INVALID_HANDLE_VALUE)
	{
		// �G���[�\��
		PrintString("Press Enter to exit...\n");
		char simpleBuffer;
		DWORD tmp;
		// �L�[���͂�҂�
		ReadConsoleA(console_input, &simpleBuffer, 1, &tmp, NULL);
		return TRUE;
	}
	return FALSE;
}
