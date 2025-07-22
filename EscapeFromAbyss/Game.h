#pragma once
#include "Controller.h"
#include "Camera.h"
#include "Renderer.h"

#include <vector>
#include <array>
#include <chrono>

class Game
{
public:
	Game() : lastTime(std::chrono::steady_clock::now()) { }
	~Game() = default;

	bool Init(void);
	void Load(void);
	void Update(void);

	bool exitFlg = false;

private:
	/* ゲーム基盤　ここから */
	std::chrono::time_point<std::chrono::steady_clock> lastTime;
	std::chrono::milliseconds elapsedTime = { };

	const int targetFrameTime = static_cast<int>(1000.0 / FPS);

	bool FrameCheck(void);
	void DebugOutputFPS(void);
	/* ゲーム基盤　ここまで */

	/* ゲーム構成パーツ　ここから */
	GDIPlusRenderer m_Renderer;
	Camera m_Camera;
	Player player;
	std::vector<Character> characters;
	Controller m_Controller;

	GAME_SCENE m_Scene = GAME_SCENE::SCENE_PLAYING;
	bool continueFlg = false;
	/* ゲーム構成パーツ　ここまで */

	/* ランダムマップ　ここから */
	struct Hole
	{
		int x;
		int y;
	};

	struct Room
	{
		int x;
		int y;
		int width;
		int height;
		bool isConnected;
		std::vector<Hole> holes;
	};

	std::vector<Room> m_rooms;
	std::array<std::array<OBJECT_TYPE, MAX_WIDTH>, MAX_HEIGHT> map = { };

	void CreateRandomMap(void);
	void CreateRoom(int, int, int, int);
	void CreateCorridor(Room&);
	void SetCharactersAtRandomFloor(Character::CHARACTER_TYPE, int);

	Room GetRoomFromPosition(int, int);
	/* ランダムマップ　ここまで */

	bool CheckStageClear(Player&);
};
