#include "Object.h"

void Player::Update(const std::array<std::array<OBJECT_TYPE, MAX_WIDTH>, MAX_HEIGHT>& map)
{
	// ��Ԉُ�m�F
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

	// �����Ă��鎞�͈ړ�+�A�j���[�V����
	if (isMoving)
	{
		PlayerMove(map);

		// �ړ����̃A�j���[�V��������
		// 5�t���[�����ɃA�j���[�V����������
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
	// �����Ă��Ȃ���
	else
	{
		status = MOTION_STATUS::IDOL;
	}
}

void Player::PlayerMove(const std::array<std::array<OBJECT_TYPE, MAX_WIDTH>, MAX_HEIGHT>& map)
{
	// �ŏ��̃t���[���Ŏ��̃}�X�ւ̈ړ��𔻒�
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

		// �ړ��\���ǂ����̔���
		// ��ʊO�͘_�O
		if ((nextX < 0) || (nextX > MAX_WIDTH) || (nextY < 0) || (nextY > MAX_HEIGHT))
		{
			isMoving = false;
			return;
		}

		// ���̃}�X���ǂ̎����_��
		if (map[nextY][nextX] == OBJECT_TYPE::WALL)
		{
			isMoving = false;
			return;
		}
	}

	// �ړ��i�s�N�Z���P�ʁj
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

	// �ړ��J�n��̌o�߃t���[����
	++frameCount;

	// 1�}�X�ړ�������~�܂�
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
		
		// �ڐG������
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
	// �ŏ��̃t���[���Ŏ��̃}�X�ւ̈ړ��𔻒�
	if (frameCount == 0)
	{
		int nextX = position.x / TILE_SIZE;
		int nextY = position.y / TILE_SIZE;

		// �v���C���[�̈ʒu�Ō�������������
		if (position.x < player.GetPosition().x)
			dir = DIRECTION::RIGHT;
		else if (position.x > player.GetPosition().x)
			dir = DIRECTION::LEFT;

		if (position.y < player.GetPosition().y)
			dir = DIRECTION::DOWN;
		else if (position.y > player.GetPosition().y)
			dir = DIRECTION::UP;

		// �����Ă���������玟�̍��W�����߂�
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

		// �ړ��\���ǂ����̔���
		// ��ʊO�͘_�O
		if ((nextX < 0) || (nextX > MAX_WIDTH) || (nextY < 0) || (nextY > MAX_HEIGHT))
		{
			return;
		}
	}

	// �ړ��i�s�N�Z���P�ʁj
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

	// �ړ��J�n��̌o�߃t���[����
	++frameCount;

	// 1�}�X�ړ�������t���[���J�E���g�����Z�b�g
	if (frameCount == TILE_SIZE / speed)
	{
		frameCount = 0;
	}

	// �ړ����̃A�j���[�V��������
	// 5�t���[�����ɃA�j���[�V����������
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
		// 3~5�}�X�i�����_���j�ړ������烉���_���ȕ����ɕς���
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

			// �ړ��\���ǂ����̔���i�s�̏ꍇ������ς���j
			// ��ʊO�͘_�O
			if ((nextX < 0) || (nextX > MAX_WIDTH) || (nextY < 0) || (nextY > MAX_HEIGHT))
			{
				dir = static_cast<DIRECTION>(GetRandomInt(0, 3));
				continue;
			}

			// ���̃}�X���ǂ̎����_��
			if (map[nextY][nextX] == OBJECT_TYPE::WALL)
			{
				dir = static_cast<DIRECTION>(GetRandomInt(0, 3));
				continue;
			}

			break;
		} while (1);
	}

	// �ړ��i�s�N�Z���P�ʁj
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

	// �ړ��J�n��̌o�߃t���[����
	++frameCount;

	// 1�}�X�ړ������烊�Z�b�g���A�J�E���^���₷
	if (frameCount == TILE_SIZE / speed)
	{
		// �ڐG������
		if ((position.x == player.GetPosition().x) &&
			(position.y == player.GetPosition().y))
		{
			// �v���C���[�������_���ȁi�ړ��\�ȁj���W�Ƀ��[�v
			player.WrapPosition(map);
			activeFlg = false;

			return;
		}

		frameCount = 0;
		++walkCount;
	}

	// �ړ����̃A�j���[�V��������
	// 5�t���[�����ɃA�j���[�V����������
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
