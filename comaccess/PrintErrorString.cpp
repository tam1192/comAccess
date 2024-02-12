#include "comaccess.h"

DWORD PrintErrorString(DWORD error)
{
	// エラー別の文章
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
	// url誘導
	PrintString("For more information see.\nhttps://learn.microsoft.com/ja-jp/windows/win32/debug/system-error-codes\n\n");
	PrintString("ErrorCode:");
	// エラーコードを文字列に変換する
	CHAR codetostr[32];
	_ultoa_s(error, codetostr, 32, 10);
	PrintString(codetostr);
	PrintString("\n\n");
	// キー入力を待つ
	GetEnter();
	// エラー番号を戻す
	return error;
}