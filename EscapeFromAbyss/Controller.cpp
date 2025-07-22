#include "Common.h"
#include "Controller.h"

#include <Windows.h>

void Controller::Update(Player& player)
{
	// プレイヤーが移動アニメーション中以外しか操作できない
	if (!player.GetPlayerMovingFlg())
	{
		if (GetKeyState(VK_UP) & 0x8000)
		{
			if (player.GetDirection() != DIRECTION::UP) player.GetDirection() = DIRECTION::UP;
			else player.GetPlayerMovingFlg() = true;
		}

		if (GetKeyState(VK_RIGHT) & 0x8000)
		{
			if (player.GetDirection() != DIRECTION::RIGHT) player.GetDirection() = DIRECTION::RIGHT;
			else player.GetPlayerMovingFlg() = true;
		}

		if (GetKeyState(VK_DOWN) & 0x8000)
		{
			if (player.GetDirection() != DIRECTION::DOWN) player.GetDirection() = DIRECTION::DOWN;
			else player.GetPlayerMovingFlg() = true;
		}

		if (GetKeyState(VK_LEFT) & 0x8000)
		{
			if (player.GetDirection() != DIRECTION::LEFT) player.GetDirection() = DIRECTION::LEFT;
			else player.GetPlayerMovingFlg() = true;
		}
	}
}

void Controller::Update(bool& continueFlg, bool& exitFlg)
{
	if (GetKeyState(VK_RETURN) & 0x8000)
	{
		continueFlg = true;
	}

	if (GetKeyState(VK_ESCAPE) & 0x8000)
	{
		exitFlg = true;
	}

	Sleep(20);
}
