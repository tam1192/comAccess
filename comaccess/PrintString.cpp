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
		// コンソールに書き込む
		WriteConsoleA(
			console_output, // ハンドル
			str, // 文字列
			counter, // null文字を除いた文字列の長さ
			NULL,
			NULL
		);
		return TRUE;
	}
	return FALSE;
}
