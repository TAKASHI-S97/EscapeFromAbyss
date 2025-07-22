#pragma once
#include "Object.h"
#include "Camera.h"

#include <Windows.h>
#include <gdiplus.h>
#include <array>
#include <vector>

#pragma comment(lib, "Gdiplus.lib")

using namespace Gdiplus;

class GDIPlusRenderer
{
public:
	GDIPlusRenderer() = default;				// コンストラクタ（デフォルト使用）
	~GDIPlusRenderer();							// デストラクタ

	bool Init(void);																				// 初期化
	void LoadMap(const std::array<std::array<OBJECT_TYPE, MAX_WIDTH>, MAX_HEIGHT>&);				// マップロード
	void Update(const Camera&, const Player&, const std::vector<Character>&);						// 更新処理（プレイ時）
	void Update(void);																				// 更新処理（ステージ終了時）

	/* アクセッサ―関数 */
	HDC GetRenderTargetDC() const { return m_hdcs[static_cast<size_t>(ENUM_HDC::RENDER_TARGET)]; }	// 描画先デバイスコンテキストの取得

private:
	enum class ENUM_HDC		// HDC列挙体
	{
		RENDER_TARGET,		// 描画先
		BACK_BUFFER,		// 描画裏バッファ
		MAP_BUFFER_BEFORE,	// オブジェクト配置前のマップ
		MAP_BUFFER_AFTER,	// オブジェクト配置後のマップ
		TYPE_MAX			// 合計列挙数
	};

	ULONG_PTR gdiplusToken = 0;																				// GDI+のトークン
	HWND m_hWnd = 0;																						// ウィンドウハンドル
	std::array<HDC, static_cast<size_t>(ENUM_HDC::TYPE_MAX)> m_hdcs = { };									// 全デバイスコンテキストの配列
	std::array<Bitmap*, static_cast<size_t>(OBJECT_TYPE::TYPE_MAX)> m_tileImages = { };						// 全マップチップ画像の配列
	Bitmap* m_playerImage = nullptr;																		// プレイヤー画像
	std::array<Bitmap*, static_cast<size_t>(Character::CHARACTER_TYPE::TYPE_MAX)> m_characterImages = { };	// その他キャラの画像の配列

	void DrawPlayer(Graphics&, const Player&);						// プレイヤーの描画
	void DrawCharacters(Graphics&, const std::vector<Character>&);	// その他キャラの描画

	Bitmap* LoadPNGImageFromResource(UINT);							// 画像取得用関数
};
