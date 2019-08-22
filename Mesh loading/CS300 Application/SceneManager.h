#pragma once
#include <vector>
#include "ISystem.h"
#include "IScene.h"

// Currently not heavily used. This was created as I had thought there might be a need to load/reload scenes in runtime. 
class SceneManager : public ISystem
{
public:
	int Init() override;

	// Add scene functions
	template<typename Scene, typename = std::enable_if_t<std::is_base_of_v<IScene, Scene>>>
	void AddScene(const std::string& name)
	{
		Scene* scene = new Scene{};
		scene->sceneName = name;
		scene->Init();
		scenes.push_back(scene);
	}

	std::vector<IScene*>& GetScenes() { return scenes; }
	IScene* GetActiveScene() { return scenes[activeSceneID]; }

private:
	unsigned activeSceneID = 0;
	std::vector<IScene*> scenes;
};