#pragma once
// Include standard headers
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <vector>
#include <unordered_map>
#include <typeinfo>
#include <typeindex>

#include "ISystem.h"

class GLFWWindow;
struct Model;
class Graphics;
class Camera;

class Engine
{
public:
	// Delete constructors to force use of singleton
	Engine(const Engine&) = delete;
	Engine(const Engine&&) = delete;

	static Engine* Instance() { return instance; }

	template<typename Sys, typename = std::enable_if_t<std::is_base_of_v<ISystem, Sys>>>
	Sys* GetSystem()
	{
		std::type_index type = static_cast<std::type_index>(typeid(Sys));
		auto result = systems.find(type);

		// Safety check
		if (result == systems.end())
			return nullptr;

		return (Sys*)result->second;
	}

	template<typename Sys, typename = std::enable_if_t<std::is_base_of_v<ISystem, Sys>>>
	const Sys* GetSystem() const
	{
		std::type_index type = static_cast<std::type_index>(typeid(Sys));
		auto result = systems.find(type);

		// Safety check
		if (result == systems.end())
			return nullptr;

		return (Sys*)result->second;
	}

	template<typename Sys, typename = std::enable_if_t<std::is_base_of_v<ISystem, Sys>>>
	void AddSystem()
	{
		Sys* system = new Sys{};
		systems_vec.push_back(system);
		std::type_index type = static_cast<std::type_index>(typeid(Sys));

		// Safety check
		auto result = systems.find(type);
		if (result != systems.end())
			return;
		
		systems.emplace(type, system);
	}

	bool IsRunning() const { return isRunning; }
	
	int Init();
	void Run();
	void Shutdown();

private:

	// Default ctor in private so only singleton is used.
	Engine() = default;
	// Singleton object
	static Engine* instance;

	// Engine variables
	bool isRunning = false;
	bool showCursor = false;
	float deltaTime = 0.0f;	// time between current frame and last frame
	float lastFrame = 0.0f;

	// Private methods

	// Systems
	std::vector<ISystem*> systems_vec;						// For looping
	std::unordered_map<std::type_index, ISystem*> systems;	// For lookup
};
