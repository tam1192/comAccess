#pragma once
#include<Windows.h>
#include<strsafe.h>
#include<vector>
#include<memory>
// バッファーのサイズ
#define BUFFERSIZE 1024

// コンソール入力のハンドル
extern HANDLE console_input;
// コンソール出力のハンドル
extern HANDLE console_output;
// comportの入出力のハンドル
extern HANDLE comport;

/**
 * プログラム終了時処理.
 * 
 * \param dwCtrlType コントロールのタイプ（シグナル）
 * \return TRUEだと正常に動作
 */
BOOL WINAPI CtrlHandler(DWORD);

/**
 * コンソール入出力用ハンドラの初期化.
 * 
 * \return 0は成功、その他GetLastErrorの戻り値
 */
DWORD ConsoleInit();

/**
 * 文字列を標準出力に投げる.
 * 
 * \param str 文字列
 * \return 成功か否か
 */
BOOL PrintString(LPCSTR);

/**
 * エラーメッセージ表示関数.
 * 
 * \param error番号
 * \return 
 */
DWORD PrintErrorString(DWORD);

/**
 * enterキーの入力を待ちます.
 * 
 * \return 成功か否か
 */
BOOL GetEnter();
