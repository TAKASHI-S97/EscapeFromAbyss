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
	GDIPlusRenderer() = default;				// �R���X�g���N�^�i�f�t�H���g�g�p�j
	~GDIPlusRenderer();							// �f�X�g���N�^

	bool Init(void);																				// ������
	void LoadMap(const std::array<std::array<OBJECT_TYPE, MAX_WIDTH>, MAX_HEIGHT>&);				// �}�b�v���[�h
	void Update(const Camera&, const Player&, const std::vector<Character>&);						// �X�V�����i�v���C���j
	void Update(void);																				// �X�V�����i�X�e�[�W�I�����j

	/* �A�N�Z�b�T�\�֐� */
	HDC GetRenderTargetDC() const { return m_hdcs[static_cast<size_t>(ENUM_HDC::RENDER_TARGET)]; }	// �`���f�o�C�X�R���e�L�X�g�̎擾

private:
	enum class ENUM_HDC		// HDC�񋓑�
	{
		RENDER_TARGET,		// �`���
		BACK_BUFFER,		// �`�无�o�b�t�@
		MAP_BUFFER_BEFORE,	// �I�u�W�F�N�g�z�u�O�̃}�b�v
		MAP_BUFFER_AFTER,	// �I�u�W�F�N�g�z�u��̃}�b�v
		TYPE_MAX			// ���v�񋓐�
	};

	ULONG_PTR gdiplusToken = 0;																				// GDI+�̃g�[�N��
	HWND m_hWnd = 0;																						// �E�B���h�E�n���h��
	std::array<HDC, static_cast<size_t>(ENUM_HDC::TYPE_MAX)> m_hdcs = { };									// �S�f�o�C�X�R���e�L�X�g�̔z��
	std::array<Bitmap*, static_cast<size_t>(OBJECT_TYPE::TYPE_MAX)> m_tileImages = { };						// �S�}�b�v�`�b�v�摜�̔z��
	Bitmap* m_playerImage = nullptr;																		// �v���C���[�摜
	std::array<Bitmap*, static_cast<size_t>(Character::CHARACTER_TYPE::TYPE_MAX)> m_characterImages = { };	// ���̑��L�����̉摜�̔z��

	void DrawPlayer(Graphics&, const Player&);						// �v���C���[�̕`��
	void DrawCharacters(Graphics&, const std::vector<Character>&);	// ���̑��L�����̕`��

	Bitmap* LoadPNGImageFromResource(UINT);							// �摜�擾�p�֐�
};
