#include "comaccess.h"

// �R���\�[�����͂̃n���h��
HANDLE console_input;
// �R���\�[���o�͂̃n���h��
HANDLE console_output;

DWORD ConsoleInit()
{
    // �R���\�[�����̓n���h��������
    console_input = GetStdHandle(STD_INPUT_HANDLE);
    // �G���[����
    if (console_input == INVALID_HANDLE_VALUE) {
        return GetLastError();
    };
    // �R���\�[���o�̓n���h��������
    console_output = GetStdHandle(STD_OUTPUT_HANDLE);
    // �G���[����
    if (console_output == INVALID_HANDLE_VALUE) {
        return GetLastError();
    };
    // �R���\�[�����[�h���w�肷��
    SetConsoleMode(
        console_input,
        // CTRL+C���V�X�e���v���Z�X�ɗ����Ȃ��悤�ɂ���
        (ENABLE_ECHO_INPUT | ENABLE_INSERT_MODE | ENABLE_LINE_INPUT)
    );
    return 0;
}
