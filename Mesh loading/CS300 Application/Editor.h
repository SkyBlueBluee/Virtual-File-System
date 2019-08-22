#pragma once
#include "ISystem.h"

// Handles all editor stuff.
class Editor : public  ISystem
{
public:
	int Init() override;
	int Update(float dt) override;
	void Shutdown() override;

private:
	void PreUpdate();
	void PostUpdate();

	void DebugDisplay();
	bool isEditorActive = false;	// Not used
	bool firstCall = true;
};