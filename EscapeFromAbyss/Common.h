#pragma once
#include <random>

constexpr int WINDOW_WIDTH = 900;		// 画面幅
constexpr int WINDOW_HEIGHT = 600;		// 画面高さ

constexpr int MAX_WIDTH = 60;			// マップ最大幅
constexpr int MAX_HEIGHT = 40;			// マップ最大高さ

constexpr int ROOM_MIN_SIZE = 4;		// 部屋最小サイズ
constexpr int ROOM_MAX_SIZE = 7;		// 部屋最大サイズ

constexpr int TILE_SIZE = 30;			// マップチップサイズ

constexpr double FPS = 30.0;			// FPS

// 方角列挙体
enum class DIRECTION
{
	UP,		// 上
	RIGHT,	// 右
	DOWN,	// 下
	LEFT	// 左
};

// ゲーム進行状態列挙体
enum class GAME_SCENE
{
	SCENE_PLAYING,	// プレイ中
	SCENE_ENDSTAGE	// ステージ終了
};

void OutputDebugFormatString(const char*, ...);		// フォーマット書式の文字列をデバッグ出力
int GetRandomInt(int, int);							// 範囲指定でランダムな整数を取得
DIRECTION GetNextRandomDirection(DIRECTION);		// 現在の方向と垂直になるランダムな方向を取得
