#include "SceneManager.h"
#include "BasiceScene.h"
#include "CS300_A1.h"
int SceneManager::Init()
{
	
	scenes.reserve(4); // at most 4 scenes before growing
	AddScene<BasicScene>("Basic");
	AddScene<CS300_A1>("Basic");

	activeSceneID = 1;
	return 0;
}
