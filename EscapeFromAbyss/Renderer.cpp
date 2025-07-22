#include "Renderer.h"
#include "resource.h"

/***********************************************
	【関数】	デストラクタ
	【機能】	リソースの解放
	【備考】	std::array<Typename>のリソースは、
	　　　　	arrayコンテナの解放処理で、
	　　　　	自動的にメモリフラッシュされるため、
	　　　　	手動で解放する必要はありません。
***********************************************/
GDIPlusRenderer::~GDIPlusRenderer()
{
	// 画像リソース解放
	delete m_playerImage;

	// デバイスコンテキスト（DC）の解放
	DeleteDC(m_hdcs[static_cast<size_t>(ENUM_HDC::MAP_BUFFER_BEFORE)]);
	DeleteDC(m_hdcs[static_cast<size_t>(ENUM_HDC::BACK_BUFFER)]);
	ReleaseDC(m_hWnd, m_hdcs[static_cast<size_t>(ENUM_HDC::RENDER_TARGET)]);

	// GDI+の終了
	GdiplusShutdown(gdiplusToken);
}

/***********************************************
	【関数】	初期化
	【機能】	ウィンドウハンドルの取得
	　　　　	デバイスコンテキストの作成
	　　　　	GDI+の初期化
	　　　　	リソース画像の読み込み
	【備考】	なし
***********************************************/
bool GDIPlusRenderer::Init()
{
	// ウィンドウハンドルの取得
	m_hWnd = GetActiveWindow();
	if (!m_hWnd)
	{
		OutputDebugString(TEXT("Error: Failed to get window handle in GDIPlusRenderer."));
		return false;
	}

	// 描画先DC取得
	m_hdcs[static_cast<size_t>(ENUM_HDC::RENDER_TARGET)] = GetDC(m_hWnd);

	// 裏バッファDC作成
	m_hdcs[static_cast<size_t>(ENUM_HDC::BACK_BUFFER)] = CreateCompatibleDC(m_hdcs[static_cast<size_t>(ENUM_HDC::RENDER_TARGET)]);
	HBITMAP hBitmapBB = CreateCompatibleBitmap(m_hdcs[static_cast<size_t>(ENUM_HDC::RENDER_TARGET)], WINDOW_WIDTH, WINDOW_HEIGHT);
	SelectObject(m_hdcs[static_cast<size_t>(ENUM_HDC::BACK_BUFFER)], hBitmapBB);
	DeleteObject(hBitmapBB);

	// GDI+の初期化
	GdiplusStartupInput gdiplusStartupInput = {};
	if (GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr) != Status::Ok)
	{
		OutputDebugString(TEXT("Error: Failed to start GDIPlus."));
		return false;
	}

	// オブジェクト配置前のマップDC作成
	m_hdcs[static_cast<size_t>(ENUM_HDC::MAP_BUFFER_BEFORE)] = CreateCompatibleDC(m_hdcs[static_cast<size_t>(ENUM_HDC::RENDER_TARGET)]);
	HBITMAP hBitmapMAPB = CreateCompatibleBitmap(m_hdcs[static_cast<size_t>(ENUM_HDC::RENDER_TARGET)], MAX_WIDTH * TILE_SIZE, MAX_HEIGHT * TILE_SIZE);
	SelectObject(m_hdcs[static_cast<size_t>(ENUM_HDC::MAP_BUFFER_BEFORE)], hBitmapMAPB);
	DeleteObject(hBitmapMAPB);

	// オブジェクト配置後のマップDCの作成
	m_hdcs[static_cast<size_t>(ENUM_HDC::MAP_BUFFER_AFTER)] = CreateCompatibleDC(m_hdcs[static_cast<size_t>(ENUM_HDC::RENDER_TARGET)]);
	HBITMAP hBitmapMAPA = CreateCompatibleBitmap(m_hdcs[static_cast<size_t>(ENUM_HDC::RENDER_TARGET)], MAX_WIDTH * TILE_SIZE, MAX_HEIGHT * TILE_SIZE);
	SelectObject(m_hdcs[static_cast<size_t>(ENUM_HDC::MAP_BUFFER_AFTER)], hBitmapMAPA);
	DeleteObject(hBitmapMAPA);

	// オブジェクトのマップチップ画像を読み取り
	for (size_t i = 0; i < static_cast<size_t>(OBJECT_TYPE::TYPE_MAX); ++i)
	{
		// 通路は床と同じタイルを使用するため、画像の割り当てをスキップ
		if (i == static_cast<size_t>(OBJECT_TYPE::CORRIDOR)) continue;
		else m_tileImages[i] = LoadPNGImageFromResource(i + IDB_WALL);
	}

	// プレイヤー画像読み取り
	m_playerImage = LoadPNGImageFromResource(IDB_PLAYER);

	// その他キャラ画像読み取り
	for (size_t i = static_cast<size_t>(Character::CHARACTER_TYPE::TYPE_GHOST); i < static_cast<size_t>(Character::CHARACTER_TYPE::TYPE_MAX); ++i)
	{
		m_characterImages[i] = LoadPNGImageFromResource(IDB_GHOST + i);
	}

	return true;
}

/***********************************************
	【関数】	マップロード
	【機能】	オブジェクト配置前のマップDCをクリアし、
	　　　　	マップを再描画
	【備考】	階段は1×2マスの画像なので、1つ上の座標から描画し始める、
	　　　　	通路は床と同じ画像を使用する。
***********************************************/
void GDIPlusRenderer::LoadMap(const std::array<std::array<OBJECT_TYPE, MAX_WIDTH>, MAX_HEIGHT>& map)
{
	// DCから描画用Graphics作成し、黒でクリア
	Graphics graphics(m_hdcs[static_cast<size_t>(ENUM_HDC::MAP_BUFFER_BEFORE)]);
	graphics.Clear(Color::Black);

	// マップチップを1マスずつ描画
	for (size_t y = 0; y < MAX_HEIGHT; ++y)
		for (size_t x = 0; x <MAX_WIDTH; ++x)
		{
			switch (map[y][x])
			{
			// 階段は1×2マスの画像なので、1つ上の座標から描画し始める
			case OBJECT_TYPE::STAIR:
				graphics.DrawImage(m_tileImages[static_cast<size_t>(map[y][x])], x * TILE_SIZE, (y - 1) * TILE_SIZE, TILE_SIZE, TILE_SIZE * 2);
				break;
			// 通路は床と同じ画像を使用する
			case OBJECT_TYPE::CORRIDOR:
				graphics.DrawImage(m_tileImages[static_cast<size_t>(OBJECT_TYPE::FLOOR)], x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE * 2);
				break;
			// その他のマップチップは1×1マスの自分のタイル画像なので、通常通りに描画
			default:
				graphics.DrawImage(m_tileImages[static_cast<size_t>(map[y][x])], x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE);
				break;
			}
		}
}

/***********************************************
	【関数】	マップロード
	【機能】	更新処理（プレイ時）
	【備考】	描画順番は以下の通り：
	　　　　	オブジェクト配置後マップに、
	　　　　	オブジェクト配置前マップ⇒
	　　　　	オブジェクト（プレイヤー、キャラ...）の順に描画し、
	　　　　	裏バッファにオブジェクト配置後マップをトリーミングし描画、
	　　　　	最後にダブルバッファリングで表バッファへ描画
***********************************************/
void GDIPlusRenderer::Update(const Camera& camera, const Player& player, const std::vector<Character>& characters)
{
	// マップの描画
	BitBlt(m_hdcs[static_cast<size_t>(ENUM_HDC::MAP_BUFFER_AFTER)], 0, 0, MAX_WIDTH * TILE_SIZE, MAX_HEIGHT * TILE_SIZE,
		m_hdcs[static_cast<size_t>(ENUM_HDC::MAP_BUFFER_BEFORE)], 0, 0, SRCCOPY);

	// プレイヤーをマップに描画
	Graphics graphics(m_hdcs[static_cast<size_t>(ENUM_HDC::MAP_BUFFER_AFTER)]);
	DrawPlayer(graphics, player);

	// その他キャラをマップに描画
	DrawCharacters(graphics, characters);

	// 裏バッファの背景を黒でクリア
	PatBlt(m_hdcs[static_cast<size_t>(ENUM_HDC::BACK_BUFFER)], 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, BLACKNESS);

	// カメラに合わせて完成したマップ画面を裏バッファに描画
	BitBlt(m_hdcs[static_cast<size_t>(ENUM_HDC::BACK_BUFFER)], 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
		m_hdcs[static_cast<size_t>(ENUM_HDC::MAP_BUFFER_AFTER)], camera.GetCameraX(), camera.GetCameraY(), SRCCOPY);

	// ダブルバッファリング
	BitBlt(m_hdcs[static_cast<size_t>(ENUM_HDC::RENDER_TARGET)], 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
		m_hdcs[static_cast<size_t>(ENUM_HDC::BACK_BUFFER)], 0, 0, SRCCOPY);
}

/***********************************************
	【関数】	更新処理（ステージ終了時）
	【機能】	画面にステージ終了のテキストを表示
	【備考】	テキストフォントを一時的に大きいサイズを使用
***********************************************/
void GDIPlusRenderer::Update()
{
	// テキストフォント指定
	HFONT StringFont = CreateFont(24, 0, 0, 0, FW_NORMAL, false, false, false, 
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("Arial"));

	// 元フォントを保存
	HFONT oldFont = static_cast<HFONT>(SelectObject(m_hdcs[static_cast<size_t>(ENUM_HDC::RENDER_TARGET)], StringFont));

	// テキスト出力範囲指定
	RECT StringRect = { 100, 225, 800, 375 };

	// 出力テキスト書式
	std::string ContinueString = "STAGE END!\nENTER: Continue \tESC: End";

	// テキスト出力
	DrawText(m_hdcs[static_cast<size_t>(ENUM_HDC::RENDER_TARGET)], ContinueString.c_str(), ContinueString.length(), &StringRect, DT_CENTER);

	// フォント設定戻し
	SelectObject(m_hdcs[static_cast<size_t>(ENUM_HDC::RENDER_TARGET)], oldFont);

	// リソース解放
	DeleteObject(StringFont);
}

/***********************************************
	【関数】	プレイヤーの描画
	【機能】	プレイヤーの描画
	【備考】	プレイヤーマップチップは
	　　　　	全行動パターン全向きの画像が1枚に統合されているため、
	　　　　	描画の開始位置（X,Y）を指定して描画する必要があります。
***********************************************/
void GDIPlusRenderer::DrawPlayer(Graphics& graphics, const Player& player)
{
	// プレイヤー画像の描画先頭X座標の設定
	int startX = 0;
	switch (player.GetMotionStatus())
	{
	case Player::MOTION_STATUS::WALK1:
		startX = 0;
		break;

	case Player::MOTION_STATUS::IDOL:
		startX = 32;
		break;

	case Player::MOTION_STATUS::WALK2:
		startX = 64;
		break;
	}

	// プレイヤー画像の描画先頭Y座標の設定
	int startY = 0;
	switch (player.GetDirectionConst())
	{
	case DIRECTION::DOWN:
		startY = 0;
		break;
		
	case DIRECTION::LEFT:
		startY = 32;
		break;

	case DIRECTION::RIGHT:
		startY = 64;
		break;

	case DIRECTION::UP:
		startY = 96;
		break;
	}

	// プレイヤーがマップ上の描画範囲指定
	Rect DrawRect = { player.GetPosition().x, player.GetPosition().y, TILE_SIZE, TILE_SIZE };

	// マップ上で指定した範囲内に指定したX・Y座標から描画
	graphics.DrawImage(
		m_playerImage, 
		DrawRect, 
		startX, startY, 
		player.IMAGE_SIZE, player.IMAGE_SIZE,
		UnitPixel
	);
}

/***********************************************
	【関数】	その他キャラの描画
	【機能】	その他キャラの描画
	【備考】	プレイヤーと同様、描画の開始位置（X,Y）を指定して描画する必要があります。
***********************************************/
void GDIPlusRenderer::DrawCharacters(Graphics& graphics, const std::vector<Character>& characters)
{
	// 全キャラをループ
	for (auto& character : characters)
	{
		// 存在中のキャラのみ描画
		if (character.activeFlg)
		{
			// キャラクター画像の描画先頭X座標の設定
			int startX = 0;
			switch (character.GetMotionStatus())
			{
			case Player::MOTION_STATUS::WALK1:
				startX = 0;
				break;

			case Player::MOTION_STATUS::IDOL:
				startX = 32;
				break;

			case Player::MOTION_STATUS::WALK2:
				startX = 64;
				break;
			}

			// キャラクター画像の描画先頭Y座標の設定
			int startY = 0;
			switch (character.GetDirectionConst())
			{
			case DIRECTION::DOWN:
				startY = 0;
				break;

			case DIRECTION::LEFT:
				startY = 32;
				break;

			case DIRECTION::RIGHT:
				startY = 64;
				break;

			case DIRECTION::UP:
				startY = 96;
				break;
			}

			// キャラクターがマップ上の描画範囲指定
			Rect DrawRect = { character.GetPosition().x, character.GetPosition().y, TILE_SIZE, TILE_SIZE };

			// マップ上で指定した範囲内に指定したX・Y座標から描画
			graphics.DrawImage(
				m_characterImages[static_cast<size_t>(character.GetCharacterType())],
				DrawRect,
				startX, startY,
				character.IMAGE_SIZE, character.IMAGE_SIZE,
				UnitPixel
			);
		}
	}
}

/***********************************************
	【関数】	画像取得用関数
	【機能】	ストリームから画像を取得
	【備考】	なし
***********************************************/
Bitmap* GDIPlusRenderer::LoadPNGImageFromResource(UINT uID)
{
	// リソースの検索
	HRSRC hRsrc = FindResource(nullptr, MAKEINTRESOURCE(uID), TEXT("PNG"));
	if (!hRsrc)
	{
		OutputDebugString(TEXT("Error: Failed to find resource."));
		return nullptr;
	}

	// リソースサイズ取得
	DWORD resourceSize = SizeofResource(nullptr, hRsrc);
	if (!resourceSize)
	{
		OutputDebugString(TEXT("Error: Failed to get the size of resource."));
		return nullptr;
	}

	// リソースのロック
	void* pRsrc = LockResource(LoadResource(nullptr, hRsrc));
	if (!pRsrc)
	{
		OutputDebugString(TEXT("Error: Failed to lock resource."));
		return nullptr;
	}

	// グローバルバッファを確保
	HGLOBAL hGlobalBuffer = GlobalAlloc(GMEM_MOVEABLE, resourceSize);
	if (!hGlobalBuffer)
	{
		GlobalFree(hGlobalBuffer);
		OutputDebugString(TEXT("Error: Failed to capture the size of resource in global."));
		return nullptr;
	}

	// グローバルバッファのロック
	void* pGlobalBuffer = GlobalLock(hGlobalBuffer);
	if (!pGlobalBuffer)
	{
		GlobalUnlock(hGlobalBuffer);
		GlobalFree(hGlobalBuffer);
		OutputDebugString(TEXT("Error: Failed to lock the global buffer."));
		return nullptr;
	}

	// リソースをグローバルバッファへコピー
	CopyMemory(pGlobalBuffer, pRsrc, resourceSize);

	// グローバルバッファからIStreamを作成
	IStream* pIStream = nullptr;
	HRESULT hr = CreateStreamOnHGlobal(hGlobalBuffer, false, &pIStream);
	if (hr != S_OK)
	{
		GlobalUnlock(hGlobalBuffer);
		GlobalFree(hGlobalBuffer);
		OutputDebugString(TEXT("Error: Failed to create IStream."));
		return nullptr;
	}

	// Bitmap*作成
	Bitmap* pBitmap = Bitmap::FromStream(pIStream);
	if ((pBitmap == nullptr) || (pBitmap->GetLastStatus() != Ok))
	{
		delete pBitmap;
		pBitmap = nullptr;
		pIStream->Release();
		GlobalUnlock(hGlobalBuffer);
		GlobalFree(hGlobalBuffer);
		OutputDebugString(TEXT("Error: Failed to create Bitmap from IStream."));
		return nullptr;
	}

	// リソースの解放
	pIStream->Release();
	GlobalUnlock(hGlobalBuffer);
	GlobalFree(hGlobalBuffer);

	return pBitmap;
}
