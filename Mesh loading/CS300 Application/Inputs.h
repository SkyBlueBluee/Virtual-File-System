#pragma once
#include "ISystem.h"


struct InputSystem : public ISystem
{
	int Update(float dt) override;

};