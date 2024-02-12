#include<Windows.h>
#include<strsafe.h>
#include<vector>
#include<memory>
#define BUFFERSIZE 1024

// console input handle
HANDLE console_input = INVALID_HANDLE_VALUE;
// console output handle
HANDLE console_output = INVALID_HANDLE_VALUE;
// comport io handle
HANDLE comport = INVALID_HANDLE_VALUE;
DCB comport_DCB;

BOOL WINAPI CtrlHandler(DWORD dwCtrlType)
{
    if (comport != INVALID_HANDLE_VALUE)
    {
		return CloseHandle(comport);
    }
    return TRUE;
}

int main(int argc, char const* argv[])
{

    // ctrl handler
    if (!SetConsoleCtrlHandler(CtrlHandler, TRUE)) 
    {
        return GetLastError();
    }

    // args
    LPCCH comName;
    DWORD comSpeed;
    DWORD comBits;
    if (argc >= 2) {
        comName = argv[1];
    }
    else {
        return -1;
    }
    if (argc >= 3) {
        comSpeed = strtoul(argv[2], NULL, 10);
    }
    else {
        comSpeed = 9600;
    }
    if (argc >= 4) {
        comSpeed = strtoul(argv[3], NULL, 10);
        if (comSpeed < 5 || comSpeed>8) {
            return -1;
        };
    }
    else {
        comBits = 8;
    };

    // get handles
    console_input =
        GetStdHandle(STD_INPUT_HANDLE); //input
    // error
    if (console_input == INVALID_HANDLE_VALUE) {
        return GetLastError();
    };
    console_output =
        GetStdHandle(STD_OUTPUT_HANDLE); // output
    // error
    if (console_output == INVALID_HANDLE_VALUE) {
        return GetLastError();
    };

    // ctrl + c 
    SetConsoleMode(
        console_input,
        (ENABLE_ECHO_INPUT | ENABLE_INSERT_MODE | ENABLE_LINE_INPUT)
    );

    // comport
    comport = CreateFileA(
        comName, // comport number
        GENERIC_READ | GENERIC_WRITE, // mode
        0, // share
        NULL,
        OPEN_EXISTING, // open mode
        0,
        NULL
    );


    // error
    if (comport == INVALID_HANDLE_VALUE) {
        return GetLastError();
    }
    if (GetCommState(
        comport,
        &comport_DCB
    ))
    {
        // comport settings
        comport_DCB.BaudRate = comSpeed; //speed
        comport_DCB.ByteSize = comBits; //bit
        comport_DCB.Parity = NOPARITY; //parity
        comport_DCB.StopBits = ONESTOPBIT;
        comport_DCB.fOutxCtsFlow = FALSE;
        comport_DCB.fRtsControl = RTS_CONTROL_ENABLE;
        comport_DCB.fAbortOnError = TRUE;
        if (!(SetCommState(comport, &comport_DCB))) {
            return GetLastError();
        };
    }
    // error
    else {
        return GetLastError();
    };

    COMMTIMEOUTS timeout;
    timeout.ReadIntervalTimeout = MAXDWORD;
    timeout.ReadTotalTimeoutConstant = 0;
    timeout.ReadTotalTimeoutMultiplier = 0;
    timeout.WriteTotalTimeoutConstant = 0;
    timeout.WriteTotalTimeoutMultiplier = 0;

    SetCommTimeouts(
        comport,
        &timeout
    );

    // buffers
    std::unique_ptr<INPUT_RECORD[]> console_input_data(new INPUT_RECORD[BUFFERSIZE]);
    DWORD console_input_length = 0; // console input buffer length
    std::vector<CHAR> comport_input_data; // comport input buffer
    comport_input_data.reserve(BUFFERSIZE*3);
    std::unique_ptr<CHAR[]> comport_output_data(new CHAR[BUFFERSIZE]);
    DWORD comport_output_length = 0; // comport output buffer length

    // loops
    while (true) {
        //std::cout << "console_load" << std::endl;
        // console -> serial
        DWORD console_load;
        GetNumberOfConsoleInputEvents(
            console_input,
            &console_load
        );
        if (console_load != 0) {
            ReadConsoleInputA(
                console_input, // handle
                console_input_data.get(), // buffer
                BUFFERSIZE, // reserved buffer size
                &console_input_length // actual buffer size
            );
            // bufferLoad
            for (DWORD i = 0; i < console_input_length; i++) {
                if (console_input_data[i].EventType == KEY_EVENT)
                {
                    KEY_EVENT_RECORD event = console_input_data[i].Event.KeyEvent;
                    if (event.bKeyDown == TRUE) {
                        switch (event.wVirtualKeyCode)
                        {
                        case VK_RETURN:
                            comport_input_data.push_back(10);
                            break;
                            // ANSI ESCAPE CODE
                        case VK_SHIFT:
                            break;
                        case VK_UP:
                            comport_input_data.push_back(27);
                            comport_input_data.push_back('[');
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
                        default:
                            CHAR c = event.uChar.AsciiChar;
                            comport_input_data.push_back(c);
                            break;
                        }
                    };
                };
            };
            DWORD write_size;
            WriteFile(
                comport,
                comport_input_data.data(),
                comport_input_data.size(),
                &write_size,
                NULL
            );
        }
        // serial -> console 
        ReadFile(
            comport,
            comport_output_data.get(),
            BUFFERSIZE,
            &comport_output_length,
            NULL
        );
        WriteConsoleA(
            console_output,
            comport_output_data.get(),
            comport_output_length,
            NULL,
            NULL
        );
        // reset
        comport_input_data.clear();
        console_input_length = 0;
        comport_output_length = 0;
    };
};