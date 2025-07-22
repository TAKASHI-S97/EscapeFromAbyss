#include "Common.h"

#include <Windows.h>
#include <stdarg.h>
#include <cstdio>

/***********************************************
	�y�֐��z	�t�H�[�}�b�g�����̕�������f�o�b�O�o��
	�y�@�\�z	�t�H�[�}�b�g�����̕�������f�o�b�O�o��
	�y���l�z	������ char* �^�ł���K�v������A
	�@�@�@�@	TEXT()�}�N���͎g�p���Ȃ��悤��
***********************************************/
void OutputDebugFormatString(const char* format, ...)
{
	va_list vl;
	va_start(vl, format);

	char buffer[1024];
	vsnprintf_s(buffer, sizeof(buffer), format, vl);
	va_end(vl);

	OutputDebugString(buffer);
}

/***********************************************
	�y�֐��z	�͈͎w��Ń����_���Ȑ������擾
	�y�@�\�z	�͈͎w��Ń����_���Ȑ������擾
	�y���l�z	�g�p����ۂ͑�1�����͑�2������菬���������Őݒ肷��K�v������
***********************************************/
int GetRandomInt(int start, int end)
{
	std::mt19937 gen{ std::random_device{}() };
	std::uniform_int_distribution<int>dist(start, end);
	return dist(gen);
}

/***********************************************
	�y�֐��z	���݂̕����Ɛ����ɂȂ郉���_���ȕ������擾
	�y�@�\�z	���݂̕����Ɛ����ɂȂ郉���_���ȕ������擾
	�y���l�z	�Ȃ�
***********************************************/
DIRECTION GetNextRandomDirection(DIRECTION dir)
{
	// ���̕����͌��݂̕����ɑ΂��Đ����ɂȂ邢���ꂩ
	DIRECTION nextDir;
	int nextDirRatio = GetRandomInt(0, 1);
	if (nextDirRatio) nextDir = static_cast<DIRECTION>((static_cast<int>(dir) + 1) % 4);
	else nextDir = static_cast<DIRECTION>((static_cast<int>(dir) + 3) % 4);

	return nextDir;
}
