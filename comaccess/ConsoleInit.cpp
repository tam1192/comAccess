#include "comaccess.h"

// コンソール入力のハンドル
HANDLE console_input;
// コンソール出力のハンドル
HANDLE console_output;

DWORD ConsoleInit()
{
    // コンソール入力ハンドラを入れる
    console_input = GetStdHandle(STD_INPUT_HANDLE);
    // エラー処理
    if (console_input == INVALID_HANDLE_VALUE) {
        return GetLastError();
    };
    // コンソール出力ハンドラを入れる
    console_output = GetStdHandle(STD_OUTPUT_HANDLE);
    // エラー処理
    if (console_output == INVALID_HANDLE_VALUE) {
        return GetLastError();
    };
    // コンソールモードを指定する
    SetConsoleMode(
        console_input,
        // CTRL+Cをシステムプロセスに流さないようにする
        (ENABLE_ECHO_INPUT | ENABLE_INSERT_MODE | ENABLE_LINE_INPUT)
    );
    return 0;
}
