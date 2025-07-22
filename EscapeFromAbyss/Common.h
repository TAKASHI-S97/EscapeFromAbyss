#pragma once
#include <random>

constexpr int WINDOW_WIDTH = 900;		// ��ʕ�
constexpr int WINDOW_HEIGHT = 600;		// ��ʍ���

constexpr int MAX_WIDTH = 60;			// �}�b�v�ő啝
constexpr int MAX_HEIGHT = 40;			// �}�b�v�ő卂��

constexpr int ROOM_MIN_SIZE = 4;		// �����ŏ��T�C�Y
constexpr int ROOM_MAX_SIZE = 7;		// �����ő�T�C�Y

constexpr int TILE_SIZE = 30;			// �}�b�v�`�b�v�T�C�Y

constexpr double FPS = 30.0;			// FPS

// ���p�񋓑�
enum class DIRECTION
{
	UP,		// ��
	RIGHT,	// �E
	DOWN,	// ��
	LEFT	// ��
};

// �Q�[���i�s��ԗ񋓑�
enum class GAME_SCENE
{
	SCENE_PLAYING,	// �v���C��
	SCENE_ENDSTAGE	// �X�e�[�W�I��
};

void OutputDebugFormatString(const char*, ...);		// �t�H�[�}�b�g�����̕�������f�o�b�O�o��
int GetRandomInt(int, int);							// �͈͎w��Ń����_���Ȑ������擾
DIRECTION GetNextRandomDirection(DIRECTION);		// ���݂̕����Ɛ����ɂȂ郉���_���ȕ������擾
