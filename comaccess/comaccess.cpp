#include<Windows.h>
#include<strsafe.h>
#include<iostream>
#define BUFFERSIZE 1024

// console input handle
HANDLE console_input;
// console output handle
HANDLE console_output;
// comport io handle
HANDLE comport;
DCB comport_DCB;
int main(int argc, char const* argv[])
{
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
        }
    }
    else {
        comBits = 8;
    }

    // get handles
    console_input =
        GetStdHandle(STD_INPUT_HANDLE); //input
    // error
    if (console_input == INVALID_HANDLE_VALUE) {
        return GetLastError();
    }
    console_output =
        GetStdHandle(STD_OUTPUT_HANDLE); // output
    // error
    if (console_output == INVALID_HANDLE_VALUE) {
        return GetLastError();
    }

    // comport
    comport = CreateFileA(
        argv[1], // comport number
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
        if (!(SetCommState(comport, &comport_DCB))) {
            return GetLastError();
        }
    }
    // error
    else {
        return GetLastError();
    }

    // buffers
    INPUT_RECORD console_input_data[BUFFERSIZE]; // console input buffer
    DWORD console_input_length = 0; // console input buffer length
    CHAR console_output_data[BUFFERSIZE]; // console output buffer
    DWORD console_output_length = 0; // console output buffer length
    CHAR comport_input_data[BUFFERSIZE]; // comport input buffer
    DWORD comport_input_length = 0; // comport input buffer length
    CHAR comport_output_data[BUFFERSIZE]; // comport output buffer
    DWORD comport_output_length = 0; // comport output buffer length

    // loops
    while (true) {
        // console -> serial
        DWORD console_load;
        GetNumberOfConsoleInputEvents(
            console_input,
            &console_load
        );
        if (console_load != 0) {
            ReadConsoleInputA(
                console_input, // handle
                console_input_data, // buffer
                BUFFERSIZE, // reserved buffer size
                &console_input_length // actual buffer size
            );
            // bufferLoad
            for (DWORD i = 0; i < console_input_length; i++) {
                if (console_input_data[i].EventType == KEY_EVENT)
                {
                    KEY_EVENT_RECORD event = console_input_data[i].Event.KeyEvent;
                    if (event.bKeyDown == TRUE) {
                        if (event.wVirtualKeyCode == VK_RETURN) {
                            comport_input_data[i] = 10;
                            comport_input_length++;
                            break;
                        }
                        else if (
                            event.dwControlKeyState == RIGHT_CTRL_PRESSED ||
                            event.dwControlKeyState == LEFT_CTRL_PRESSED
                            ) {
                            CHAR c = event.uChar.AsciiChar;
                            comport_input_data[i] = (c - 64); // ascii control char
                            comport_input_length++;
                        }
                        else {
                            CHAR c = event.uChar.AsciiChar;
                            comport_input_data[i] = c;
                            comport_input_length++;
                        }
                    };
                };
            };
            DWORD write_size;
            WriteFile(
                comport,
                comport_input_data,
                comport_input_length,
                &write_size,
                NULL
            );
        }
        // serial -> console 
        ReadFile(
            comport,
            comport_output_data,
            BUFFERSIZE,
            &comport_output_length,
            NULL
        );
        WriteConsoleA(
            console_output,
            comport_output_data,
            comport_output_length,
            NULL,
            NULL
        );
        // reset
        comport_input_length = 0;
        console_input_length = 0;
        comport_output_length = 0;
        console_output_length = 0;
    }
};