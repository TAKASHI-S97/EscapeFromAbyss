#pragma once
#include "Object.h"

#include <Windows.h>
#include <gdiplus.h>
#pragma comment(lib, "Gdiplus.lib")

using namespace Gdiplus;

class Camera
{
public:
	Camera() = default;
	~Camera() = default;

	void SetCamera(const Player&);

	int GetCameraX() const { return x; }
	int GetCameraY() const { return y; }

private:
	const int zoom = 1;
	int x = 0;
	int y = 0;
};
