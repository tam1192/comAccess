#include "comaccess.h"

BOOL PrintString(LPCSTR str)
{
	if(console_output!=INVALID_HANDLE_VALUE)
	{
		int counter = 0;
		while (str[counter] != '\0')
		{
			counter++;
		}
		// �R���\�[���ɏ�������
		WriteConsoleA(
			console_output, // �n���h��
			str, // ������
			counter, // null������������������̒���
			NULL,
			NULL
		);
		return TRUE;
	}
	return FALSE;
}
