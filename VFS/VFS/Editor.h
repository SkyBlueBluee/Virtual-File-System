#pragma once

// Handles all editor stuff.
class Editor
{
public:
	void Init();
	void Update(float dt);
	void Shutdown();

private:
	void PreUpdate();
	void PostUpdate();

	void DebugDisplay();
	bool isEditorActive = false;	// Not used
	bool firstCall = true;
};