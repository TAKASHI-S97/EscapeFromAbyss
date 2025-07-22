#include "Common.h"
#include "App.h"

bool WndFlg = true;

App::~App()
{
	// �E�B���h�E�̔j���ƃN���X�o�^����
	DestroyWindow(m_hWnd);
	UnregisterClass(CLASSNAME, GetModuleHandle(nullptr));
}

bool App::Init()
{
	// 1.�E�B���h�E�N���X�̓o�^
	WNDCLASS wc = { };
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hInst;
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = CLASSNAME;

	if (!RegisterClass(&wc))
	{
		OutputDebugFormatString("Error: Failed to register window class.\tErrorCode: %x", GetLastError());
		return false;
	}

	// 2.�E�B���h�E�T�C�Y�̒���
	RECT WndRect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
	if (!AdjustWindowRect(&WndRect, WS_OVERLAPPEDWINDOW, false))
	{
		OutputDebugFormatString("Error: Failed to resize window.\tErrorCode: %x", GetLastError());
		return false;
	}

	// 3.�E�B���h�E�̍쐬
	m_hWnd = CreateWindow(
		CLASSNAME,
		TEXT("Escape From Abyss"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		WndRect.right - WndRect.left,
		WndRect.bottom - WndRect.top,
		nullptr,
		nullptr,
		wc.hInstance,
		nullptr);

	if (!m_hWnd)
	{
		OutputDebugFormatString("Error: Failed to create window.\tErrorCode: %x", GetLastError());
		return false;
	}

	// 4.�E�B���h�E�̕\��
	ShowWindow(m_hWnd, SW_SHOWNORMAL);
	UpdateWindow(m_hWnd);

	// 5.�Q�[���̏�����
	if (!m_Game.Init())
	{
		OutputDebugString(TEXT("Error: Failed to initialize the game."));
		return false;
	}

	return true;
}

void App::Run()
{
	MSG msg = { };

	m_Game.Load();

	while (WndFlg)
	{
		if (PeekMessage(&msg, m_hWnd, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			m_Game.Update();
			if (m_Game.exitFlg) WndFlg = false;
		}
	}
}

LRESULT CALLBACK App::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		WndFlg = false;
		break;

	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
