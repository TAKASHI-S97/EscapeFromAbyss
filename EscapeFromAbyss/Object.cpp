#include "Object.h"

void Player::Update(const std::array<std::array<OBJECT_TYPE, MAX_WIDTH>, MAX_HEIGHT>& map)
{
	// 状態異常確認
	if (stopFlg)
	{
		isMoving = false;
		++stopFrameCount;
		if (stopFrameCount == static_cast<int>(FPS) * 3)
		{
			stopFrameCount = 0;
			stopFlg = false;
		}
	}

	// 動いている時は移動+アニメーション
	if (isMoving)
	{
		PlayerMove(map);

		// 移動時のアニメーション制御
		// 5フレーム毎にアニメーションさせる
		switch (motionCode / 5)
		{
		case 0:
			status = MOTION_STATUS::WALK1;
			break;

		case 1:
			status = MOTION_STATUS::IDOL;
			break;

		case 2:
			status = MOTION_STATUS::WALK2;
			break;

		case 3:
			status = MOTION_STATUS::IDOL;
			break;
		}

		++motionCode;
		if (motionCode == 20) motionCode = 0;
	}
	// 動いていない時
	else
	{
		status = MOTION_STATUS::IDOL;
	}
}

void Player::PlayerMove(const std::array<std::array<OBJECT_TYPE, MAX_WIDTH>, MAX_HEIGHT>& map)
{
	// 最初のフレームで次のマスへの移動を判定
	if (frameCount == 0)
	{
		int nextX = position.x / TILE_SIZE;
		int nextY = position.y / TILE_SIZE;

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

		// 移動可能かどうかの判定
		// 画面外は論外
		if ((nextX < 0) || (nextX > MAX_WIDTH) || (nextY < 0) || (nextY > MAX_HEIGHT))
		{
			isMoving = false;
			return;
		}

		// 次のマスが壁の時もダメ
		if (map[nextY][nextX] == OBJECT_TYPE::WALL)
		{
			isMoving = false;
			return;
		}
	}

	// 移動（ピクセル単位）
	switch (dir)
	{
	case DIRECTION::UP:
		position.y -= speed;
		break;

	case DIRECTION::RIGHT:
		position.x += speed;
		break;

	case DIRECTION::DOWN:
		position.y += speed;
		break;

	case DIRECTION::LEFT:
		position.x -= speed;
		break;
	}

	// 移動開始後の経過フレーム数
	++frameCount;

	// 1マス移動したら止まる
	if (frameCount == TILE_SIZE / speed)
	{
		frameCount = 0;
		isMoving = false;
	}
}

void Player::WrapPosition(const std::array<std::array<OBJECT_TYPE, MAX_WIDTH>, MAX_HEIGHT>& map)
{
	do
	{
		int nextX = GetRandomInt(0, MAX_WIDTH - 1);
		int nextY = GetRandomInt(0, MAX_HEIGHT - 1);

		if (map[nextY][nextX] == OBJECT_TYPE::FLOOR)
		{
			position.x = nextX * TILE_SIZE;
			position.y = nextY * TILE_SIZE;

			break;
		}

	} while (1);
}

void Character::Update(const std::array<std::array<OBJECT_TYPE, MAX_WIDTH>, MAX_HEIGHT>& map, Player& player)
{
	switch (type)
	{
	case CHARACTER_TYPE::TYPE_GHOST:
		if (SearchPlayer(player)) ChasePlayer(player);
		
		// 接触時処理
		if ((position.x == player.GetPosition().x) &&
			(position.y == player.GetPosition().y))
		{
			player.GetSlowFlg() = true;
			activeFlg = false;
			return;
		}

		break;

	case CHARACTER_TYPE::TYPE_ALIAN:
		RandomWalk(map, player);
		break;
	}
}

bool Character::SearchPlayer(const Player& player)
{
	if (std::abs(player.GetPosition().x - position.x) +
		std::abs(player.GetPosition().y - position.y) <= 4 * TILE_SIZE)
		return true;
	else return false;
}

void Character::ChasePlayer(Player& player)
{
	// 最初のフレームで次のマスへの移動を判定
	if (frameCount == 0)
	{
		int nextX = position.x / TILE_SIZE;
		int nextY = position.y / TILE_SIZE;

		// プレイヤーの位置で向く方向を決定
		if (position.x < player.GetPosition().x)
			dir = DIRECTION::RIGHT;
		else if (position.x > player.GetPosition().x)
			dir = DIRECTION::LEFT;

		if (position.y < player.GetPosition().y)
			dir = DIRECTION::DOWN;
		else if (position.y > player.GetPosition().y)
			dir = DIRECTION::UP;

		// 向いている方向から次の座標を求める
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

		// 移動可能かどうかの判定
		// 画面外は論外
		if ((nextX < 0) || (nextX > MAX_WIDTH) || (nextY < 0) || (nextY > MAX_HEIGHT))
		{
			return;
		}
	}

	// 移動（ピクセル単位）
	switch (dir)
	{
	case DIRECTION::UP:
		position.y -= speed;
		break;

	case DIRECTION::RIGHT:
		position.x += speed;
		break;

	case DIRECTION::DOWN:
		position.y += speed;
		break;

	case DIRECTION::LEFT:
		position.x -= speed;
		break;
	}

	// 移動開始後の経過フレーム数
	++frameCount;

	// 1マス移動したらフレームカウントをリセット
	if (frameCount == TILE_SIZE / speed)
	{
		frameCount = 0;
	}

	// 移動時のアニメーション制御
	// 5フレーム毎にアニメーションさせる
	switch (motionCode / 5)
	{
	case 0:
		status = MOTION_STATUS::WALK1;
		break;

	case 1:
		status = MOTION_STATUS::IDOL;
		break;

	case 2:
		status = MOTION_STATUS::WALK2;
		break;

	case 3:
		status = MOTION_STATUS::IDOL;
		break;
	}

	++motionCode;
	if (motionCode == 20) motionCode = 0;
}

void Character::RandomWalk(const std::array<std::array<OBJECT_TYPE, MAX_WIDTH>, MAX_HEIGHT>& map, Player& player)
{
	if (frameCount == 0)
	{
		// 3~5マス（ランダム）移動したらランダムな方向に変える
		if (walkCount == GetRandomInt(3, 5))
		{
			walkCount = 0;
			dir = GetNextRandomDirection(dir);
		}

		do
		{
			int nextX = position.x / TILE_SIZE;
			int nextY = position.y / TILE_SIZE;

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

			// 移動可能かどうかの判定（不可の場合方向を変える）
			// 画面外は論外
			if ((nextX < 0) || (nextX > MAX_WIDTH) || (nextY < 0) || (nextY > MAX_HEIGHT))
			{
				dir = static_cast<DIRECTION>(GetRandomInt(0, 3));
				continue;
			}

			// 次のマスが壁の時もダメ
			if (map[nextY][nextX] == OBJECT_TYPE::WALL)
			{
				dir = static_cast<DIRECTION>(GetRandomInt(0, 3));
				continue;
			}

			break;
		} while (1);
	}

	// 移動（ピクセル単位）
	switch (dir)
	{
	case DIRECTION::UP:
		position.y -= speed;
		break;

	case DIRECTION::RIGHT:
		position.x += speed;
		break;

	case DIRECTION::DOWN:
		position.y += speed;
		break;

	case DIRECTION::LEFT:
		position.x -= speed;
		break;
	}

	// 移動開始後の経過フレーム数
	++frameCount;

	// 1マス移動したらリセットし、カウンタ増やす
	if (frameCount == TILE_SIZE / speed)
	{
		// 接触時処理
		if ((position.x == player.GetPosition().x) &&
			(position.y == player.GetPosition().y))
		{
			// プレイヤーをランダムな（移動可能な）座標にワープ
			player.WrapPosition(map);
			activeFlg = false;

			return;
		}

		frameCount = 0;
		++walkCount;
	}

	// 移動時のアニメーション制御
	// 5フレーム毎にアニメーションさせる
	switch (motionCode / 5)
	{
	case 0:
		status = MOTION_STATUS::WALK1;
		break;

	case 1:
		status = MOTION_STATUS::IDOL;
		break;

	case 2:
		status = MOTION_STATUS::WALK2;
		break;

	case 3:
		status = MOTION_STATUS::IDOL;
		break;
	}

	++motionCode;
	if (motionCode == 20) motionCode = 0;
}
