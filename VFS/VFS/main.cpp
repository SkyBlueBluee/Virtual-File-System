#include "Core.h"
int main()
{
	// Initialize the engine
	Core* engine = Core::Get();
	engine->Init();
	
	// Loop while the engine is running
	while (true)
	{
		engine->Run();
	}

	// Cleanup
	engine->Shutdown();

	return 0;

	return 0;
}