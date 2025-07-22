#pragma once
#include "Object.h"

class Controller
{
public:
	Controller() = default;
	~Controller() = default;

	void Update(Player&);
	void Update(bool&, bool&);
};
