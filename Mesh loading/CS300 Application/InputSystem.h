#pragma once
#include "ISystem.h"
class Camera;

// System for handling application inputs.
class InputSystem : public ISystem
{
public:
	int Init() override;
	int Update(float dt) override;
	bool IsCursorShown() const;
private:
	
};