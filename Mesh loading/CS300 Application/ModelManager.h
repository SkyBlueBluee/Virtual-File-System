#pragma once
#include "Model.h"

class ModelManager
{
public:
	ModelManager() = default;
	~ModelManager();

	// Does not load all the models from the objs yet. 
	// Simply finds all the obj files and construct the map of them first.
	void Init();

	// Getters
	// All model names should disregard .obj and are case sensitive.
	// Eg:	Model* model = GetModel("cube");		CORRECT
	//		Model* model = GetModel("cube.obj");	WRONG
	const std::vector<Model*>& GetAllModels() const { return modelsVec; }

	// This function loads the model if it was not already loaded. 
	// If the model was loaded before, it simply returns that model.
	Model* GetModel(const std::string& model);

	int LoadModel(const std::string& model);
private:

	void AddModel(Model* model);
	std::map<std::string, Model*> models;
	std::vector<Model*> modelsVec;
};