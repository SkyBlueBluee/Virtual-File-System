#include "Engine.h"

int main(int, char**)
{
	// Initialize the engine
	Engine* engine = Engine::Instance();
	int error = engine->Init();
	if (error)
	{
		return 1;
	}

	// Loop while the engine is running
	while (engine->IsRunning())
	{
		engine->Run();
	}

	// Cleanup
	engine->Shutdown();

	return 0;
}