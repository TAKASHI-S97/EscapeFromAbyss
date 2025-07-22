#pragma once
#include "Common.h"

#include <array>

enum class OBJECT_TYPE
{
	WALL,
	FLOOR,
	CORRIDOR,
	STAIR,
	TYPE_MAX
};

class Object
{
public:
	Object(OBJECT_TYPE inType, int inX, int inY) 
		: type(inType), x(inX), y(inY) { }
	~Object() = default;


	OBJECT_TYPE type;
	int x;
	int y;
};

class Player
{
public:
	enum class MOTION_STATUS
	{
		IDOL,
		WALK1,
		WALK2
	};

	struct Position
	{
		int x;
		int y;
	};

	Player() = default;
	virtual ~Player() = default;

	const int IMAGE_SIZE = 32;

	void InitPlayer(int inX, int inY) { position.x = inX; position.y = inY; }
	void Update(const std::array<std::array<OBJECT_TYPE, MAX_WIDTH>, MAX_HEIGHT>&);

	void WrapPosition(const std::array<std::array<OBJECT_TYPE, MAX_WIDTH>, MAX_HEIGHT>&);

	MOTION_STATUS GetMotionStatus() const { return status; }
	Position GetPosition() const { return position; }
	DIRECTION GetDirectionConst() const { return dir; }

	DIRECTION& GetDirection() { return dir; }
	bool& GetPlayerMovingFlg() { return isMoving; }
	bool& GetSlowFlg() { return stopFlg; }

protected:
	Position position = { };
	MOTION_STATUS status = MOTION_STATUS::IDOL;
	DIRECTION dir = DIRECTION::DOWN;
	int motionCode = 0;
	const int speed = TILE_SIZE * 3  / static_cast<int>(FPS);	// 移動スピードは 3マス/1秒 ⇒ 3マス分のピクセル数/1秒間のフレーム数
	int frameCount = 0;

private:
	bool isMoving = false;
	bool stopFlg = false;
	int stopFrameCount = 0;

	void PlayerMove(const std::array<std::array<OBJECT_TYPE, MAX_WIDTH>, MAX_HEIGHT>&);
};

class Character : public Player
{
public:
	enum class CHARACTER_TYPE
	{
		TYPE_GHOST,
		TYPE_ALIAN,
		TYPE_MAX
	};

	Character() = default;
	~Character() override = default;

	void InitCharacter(CHARACTER_TYPE inType, int inX, int inY) { type = inType; position.x = inX; position.y = inY; }
	void Update(const std::array<std::array<OBJECT_TYPE, MAX_WIDTH>, MAX_HEIGHT>&, Player&);

	CHARACTER_TYPE GetCharacterType() const { return type; }
	
	bool activeFlg = true;

private:
	CHARACTER_TYPE type = CHARACTER_TYPE::TYPE_MAX;
	int walkCount = 0;

	bool SearchPlayer(const Player&);
	void ChasePlayer(Player&);
	void RandomWalk(const std::array<std::array<OBJECT_TYPE, MAX_WIDTH>, MAX_HEIGHT>&, Player&);
};
