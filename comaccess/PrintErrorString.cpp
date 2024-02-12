#include "comaccess.h"

DWORD PrintErrorString(DWORD error)
{
	// �G���[�ʂ̕���
	switch (error)
	{
	case 2:
		PrintString("The system cannot find the file specified.\n");
		break;
	case 31:
		PrintString("Another process may be in use.\n");
		break;
	default:
		PrintString("Unregistered error.\n");
		break;
	}
	// url�U��
	PrintString("For more information see.\nhttps://learn.microsoft.com/ja-jp/windows/win32/debug/system-error-codes\n\n");
	PrintString("ErrorCode:");
	// �G���[�R�[�h�𕶎���ɕϊ�����
	CHAR codetostr[32];
	_ultoa_s(error, codetostr, 32, 10);
	PrintString(codetostr);
	PrintString("\n\n");
	// �L�[���͂�҂�
	GetEnter();
	// �G���[�ԍ���߂�
	return error;
}