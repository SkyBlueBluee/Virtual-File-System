#pragma once
#include "VFS.h"
#include "imgui.h"

struct TestSys
{
	void init();
	void update(float dt);
	void shutdown();

	VFS* vfs = nullptr;

};