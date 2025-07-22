#pragma once
#include "Game.h"

#include <Windows.h>

class App
{
public:
	App(HINSTANCE hInst) : m_hInst(hInst) { }
	~App();

	bool Init(void);
	void Run(void);

private:
	HINSTANCE m_hInst;
	LPCSTR CLASSNAME = TEXT("MY_WINDOW_CLASS");
	HWND m_hWnd;

	Game m_Game;

	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
};
