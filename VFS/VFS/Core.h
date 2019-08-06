#pragma once

class Graphics;
class VFS;

class Core
{
public:

	void Init();
	void Run();
	void Shutdown();

	auto GetGraphics()  ->Graphics* { return graphics_; }
	auto GetVFS()		->VFS*		{ return vfs_; }
	static Core* Get();

private:
	// Default ctor in private so only singleton is used.
	Core() = default;
	// Singleton object
	static Core* instance_;

	// Core variables
	bool is_running_ = false;
	bool show_cursor_ = false;
	float dt_ = 0.0f;	// time between current frame and last frame
	float prev_time_ = 0.0f;

	// Private methods
	Graphics* graphics_;
	VFS* vfs_;
	// Systems
	// std::vector<ISystem*> systems_vec;						// For looping
	// std::unordered_map<std::type_index, ISystem*> systems;	// For lookup
};
