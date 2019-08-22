#include "ModelManager.h"
#include "Icosphere.h"
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

// The path to the models folder
static const std::string MODEL_PATH = "./resource/models/";

ModelManager::~ModelManager()
{
	// Cleanup
	for (auto* elem : modelsVec)
	{
		delete elem;
	}
}

void ModelManager::Init()
{
	// Directly add an icosphere model
	Icosphere* sphere = new Icosphere{};
	sphere->name = "icosphere";
	// sphere->isLoaded = true;
	AddModel(sphere);

	// For every obj file found inside the models folder, we add it to the map.
	// NOTE: We have not actually loaded the models yet. Models are loaded when the user first selects it.
	std::string ext(".obj");
	for (auto& p : fs::directory_iterator(MODEL_PATH))
	{
		if (p.path().extension() == ext)
		{
			Model* model = new Model{ fs::path{ p }.string() };
			AddModel(model);
		}
	}

	ext = ".fbx";
	for (auto& p : fs::directory_iterator(MODEL_PATH))
	{
		if (p.path().extension() == ext)
		{
			Model* model = new Model{ fs::path{ p }.string() };
			AddModel(model);
		}
	}
}

Model * ModelManager::GetModel(const std::string & model)
{
	// Finds a model inside the map. If it is there, check if it has been loaded already. If not, load it.
	auto result = models.find(model);
	if (result != models.end())
	{
		if (result->second->isLoaded == false)
		{
			int loadResult = result->second->LoadModel(result->second->path);
			assert(loadResult == 0);
			result->second->isLoaded = true;
		}
		return result->second;
	}
	return nullptr;
}

int ModelManager::LoadModel(const std::string & model)
{
	return !GetModel(model);
}

void ModelManager::AddModel(Model * model)
{
	// Adds a model into the map. Model names cannot be the same.
	auto result = models.find(model->name);
	if (result == models.end())
	{
		models.emplace(model->name, model);
		modelsVec.push_back(model);
	}
}
