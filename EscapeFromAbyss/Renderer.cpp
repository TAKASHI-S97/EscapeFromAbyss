#include "Renderer.h"
#include "resource.h"

/***********************************************
	�y�֐��z	�f�X�g���N�^
	�y�@�\�z	���\�[�X�̉��
	�y���l�z	std::array<Typename>�̃��\�[�X�́A
	�@�@�@�@	array�R���e�i�̉�������ŁA
	�@�@�@�@	�����I�Ƀ������t���b�V������邽�߁A
	�@�@�@�@	�蓮�ŉ������K�v�͂���܂���B
***********************************************/
GDIPlusRenderer::~GDIPlusRenderer()
{
	// �摜���\�[�X���
	delete m_playerImage;

	// �f�o�C�X�R���e�L�X�g�iDC�j�̉��
	DeleteDC(m_hdcs[static_cast<size_t>(ENUM_HDC::MAP_BUFFER_BEFORE)]);
	DeleteDC(m_hdcs[static_cast<size_t>(ENUM_HDC::BACK_BUFFER)]);
	ReleaseDC(m_hWnd, m_hdcs[static_cast<size_t>(ENUM_HDC::RENDER_TARGET)]);

	// GDI+�̏I��
	GdiplusShutdown(gdiplusToken);
}

/***********************************************
	�y�֐��z	������
	�y�@�\�z	�E�B���h�E�n���h���̎擾
	�@�@�@�@	�f�o�C�X�R���e�L�X�g�̍쐬
	�@�@�@�@	GDI+�̏�����
	�@�@�@�@	���\�[�X�摜�̓ǂݍ���
	�y���l�z	�Ȃ�
***********************************************/
bool GDIPlusRenderer::Init()
{
	// �E�B���h�E�n���h���̎擾
	m_hWnd = GetActiveWindow();
	if (!m_hWnd)
	{
		OutputDebugString(TEXT("Error: Failed to get window handle in GDIPlusRenderer."));
		return false;
	}

	// �`���DC�擾
	m_hdcs[static_cast<size_t>(ENUM_HDC::RENDER_TARGET)] = GetDC(m_hWnd);

	// ���o�b�t�@DC�쐬
	m_hdcs[static_cast<size_t>(ENUM_HDC::BACK_BUFFER)] = CreateCompatibleDC(m_hdcs[static_cast<size_t>(ENUM_HDC::RENDER_TARGET)]);
	HBITMAP hBitmapBB = CreateCompatibleBitmap(m_hdcs[static_cast<size_t>(ENUM_HDC::RENDER_TARGET)], WINDOW_WIDTH, WINDOW_HEIGHT);
	SelectObject(m_hdcs[static_cast<size_t>(ENUM_HDC::BACK_BUFFER)], hBitmapBB);
	DeleteObject(hBitmapBB);

	// GDI+�̏�����
	GdiplusStartupInput gdiplusStartupInput = {};
	if (GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr) != Status::Ok)
	{
		OutputDebugString(TEXT("Error: Failed to start GDIPlus."));
		return false;
	}

	// �I�u�W�F�N�g�z�u�O�̃}�b�vDC�쐬
	m_hdcs[static_cast<size_t>(ENUM_HDC::MAP_BUFFER_BEFORE)] = CreateCompatibleDC(m_hdcs[static_cast<size_t>(ENUM_HDC::RENDER_TARGET)]);
	HBITMAP hBitmapMAPB = CreateCompatibleBitmap(m_hdcs[static_cast<size_t>(ENUM_HDC::RENDER_TARGET)], MAX_WIDTH * TILE_SIZE, MAX_HEIGHT * TILE_SIZE);
	SelectObject(m_hdcs[static_cast<size_t>(ENUM_HDC::MAP_BUFFER_BEFORE)], hBitmapMAPB);
	DeleteObject(hBitmapMAPB);

	// �I�u�W�F�N�g�z�u��̃}�b�vDC�̍쐬
	m_hdcs[static_cast<size_t>(ENUM_HDC::MAP_BUFFER_AFTER)] = CreateCompatibleDC(m_hdcs[static_cast<size_t>(ENUM_HDC::RENDER_TARGET)]);
	HBITMAP hBitmapMAPA = CreateCompatibleBitmap(m_hdcs[static_cast<size_t>(ENUM_HDC::RENDER_TARGET)], MAX_WIDTH * TILE_SIZE, MAX_HEIGHT * TILE_SIZE);
	SelectObject(m_hdcs[static_cast<size_t>(ENUM_HDC::MAP_BUFFER_AFTER)], hBitmapMAPA);
	DeleteObject(hBitmapMAPA);

	// �I�u�W�F�N�g�̃}�b�v�`�b�v�摜��ǂݎ��
	for (size_t i = 0; i < static_cast<size_t>(OBJECT_TYPE::TYPE_MAX); ++i)
	{
		// �ʘH�͏��Ɠ����^�C�����g�p���邽�߁A�摜�̊��蓖�Ă��X�L�b�v
		if (i == static_cast<size_t>(OBJECT_TYPE::CORRIDOR)) continue;
		else m_tileImages[i] = LoadPNGImageFromResource(i + IDB_WALL);
	}

	// �v���C���[�摜�ǂݎ��
	m_playerImage = LoadPNGImageFromResource(IDB_PLAYER);

	// ���̑��L�����摜�ǂݎ��
	for (size_t i = static_cast<size_t>(Character::CHARACTER_TYPE::TYPE_GHOST); i < static_cast<size_t>(Character::CHARACTER_TYPE::TYPE_MAX); ++i)
	{
		m_characterImages[i] = LoadPNGImageFromResource(IDB_GHOST + i);
	}

	return true;
}

/***********************************************
	�y�֐��z	�}�b�v���[�h
	�y�@�\�z	�I�u�W�F�N�g�z�u�O�̃}�b�vDC���N���A���A
	�@�@�@�@	�}�b�v���ĕ`��
	�y���l�z	�K�i��1�~2�}�X�̉摜�Ȃ̂ŁA1��̍��W����`�悵�n�߂�A
	�@�@�@�@	�ʘH�͏��Ɠ����摜���g�p����B
***********************************************/
void GDIPlusRenderer::LoadMap(const std::array<std::array<OBJECT_TYPE, MAX_WIDTH>, MAX_HEIGHT>& map)
{
	// DC����`��pGraphics�쐬���A���ŃN���A
	Graphics graphics(m_hdcs[static_cast<size_t>(ENUM_HDC::MAP_BUFFER_BEFORE)]);
	graphics.Clear(Color::Black);

	// �}�b�v�`�b�v��1�}�X���`��
	for (size_t y = 0; y < MAX_HEIGHT; ++y)
		for (size_t x = 0; x <MAX_WIDTH; ++x)
		{
			switch (map[y][x])
			{
			// �K�i��1�~2�}�X�̉摜�Ȃ̂ŁA1��̍��W����`�悵�n�߂�
			case OBJECT_TYPE::STAIR:
				graphics.DrawImage(m_tileImages[static_cast<size_t>(map[y][x])], x * TILE_SIZE, (y - 1) * TILE_SIZE, TILE_SIZE, TILE_SIZE * 2);
				break;
			// �ʘH�͏��Ɠ����摜���g�p����
			case OBJECT_TYPE::CORRIDOR:
				graphics.DrawImage(m_tileImages[static_cast<size_t>(OBJECT_TYPE::FLOOR)], x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE * 2);
				break;
			// ���̑��̃}�b�v�`�b�v��1�~1�}�X�̎����̃^�C���摜�Ȃ̂ŁA�ʏ�ʂ�ɕ`��
			default:
				graphics.DrawImage(m_tileImages[static_cast<size_t>(map[y][x])], x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE);
				break;
			}
		}
}

/***********************************************
	�y�֐��z	�}�b�v���[�h
	�y�@�\�z	�X�V�����i�v���C���j
	�y���l�z	�`�揇�Ԃ͈ȉ��̒ʂ�F
	�@�@�@�@	�I�u�W�F�N�g�z�u��}�b�v�ɁA
	�@�@�@�@	�I�u�W�F�N�g�z�u�O�}�b�v��
	�@�@�@�@	�I�u�W�F�N�g�i�v���C���[�A�L����...�j�̏��ɕ`�悵�A
	�@�@�@�@	���o�b�t�@�ɃI�u�W�F�N�g�z�u��}�b�v���g���[�~���O���`��A
	�@�@�@�@	�Ō�Ƀ_�u���o�b�t�@�����O�ŕ\�o�b�t�@�֕`��
***********************************************/
void GDIPlusRenderer::Update(const Camera& camera, const Player& player, const std::vector<Character>& characters)
{
	// �}�b�v�̕`��
	BitBlt(m_hdcs[static_cast<size_t>(ENUM_HDC::MAP_BUFFER_AFTER)], 0, 0, MAX_WIDTH * TILE_SIZE, MAX_HEIGHT * TILE_SIZE,
		m_hdcs[static_cast<size_t>(ENUM_HDC::MAP_BUFFER_BEFORE)], 0, 0, SRCCOPY);

	// �v���C���[���}�b�v�ɕ`��
	Graphics graphics(m_hdcs[static_cast<size_t>(ENUM_HDC::MAP_BUFFER_AFTER)]);
	DrawPlayer(graphics, player);

	// ���̑��L�������}�b�v�ɕ`��
	DrawCharacters(graphics, characters);

	// ���o�b�t�@�̔w�i�����ŃN���A
	PatBlt(m_hdcs[static_cast<size_t>(ENUM_HDC::BACK_BUFFER)], 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, BLACKNESS);

	// �J�����ɍ��킹�Ċ��������}�b�v��ʂ𗠃o�b�t�@�ɕ`��
	BitBlt(m_hdcs[static_cast<size_t>(ENUM_HDC::BACK_BUFFER)], 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
		m_hdcs[static_cast<size_t>(ENUM_HDC::MAP_BUFFER_AFTER)], camera.GetCameraX(), camera.GetCameraY(), SRCCOPY);

	// �_�u���o�b�t�@�����O
	BitBlt(m_hdcs[static_cast<size_t>(ENUM_HDC::RENDER_TARGET)], 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
		m_hdcs[static_cast<size_t>(ENUM_HDC::BACK_BUFFER)], 0, 0, SRCCOPY);
}

/***********************************************
	�y�֐��z	�X�V�����i�X�e�[�W�I�����j
	�y�@�\�z	��ʂɃX�e�[�W�I���̃e�L�X�g��\��
	�y���l�z	�e�L�X�g�t�H���g���ꎞ�I�ɑ傫���T�C�Y���g�p
***********************************************/
void GDIPlusRenderer::Update()
{
	// �e�L�X�g�t�H���g�w��
	HFONT StringFont = CreateFont(24, 0, 0, 0, FW_NORMAL, false, false, false, 
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("Arial"));

	// ���t�H���g��ۑ�
	HFONT oldFont = static_cast<HFONT>(SelectObject(m_hdcs[static_cast<size_t>(ENUM_HDC::RENDER_TARGET)], StringFont));

	// �e�L�X�g�o�͔͈͎w��
	RECT StringRect = { 100, 225, 800, 375 };

	// �o�̓e�L�X�g����
	std::string ContinueString = "STAGE END!\nENTER: Continue \tESC: End";

	// �e�L�X�g�o��
	DrawText(m_hdcs[static_cast<size_t>(ENUM_HDC::RENDER_TARGET)], ContinueString.c_str(), ContinueString.length(), &StringRect, DT_CENTER);

	// �t�H���g�ݒ�߂�
	SelectObject(m_hdcs[static_cast<size_t>(ENUM_HDC::RENDER_TARGET)], oldFont);

	// ���\�[�X���
	DeleteObject(StringFont);
}

/***********************************************
	�y�֐��z	�v���C���[�̕`��
	�y�@�\�z	�v���C���[�̕`��
	�y���l�z	�v���C���[�}�b�v�`�b�v��
	�@�@�@�@	�S�s���p�^�[���S�����̉摜��1���ɓ�������Ă��邽�߁A
	�@�@�@�@	�`��̊J�n�ʒu�iX,Y�j���w�肵�ĕ`�悷��K�v������܂��B
***********************************************/
void GDIPlusRenderer::DrawPlayer(Graphics& graphics, const Player& player)
{
	// �v���C���[�摜�̕`��擪X���W�̐ݒ�
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

	// �v���C���[�摜�̕`��擪Y���W�̐ݒ�
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

	// �v���C���[���}�b�v��̕`��͈͎w��
	Rect DrawRect = { player.GetPosition().x, player.GetPosition().y, TILE_SIZE, TILE_SIZE };

	// �}�b�v��Ŏw�肵���͈͓��Ɏw�肵��X�EY���W����`��
	graphics.DrawImage(
		m_playerImage, 
		DrawRect, 
		startX, startY, 
		player.IMAGE_SIZE, player.IMAGE_SIZE,
		UnitPixel
	);
}

/***********************************************
	�y�֐��z	���̑��L�����̕`��
	�y�@�\�z	���̑��L�����̕`��
	�y���l�z	�v���C���[�Ɠ��l�A�`��̊J�n�ʒu�iX,Y�j���w�肵�ĕ`�悷��K�v������܂��B
***********************************************/
void GDIPlusRenderer::DrawCharacters(Graphics& graphics, const std::vector<Character>& characters)
{
	// �S�L���������[�v
	for (auto& character : characters)
	{
		// ���ݒ��̃L�����̂ݕ`��
		if (character.activeFlg)
		{
			// �L�����N�^�[�摜�̕`��擪X���W�̐ݒ�
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

			// �L�����N�^�[�摜�̕`��擪Y���W�̐ݒ�
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

			// �L�����N�^�[���}�b�v��̕`��͈͎w��
			Rect DrawRect = { character.GetPosition().x, character.GetPosition().y, TILE_SIZE, TILE_SIZE };

			// �}�b�v��Ŏw�肵���͈͓��Ɏw�肵��X�EY���W����`��
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
	�y�֐��z	�摜�擾�p�֐�
	�y�@�\�z	�X�g���[������摜���擾
	�y���l�z	�Ȃ�
***********************************************/
Bitmap* GDIPlusRenderer::LoadPNGImageFromResource(UINT uID)
{
	// ���\�[�X�̌���
	HRSRC hRsrc = FindResource(nullptr, MAKEINTRESOURCE(uID), TEXT("PNG"));
	if (!hRsrc)
	{
		OutputDebugString(TEXT("Error: Failed to find resource."));
		return nullptr;
	}

	// ���\�[�X�T�C�Y�擾
	DWORD resourceSize = SizeofResource(nullptr, hRsrc);
	if (!resourceSize)
	{
		OutputDebugString(TEXT("Error: Failed to get the size of resource."));
		return nullptr;
	}

	// ���\�[�X�̃��b�N
	void* pRsrc = LockResource(LoadResource(nullptr, hRsrc));
	if (!pRsrc)
	{
		OutputDebugString(TEXT("Error: Failed to lock resource."));
		return nullptr;
	}

	// �O���[�o���o�b�t�@���m��
	HGLOBAL hGlobalBuffer = GlobalAlloc(GMEM_MOVEABLE, resourceSize);
	if (!hGlobalBuffer)
	{
		GlobalFree(hGlobalBuffer);
		OutputDebugString(TEXT("Error: Failed to capture the size of resource in global."));
		return nullptr;
	}

	// �O���[�o���o�b�t�@�̃��b�N
	void* pGlobalBuffer = GlobalLock(hGlobalBuffer);
	if (!pGlobalBuffer)
	{
		GlobalUnlock(hGlobalBuffer);
		GlobalFree(hGlobalBuffer);
		OutputDebugString(TEXT("Error: Failed to lock the global buffer."));
		return nullptr;
	}

	// ���\�[�X���O���[�o���o�b�t�@�փR�s�[
	CopyMemory(pGlobalBuffer, pRsrc, resourceSize);

	// �O���[�o���o�b�t�@����IStream���쐬
	IStream* pIStream = nullptr;
	HRESULT hr = CreateStreamOnHGlobal(hGlobalBuffer, false, &pIStream);
	if (hr != S_OK)
	{
		GlobalUnlock(hGlobalBuffer);
		GlobalFree(hGlobalBuffer);
		OutputDebugString(TEXT("Error: Failed to create IStream."));
		return nullptr;
	}

	// Bitmap*�쐬
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

	// ���\�[�X�̉��
	pIStream->Release();
	GlobalUnlock(hGlobalBuffer);
	GlobalFree(hGlobalBuffer);

	return pBitmap;
}
