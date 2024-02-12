#pragma once
#include<Windows.h>
#include<strsafe.h>
#include<vector>
#include<memory>
// �o�b�t�@�[�̃T�C�Y
#define BUFFERSIZE 1024

// �R���\�[�����͂̃n���h��
extern HANDLE console_input;
// �R���\�[���o�͂̃n���h��
extern HANDLE console_output;
// comport�̓��o�͂̃n���h��
extern HANDLE comport;

/**
 * �v���O�����I��������.
 * 
 * \param dwCtrlType �R���g���[���̃^�C�v�i�V�O�i���j
 * \return TRUE���Ɛ���ɓ���
 */
BOOL WINAPI CtrlHandler(DWORD);

/**
 * �R���\�[�����o�͗p�n���h���̏�����.
 * 
 * \return 0�͐����A���̑�GetLastError�̖߂�l
 */
DWORD ConsoleInit();

/**
 * �������W���o�͂ɓ�����.
 * 
 * \param str ������
 * \return �������ۂ�
 */
BOOL PrintString(LPCSTR);

/**
 * �G���[���b�Z�[�W�\���֐�.
 * 
 * \param error�ԍ�
 * \return 
 */
DWORD PrintErrorString(DWORD);

/**
 * enter�L�[�̓��͂�҂��܂�.
 * 
 * \return �������ۂ�
 */
BOOL GetEnter();
