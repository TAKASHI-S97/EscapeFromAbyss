#include "Common.h"

#include <Windows.h>
#include <stdarg.h>
#include <cstdio>

/***********************************************
	【関数】	フォーマット書式の文字列をデバッグ出力
	【機能】	フォーマット書式の文字列をデバッグ出力
	【備考】	引数は char* 型である必要がある、
	　　　　	TEXT()マクロは使用しないように
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
	【関数】	範囲指定でランダムな整数を取得
	【機能】	範囲指定でランダムな整数を取得
	【備考】	使用する際は第1引数は第2引数より小さい整数で設定する必要がある
***********************************************/
int GetRandomInt(int start, int end)
{
	std::mt19937 gen{ std::random_device{}() };
	std::uniform_int_distribution<int>dist(start, end);
	return dist(gen);
}

/***********************************************
	【関数】	現在の方向と垂直になるランダムな方向を取得
	【機能】	現在の方向と垂直になるランダムな方向を取得
	【備考】	なし
***********************************************/
DIRECTION GetNextRandomDirection(DIRECTION dir)
{
	// 次の方向は現在の方向に対して垂直になるいずれか
	DIRECTION nextDir;
	int nextDirRatio = GetRandomInt(0, 1);
	if (nextDirRatio) nextDir = static_cast<DIRECTION>((static_cast<int>(dir) + 1) % 4);
	else nextDir = static_cast<DIRECTION>((static_cast<int>(dir) + 3) % 4);

	return nextDir;
}
