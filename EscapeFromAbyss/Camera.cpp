#include "Camera.h"

void Camera::SetCamera(const Player& player)
{
	int centerX = player.GetPosition().x + TILE_SIZE / 2;
	int centerY = player.GetPosition().y + TILE_SIZE / 2;

	x = centerX - WINDOW_WIDTH / 2;
	y = centerY - WINDOW_HEIGHT / 2;
}
