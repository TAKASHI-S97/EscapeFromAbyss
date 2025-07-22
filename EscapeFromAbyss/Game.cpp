#include "Game.h"

#include <algorithm>

bool Game::Init()
{
	if (!m_Renderer.Init())
	{
		OutputDebugString("Error: Failed to initialize GDI+ renderer.");
		return false;
	}

	return true;
}

void Game::Load()
{
	// ランダムマップ生成
	CreateRandomMap();

	// 描画エンジンにマップをロードさせる
	m_Renderer.LoadMap(map);

	// カメラの位置をプレイヤー座標に基づき初期化
	m_Camera.SetCamera(player);
}

void Game::CreateRandomMap()
{
	// 1.マップを壁で埋め尽くす
	for (auto& row : map) row.fill(OBJECT_TYPE::WALL);

	// 2.部屋を作る
	int roomCount = GetRandomInt(10, 16);
	for (int i = 0; i < roomCount; ++i)
	{
		// 部屋のXはバランスよくばらける
		int roomX = GetRandomInt(i * ((MAX_WIDTH - ROOM_MAX_SIZE) / roomCount), ROOM_MIN_SIZE + i * ((MAX_WIDTH - ROOM_MAX_SIZE) / roomCount));
		
		// 部屋のYはランダム
		int roomY = GetRandomInt(0, MAX_HEIGHT - ROOM_MAX_SIZE);

		// 部屋の幅高さは規定の範囲
		int roomW = GetRandomInt(ROOM_MIN_SIZE, ROOM_MAX_SIZE);
		int roomH = GetRandomInt(ROOM_MIN_SIZE, ROOM_MAX_SIZE);

		// 部屋をマップのバッファ上に作成
		CreateRoom(roomX, roomY, roomW, roomH);
	}
	
	// 3.部屋作った後に全部屋1つずつ通路を作る
	for (auto& room : m_rooms)
	{
		// 通路を作る数を決める（80％で1本、20%で2本）
		int corridorRatio = GetRandomInt(1, 10);

		// 1本作る
		if (corridorRatio <= 8)
		{
			CreateCorridor(room);
		}
		// 2本作る
		else
		{
			CreateCorridor(room);
			CreateCorridor(room);
		}
	}

	while (1)
	{
		// 5.階段をランダム配置
		int stairX = GetRandomInt(0, MAX_WIDTH - 1);
		int stairY = GetRandomInt(0, MAX_HEIGHT - 1);

		// 6.プレイヤーをランダム配置
		int playerX = GetRandomInt(0, MAX_WIDTH - 1);
		int playerY = GetRandomInt(0, MAX_HEIGHT - 1);

		// 両者はFLOORの上しか配置できない
		if ((map[playerY][playerX] == OBJECT_TYPE::FLOOR) &&
			(map[stairY][stairX] == OBJECT_TYPE::FLOOR) &&
			// 且つ必ず画面の半分の距離を離れるように配置
			std::abs(playerY - stairY) + std::abs(playerX - stairX) >= (MAX_HEIGHT + MAX_WIDTH) / 2)
		{
			map[stairY][stairX] = OBJECT_TYPE::STAIR;
			player.InitPlayer(playerX * TILE_SIZE, playerY * TILE_SIZE);
			break;
		}
	}

	// 7.キャラクターをクリア
	characters.clear();

	// 7.幽霊をランダムに3体配置
	SetCharactersAtRandomFloor(Character::CHARACTER_TYPE::TYPE_GHOST, 3);

	// 8.宇宙人をランダムに3体配置
	SetCharactersAtRandomFloor(Character::CHARACTER_TYPE::TYPE_ALIAN, 3);
}

void Game::CreateRoom(int inX, int inY, int inWidth, int inHeight)
{
	// 部屋をマップのバッファ上に反映
	for (int y = inY; (y < inY + inHeight) && (y < MAX_HEIGHT); ++y)
		for (int x = inX; (x < inX + inWidth) && (x < MAX_WIDTH); ++x)
			map[y][x] = OBJECT_TYPE::FLOOR;

	// 部屋を登録
	m_rooms.push_back({ inX, inY, inWidth, inHeight, false, { } });
}

void Game::CreateCorridor(Room& room)
{
	// 通路を作る必要のある部屋は一度接続状態を解除
	room.isConnected = false;

	// 通路は別のマップバッファ上で仮作成し、作成後に元のマップバッファへコピー
	std::array<std::array<OBJECT_TYPE, MAX_WIDTH>, MAX_HEIGHT> backmap = { };
	std::copy(map.begin(), map.end(), backmap.begin());

	// 通路がちゃんとどこかの部屋がつながるまでループ
	while (!room.isConnected)
	{
		// 通路の伸ばす方向を決める
		DIRECTION dir = static_cast<DIRECTION>(GetRandomInt(0, 3));

		// 伸ばす方向毎に通路出入口の座標をランダムに取得
		int holeX = 0;
		int holeY = 0;

		while (1)
		{
			// 使用済みの出入口かのチェックフラグ
			bool holeCheckFlg = false;

			switch (dir)
			{
			case DIRECTION::UP:
				holeX = GetRandomInt(room.x, room.x + room.width - 1);
				holeY = room.y;
				break;

			case DIRECTION::RIGHT:
				holeX = room.x + room.width - 1;
				holeY = GetRandomInt(room.y, room.y + room.height - 1);
				break;

			case DIRECTION::DOWN:
				holeX = GetRandomInt(room.x, room.x + room.width - 1);
				holeY = room.y + room.height - 1;
				break;

			case DIRECTION::LEFT:
				holeX = room.x;
				holeY = GetRandomInt(room.y, room.y + room.height - 1);
				break;
			}

			// 既に出口として使用したかどうかの判定
			for (auto& hole : room.holes)
			{
				if ((holeX == hole.x) && (holeY == hole.y))
				{
					holeCheckFlg = true;
					break;
				}
			}

			// 初めての出入口（フラグがfalseのまま）の場合
			if (!holeCheckFlg) break;
			// 既に出入口になっている場合
			else
			{
				// 出入口の方向を変えてみる
				dir = static_cast<DIRECTION>(GetRandomInt(0, 3));
			}
		}

		// まっすぐに進んだマス数のチェック
		int makeCount = 0;

		// 次の座標を出入口の座標で初期化
		int nextX = holeX;
		int nextY = holeY;

		// 通路を1マスずつ伸ばす
		while (!room.isConnected)
		{
			// 現在の進む方向で次のマスを決定
			switch (dir)
			{
			case DIRECTION::UP:
				--nextY;
				break;

			case DIRECTION::RIGHT:
				++nextX;
				break;

			case DIRECTION::DOWN:
				++nextY;
				break;

			case DIRECTION::LEFT:
				--nextX;
				break;
			}

			// 画面外の場合はやり直し
			if ((nextX < 0) || (nextX >= MAX_WIDTH) || (nextY < 0) || (nextY >= MAX_HEIGHT))
			{
				// 裏マップを元に戻す
				std::copy(map.begin(), map.end(), backmap.begin());
				break;
			}
			// 画面内の場合は次のマスで判断
			else
			{
				// 次のマスが部屋の場合
				if (map[nextY][nextX] == OBJECT_TYPE::FLOOR)
				{
					// 部屋はすべて接続済みの部屋と接続し、接続されていない部屋同士が繋がるのを防ぐ必要がある。
					// しかし、最初の部屋から作った通路は、繋いだ先の部屋は絶対まだどの部屋にも繋がっていない状態なので、
					// 最初に部屋は制限なしで繋げる必要がある。

					// 現在の部屋数をチェック（既に接続済みの部屋をカウント）
					int connectCount = 0;
					for (auto& check : m_rooms) if (check.isConnected) ++connectCount;

					// 既に接続済みの部屋が0個の時（最初の部屋）は制限なし
					if (connectCount == 0)
					{
						// 今の出入口を部屋に登録
						room.holes.push_back({ holeX, holeY });

						// 部屋を繋いでいる状態にステータスを変更し、通路作りをやめる
						room.isConnected = true;

						// 繋いだ先の部屋も繋いだ状態にステータスを変更
						Room targetroom = GetRoomFromPosition(nextX, nextY);
						for (auto& checkRoom : m_rooms)
						{
							if ((checkRoom.x == targetroom.x) && (checkRoom.y == targetroom.y))
								checkRoom.isConnected = true;
						}

						// 通路作りをやめる
						break;
					}
					// 2つ目以降の部屋の場合
					else
					{
						// 次のマスに属する部屋の接続状態が繋いでいない状態なら
						if (!GetRoomFromPosition(nextX, nextY).isConnected)
							// 今回の通路作りはやり直し
							continue;
						// 次のマスに属する部屋の接続状態が繋いでいる状態なら
						else
						{
							// 今の出入口を部屋に登録
							room.holes.push_back({ holeX, holeY });

							// 部屋を繋いでいる状態にステータスを変更
							room.isConnected = true;

							// 通路作りをやめる
							break;
						}
					}
				}
				// 次のマスが壁の場合
				else if (map[nextY][nextX] == OBJECT_TYPE::WALL)
				{
					// 通路を作る
					backmap[nextY][nextX] = OBJECT_TYPE::CORRIDOR;
					
					// 作成カウンタをインクリメント
					++makeCount;
				}
				// 次のマスが通路の場合
				else if (map[nextY][nextX] == OBJECT_TYPE::CORRIDOR)
				{
					// 今の出入口を部屋に登録
					room.holes.push_back({ holeX, holeY });

					// 部屋を繋いでいる状態にステータスを変更
					room.isConnected = true;

					// 通路作りをやめる
					break;
				}

				// 道路作りが進むに連れて、方向を変換する確率が上がる
				// 3マス以上連続に作った場合
				if (makeCount > 3)
				{
					// 33%の確率で曲がる
					int changeRatio = GetRandomInt(1, 3);
					if (changeRatio == 1)
					{
						dir = GetNextRandomDirection(dir);
						makeCount = 0;
					}
				}
				// 5マス以上連続で作った場合
				else if (makeCount > 5)
				{
					// 66%の確率で曲がる
					int changeRatio = GetRandomInt(1, 3);
					if (changeRatio != 1)
					{
						dir = GetNextRandomDirection(dir);
						makeCount = 0;
					}
				}
			}
		}
	}

	// 通路が作れた後（どこかの部屋に繋がった後）、本番のmapに反映
	std::copy(backmap.begin(), backmap.end(), map.begin());
}

void Game::SetCharactersAtRandomFloor(Character::CHARACTER_TYPE type, int count)
{
	for (int i = 0; i < count; )
	{
		int characterX = GetRandomInt(0, MAX_WIDTH - 1);
		int characterY = GetRandomInt(0, MAX_HEIGHT - 1);

		// プレイヤーとキャラの距離が、両者のX・Y座標の差の合計がマップの幅高さ合計の1/4より大きい
		if ((std::abs(player.GetPosition().y / TILE_SIZE - characterY) +
			std::abs(player.GetPosition().x / TILE_SIZE - characterX) >= (MAX_WIDTH + MAX_HEIGHT) / 4) &&
			// 且つその座標が部屋の床の場合のみ配置可能
			(map[characterY][characterX] == OBJECT_TYPE::FLOOR))
		{
			Character character;
			character.InitCharacter(type, characterX * TILE_SIZE, characterY * TILE_SIZE);
			characters.push_back(character);
			++i;
		}
	}
}

Game::Room Game::GetRoomFromPosition(int x, int y)
{
	for (auto& room : m_rooms)
	{
		for (int i = room.x; i < room.x + room.width; ++i)
			for (int j = room.y; j < room.y + room.height; ++j)
				if ((i == x) && (j == y)) return room;
	}
	return { 0 };
}

void Game::Update()
{
	if (FrameCheck())
	{
		switch (m_Scene)
		{
		case GAME_SCENE::SCENE_PLAYING:
			m_Controller.Update(player);

			player.Update(map);

			for (auto& enemy : characters)
				if (enemy.activeFlg) 
					enemy.Update(map, player);

			m_Camera.SetCamera(player);

			m_Renderer.Update(m_Camera, player, characters);
			
			if (CheckStageClear(player)) 
				m_Scene = GAME_SCENE::SCENE_ENDSTAGE;

			break;

		case GAME_SCENE::SCENE_ENDSTAGE:
			m_Renderer.Update();

			m_Controller.Update(continueFlg, exitFlg);

			if (continueFlg)
			{
				continueFlg = false;
				m_Scene = GAME_SCENE::SCENE_PLAYING;
				Load();
			}

			break;
		}

#ifdef _DEBUG
		DebugOutputFPS();
#endif
	}
}

bool Game::CheckStageClear(Player& player)
{
	int x = 0;
	int y = 0;
	if ((player.GetPosition().x % TILE_SIZE == 0) &&
		(player.GetPosition().y % TILE_SIZE == 0))
	{
		x = player.GetPosition().x / TILE_SIZE;
		y = player.GetPosition().y / TILE_SIZE;
		if (map[y][x] == OBJECT_TYPE::STAIR)
			return true;
	}

	return false;
}

bool Game::FrameCheck()
{
	auto currentTime = std::chrono::steady_clock::now();
	elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime);

	if (elapsedTime.count() > targetFrameTime)
	{
		lastTime = currentTime;
		return true;
	}

	return false;
}

void Game::DebugOutputFPS()
{
	RECT FPSDebugRect = { WINDOW_WIDTH - 120, 0, WINDOW_WIDTH, 20 };
	float currentFPS = static_cast<float>(1000.0 / elapsedTime.count());
	char FPSString[12];
	sprintf_s(FPSString, 12, "FPS: %.1f", currentFPS);
	DrawText(m_Renderer.GetRenderTargetDC(), FPSString, 10, &FPSDebugRect, DT_RIGHT);
}
