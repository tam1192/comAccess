#include<Windows.h>
#include<strsafe.h>
#include<vector>
#include<memory>
// バッファーのサイズ
#define BUFFERSIZE 1024

// コンソール入力のハンドル
HANDLE console_input = INVALID_HANDLE_VALUE;
// コンソール出力のハンドル
HANDLE console_output = INVALID_HANDLE_VALUE;
// comportの入出力のハンドル
HANDLE comport = INVALID_HANDLE_VALUE;
// comportのDCB構造体
DCB comport_DCB;
// comportのtimeout構造体
COMMTIMEOUTS comport_timeout;

/**
 * プログラム終了時処理.
 * 
 * \param dwCtrlType コントロールのタイプ（シグナル）
 * \return TRUEだと正常に動作
 */
BOOL WINAPI CtrlHandler(DWORD dwCtrlType)
{
    if (comport != INVALID_HANDLE_VALUE)
    {
		return CloseHandle(comport);
    }
    return TRUE;
}

/**
 * COMポートをコンソールで扱うプログラムです.
 * 
 * \param argc 引数の長さ
 * \param argv[1] Comポートの名前
 * \param argv[2] Comポートの速度(デフォルト:9600)
 * \param argv[3] Comポートのbit数(デフォルト:8)
 * \return GetLastErrorの戻り値
 */
int main(int argc, char const* argv[])
{
    // 終了割込みハンドラ
    if (!SetConsoleCtrlHandler(CtrlHandler, TRUE)) 
    {
        // エラーを戻り値へ
        return GetLastError();
    }

    // 引数の処理
    LPCCH comName; // COMポートの名前
    DWORD comSpeed; // COMポートの速度
    DWORD comBits; // COMポートのbit
    // 名前まで
    if (argc >= 2) {
        // そのまま代入
        comName = argv[1];
    }
    // 何もないとき
    else {
        // COMポート未指定のエラー
        return -1;
    }
    // 速度まで
    if (argc >= 3) {
        // 数値に直して代入
        comSpeed = strtoul(argv[2], NULL, 10);
    }
    else {
        // デフォルト
        comSpeed = 9600;
    }
    // bitまで
    if (argc >= 4) {
        // 数値に直して代入
        comSpeed = strtoul(argv[3], NULL, 10);
        if (comSpeed < 5 || comSpeed>8) {
            // 指定外の数字はエラー
            return -1;
        };
    }
    else {
        // デフォルト
        comBits = 8;
    };

    // コンソール入力ハンドラを入れる
    console_input =
        GetStdHandle(STD_INPUT_HANDLE); //input
    // エラー処理
    if (console_input == INVALID_HANDLE_VALUE) {
        return GetLastError();
    };
    // コンソール出力ハンドラを入れる
    console_output =
        GetStdHandle(STD_OUTPUT_HANDLE); // output
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

    // comポートハンドラを入れる
    comport = CreateFileA(
        comName, // コムポートの名前
        GENERIC_READ | GENERIC_WRITE, // アクセスモード
        0, // 通信デバイスは共有できない
        NULL, // 無視
        OPEN_EXISTING, // 通信デバイスは固定値
        0, // 無視
        NULL // 無視
    );
    // エラー処理
    if (comport == INVALID_HANDLE_VALUE) {
        return GetLastError();
    }
    // 現在のComポートのDCBステータスをDCB構造体に代入
    if (GetCommState(
        comport,
        &comport_DCB
    ))
    {
        // comport settings
        comport_DCB.BaudRate = comSpeed; // 速度指定
        comport_DCB.ByteSize = comBits; // bit指定
        comport_DCB.Parity = NOPARITY; // パリティーはなし
        comport_DCB.StopBits = ONESTOPBIT; // 終了bitは0
        // 以下一般的な設定
        comport_DCB.fOutxCtsFlow = FALSE; 
        comport_DCB.fRtsControl = RTS_CONTROL_ENABLE;
        comport_DCB.fAbortOnError = TRUE;
        // DCB構造体の変更を適用、エラー処理
        if (!(SetCommState(comport, &comport_DCB))) {
            return GetLastError();
        };
    }
    // エラー処理
    else {
        return GetLastError();
    };
    // コムポートのタイムアウト時間を指定する
    if (GetCommTimeouts(
        comport,
        &comport_timeout
    ))
    {
        // タイムアウトを無効化する
		comport_timeout.ReadIntervalTimeout = MAXDWORD;
		comport_timeout.ReadTotalTimeoutConstant = 0;
		comport_timeout.ReadTotalTimeoutMultiplier = 0;
		comport_timeout.WriteTotalTimeoutConstant = 0;
		comport_timeout.WriteTotalTimeoutMultiplier = 0;
        // timeout構造体の変更を適用、エラー処理
        if (!(SetCommTimeouts(comport, &comport_timeout))) {
            return GetLastError();
        };
    }
    // エラー処理
    else {
        return GetLastError();
    };

    // バッファーの作成
    // コンソールから入力されたレコードを入れるバッファ
    std::unique_ptr<INPUT_RECORD[]> console_input_data(new INPUT_RECORD[BUFFERSIZE]);
    // 実際にコンソールから入力されたレコード数
    DWORD console_input_length = 0;
    // comportに書き込むときに使うバッファ
    std::vector<CHAR> comport_input_data;
    // あらかじめメモリだけ確保しておく
    comport_input_data.reserve(BUFFERSIZE*3);
    // COMポートの出力を入れるバッファ
    std::unique_ptr<CHAR[]> comport_output_data(new CHAR[BUFFERSIZE]);
    // 実際に取れた数
    DWORD comport_output_length = 0;

    // ループ処理開始
    while (true) {
        DWORD console_load; // コンソールに溜まってるレコードの数
        // console_load変数にレコード数を入れる
        GetNumberOfConsoleInputEvents(
            console_input,
            &console_load
        );
        // レコード数が1を超えていたら
        if (console_load != 0) {
            // バッファにデータを移す
            ReadConsoleInputA(
                console_input, // ハンドラ
                console_input_data.get(), // バッファの先頭ポインタ
                BUFFERSIZE, // バッファの最大長
                &console_input_length // 実際に使われたバッファの長さ
            );
            // バッファに入れられたレコードを一つずつ文字に変換する
            for (DWORD i = 0; i < console_input_length; i++) {
                // レコードがキー入力であるなら
                if (console_input_data[i].EventType == KEY_EVENT)
                {
                    // 変数が長くならないようにする
                    KEY_EVENT_RECORD& event = console_input_data[i].Event.KeyEvent;
                    // キーが押されていたら...
                    if (event.bKeyDown == TRUE) {
                        // 仮想キーコードが...
                        switch (event.wVirtualKeyCode)
                        {
                        // enterならasciiの10番を入れる
                        case VK_RETURN:
                            comport_input_data.push_back(10);
                            break;
                        // shiftは単体で使用することがないので無効化
                        case VK_SHIFT:
                            break;
                        // 矢印キーの処理
                        // 矢印キーにはasciiコードが指定されていない
                        // そこで、エスケープ処理を使って表現する
                        // なお、エスケープ処理はANSIエスケープシーケンス
                        // という規格があるので従う
                        case VK_UP:
                            // エスケープ制御文
                            comport_input_data.push_back(27);
                            // ANSI規格
                            comport_input_data.push_back('[');
                            // Aは矢印上を意味する
                            comport_input_data.push_back('A');
                            break;
                        case VK_DOWN:
                            comport_input_data.push_back(27);
                            comport_input_data.push_back('[');
                            comport_input_data.push_back('B');
                            break;
                        case VK_RIGHT:
                            comport_input_data.push_back(27);
                            comport_input_data.push_back('[');
                            comport_input_data.push_back('C');
                            break;
                        case VK_LEFT:
                            comport_input_data.push_back(27);
                            comport_input_data.push_back('[');
                            comport_input_data.push_back('D');
                            break;
                        // その他
                        default:
                            // Ascii文字コードにを取り出す
                            CHAR c = event.uChar.AsciiChar;
                            // 書き込みバッファに入れる
                            comport_input_data.push_back(c);
                            break;
                        }
                    };
                };
            };
            // 書き込みバッファに代入されたデータを実際に書き込む
            WriteFile(
                comport, // ハンドル
                comport_input_data.data(), // バッファの先頭アドレス
                comport_input_data.size(), // バッファのサイズ
                NULL,
                NULL
            );
        }
        // COMポートからデータを読み取る
        ReadFile(
            comport, // ハンドル
            comport_output_data.get(), // バッファのポインタ
            BUFFERSIZE, // バッファのサイズ
            &comport_output_length, // 実際に取得したサイズ
            NULL
        );
        // コンソールに書き込む
        WriteConsoleA(
            console_output, // ハンドル
            comport_output_data.get(), // バッファのポインタ
            comport_output_length, // 書き込むサイズ(=実際に取得したサイズ)
            NULL,
            NULL
        );
        // バッファをリセットする
        comport_input_data.clear();
        console_input_length = 0;
        comport_output_length = 0;
    };
};