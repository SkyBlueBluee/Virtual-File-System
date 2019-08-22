#pragma once

// Abstract class that all systems should inherit from if they want to be added into the Engine.
class ISystem
{
public:
	virtual int Init() { return 0; }
	virtual int Update(float dt = 1.0f / 60.0f) { return 0; }
	virtual void Shutdown() {}
	virtual ~ISystem() {}
};