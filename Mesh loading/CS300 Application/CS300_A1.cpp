#include "CS300_A1.h"
#include "BasiceScene.h"
#include "Engine.h"
#include "Graphics.h"
#include "Camera.h"
#include "imgui.h"
#include <algorithm>
int CS300_A1::Init()
{
	// Setting up the main object
	objects.reserve(10);
	Object obj{};
	obj.modelID = currModel = "Running";
	obj.scale = glm::vec3{ 0.01,0.01,0.01 };
	objects.push_back(obj);

	Light light;
	light.modelID = "Running";
	light.diffuse = glm::vec3{ 1,1,1 };
	light.type = Light::POINT;
	light.position = glm::vec3{0, 3, 3};
	light.scale = glm::vec3{0.1, 0.1, 0.1};
	light.direction = light.position;
	lights.push_back(light);
	lights[0].isOn = true;
	
	fogColor = Engine::Instance()->GetSystem<Graphics>()->GetBGColor();

	// Setup camera positions and misc variables
	Engine::Instance()->GetSystem<Camera>()->position = glm::vec3{ 0,0,6 };
	models = Engine::Instance()->GetSystem<Graphics>()->GetModelManager()->GetAllModels();

	// setUpSkybox();
	return 0;
}

int CS300_A1::PreRender(float dt)
{
	currMaxTexture = 0;
	return 0;
}

int CS300_A1::Render(float dt)
{
	Graphics* graphics = Engine::Instance()->GetSystem<Graphics>();
	PreRender(dt);

	// Draw skybox first
	// drawSkybox();

	drawPhong(dt);

	// Draw lights
	// drawLights();

	return 0;
}

int CS300_A1::PostRender(float dt)
{
	return 0;
}

void CS300_A1::Shutdown()
{
	
}

#pragma region Draw Functions
void CS300_A1::drawPhong(float dt)
{
	Graphics* graphics = Engine::Instance()->GetSystem<Graphics>();
	glEnable(GL_DEPTH_TEST);

	// Check the draw mode
	drawMode == FILL ? glPolygonMode(GL_FRONT_AND_BACK, GL_FILL) : glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Drawing the main object with phong lighting (sort of)
	Shader* shader = graphics->GetShader("PhongS");
	shader->Use();
	setLightVariables(shader);

	glm::mat4 view = Engine::Instance()->GetSystem<Camera>()->GetViewMatrix();
	glm::mat4 persp = Engine::Instance()->GetSystem<Camera>()->GetProjMatrix();
	
	for (size_t i = 0; i < objects.size(); ++i)
	{
		Object& obj = objects[i];
		
		glm::mat4 model = obj.GetModelMat();
		glm::mat4 normalTransform = glm::transpose(obj.GetNormalMat());
		glm::mat4 MVP = GetMVP(model);

		shader->SetMat4("normalTransform", view * normalTransform);
		shader->SetMat4("modelToView", view * model);
		shader->SetMat4("MVP", MVP);

		obj.Draw(shader, dt);
	}
}

void CS300_A1::setLightVariables(Shader* shader)
{
	Graphics* graphics = Engine::Instance()->GetSystem<Graphics>();

	glm::mat4 view = Engine::Instance()->GetSystem<Camera>()->GetViewMatrix();
	glm::mat4 persp = Engine::Instance()->GetSystem<Camera>()->GetProjMatrix();

	unsigned dirNum = 0, pointNum = 0, spotNum = 0;
	// Setting uniforms for lights
	for (auto light : lights)
	{
		if (!light.isOn)
			continue;

		std::string lightPrefix;
		switch (light.type)
		{
		case Light::DIRECTIONAL:
		{
			lightPrefix = "light_dir_Lights";
			lightPrefix += "[" + std::to_string(dirNum) + "]" + ".";

			shader->SetVec3(lightPrefix + "direction", view * glm::vec4{ light.direction,0.0f });
			shader->SetVec3(lightPrefix + "ambient", light.ambient);
			shader->SetVec3(lightPrefix + "diffuse", light.diffuse);
			shader->SetVec3(lightPrefix + "specular", light.specular);

			++dirNum;
			break;
		}
		case Light::POINT:
			lightPrefix = "light_point_Lights";
			lightPrefix += "[" + std::to_string(pointNum) + "]" + ".";

			shader->SetVec3(lightPrefix + "position", view * glm::vec4{ light.position, 1.0f });
			shader->SetVec3(lightPrefix + "ambient", light.ambient);
			shader->SetVec3(lightPrefix + "diffuse", light.diffuse);
			shader->SetVec3(lightPrefix + "specular", light.specular);

			++pointNum;
			break;
		case Light::SPOTLIGHT:
			lightPrefix = "light_spot_Lights";
			lightPrefix += "[" + std::to_string(spotNum) + "]" + ".";

			shader->SetVec3(lightPrefix + "position", view * glm::vec4{ light.position, 1.0f });
			shader->SetVec3(lightPrefix + "direction", view * glm::vec4{ light.direction, 0.0f });

			shader->SetVec3(lightPrefix + "ambient", light.ambient);
			shader->SetVec3(lightPrefix + "diffuse", light.diffuse);
			shader->SetVec3(lightPrefix + "specular", light.specular);

			shader->SetFloat(lightPrefix + "inner_angle", cosf(glm::radians(light.innerAngle)));
			shader->SetFloat(lightPrefix + "outer_angle", cosf(glm::radians(light.outerAngle)));
			shader->SetFloat(lightPrefix + "falloff", light.spotFalloff);

			++spotNum;
			break;
		}
	}

	// Set number of each type of light
	shader->SetInt("light_dir_numLights", dirNum);
	shader->SetInt("light_point_numLights", pointNum);
	shader->SetInt("light_spot_numLights", spotNum);

	// Set the global ambient and fog attributes
	shader->SetVec3("global_ambient", globalAmbient);
	shader->SetVec3("fog_color", fogColor);
	shader->SetFloat("z_near", zNear);
	shader->SetFloat("z_far", zFar);

	// Set the attenuation properties
	shader->SetFloat("constant", attenuation[CONSTANT]);
	shader->SetFloat("linear", attenuation[LINEAR]);
	shader->SetFloat("quadratic", attenuation[QUADRATIC]);

	// Binding the diffuse and specular map
	GLuint diffuseMap = graphics->textureManager.GetTexture("A2_Diffuse");
	GLuint specularMap = graphics->textureManager.GetTexture("A2_Specular");

	// Setting material attributes
	shader->SetVec3("material_emissive", materialEmissive);
	shader->SetVec3("material.ambient", materialAmbient);
	shader->SetInt("material.diffuse", currMaxTexture);
	glActiveTexture(GL_TEXTURE0 + currMaxTexture++);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);
	shader->SetInt("material.specular", currMaxTexture);
	glActiveTexture(GL_TEXTURE0 + currMaxTexture++);
	glBindTexture(GL_TEXTURE_2D, specularMap);

}

void CS300_A1::drawLights()
{
	Shader* shader = Engine::Instance()->GetSystem<Graphics>()->GetShader("default");
	shader->Use();

	for (auto light : lights)
	{
		if (!light.isOn)
			continue;
		glm::mat4 model = light.GetModelMat();
		glm::mat4 MVP = GetMVP(model);

		shader->SetMat4("vertexTransform", MVP);
		shader->SetVec3("objectColor", light.diffuse);
		light.Draw(shader, 0.0f);
	}
}

void CS300_A1::drawLights(const glm::mat4 view, const glm::mat4 perps)
{
	Shader* shader = Engine::Instance()->GetSystem<Graphics>()->GetShader("default");
	shader->Use();

	for (auto light : lights)
	{
		if (!light.isOn)
			continue;
		glm::mat4 model = light.GetModelMat();
		glm::mat4 MVP = perps * view * model;

		shader->SetMat4("vertexTransform", MVP);
		shader->SetVec3("objectColor", light.diffuse);
		light.Draw(shader, 0.0f);
	}
}

#pragma region SkyBox

void CS300_A1::drawSkybox()
{
	Graphics* graphics = Engine::Instance()->GetSystem<Graphics>();
	Shader* shader = graphics->GetShader("skybox");
	shader->Use();
	glDepthMask(false);

	// remove translation from the view matrix
	glm::mat4 view = glm::mat4(glm::mat3(Engine::Instance()->GetSystem<Camera>()->GetViewMatrix()));
	shader->SetMat4("view", view);
	shader->SetMat4("projection", Engine::Instance()->GetSystem<Camera>()->GetProjMatrix());

	glBindVertexArray(skyboxVAO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, graphics->textureManager.GetTexture("skybox"));

	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthMask(true);
}

void CS300_A1::drawSkybox(const glm::mat4 v, const glm::mat4 p)
{
	Graphics* graphics = Engine::Instance()->GetSystem<Graphics>();
	Shader* shader = graphics->GetShader("skybox");
	shader->Use();
	glDepthMask(false);

	// remove translation from the view matrix
	glm::mat4 view = glm::mat4(glm::mat3(v));
	shader->SetMat4("view", view);
	shader->SetMat4("projection", p);

	glBindVertexArray(skyboxVAO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, graphics->textureManager.GetTexture("skybox"));

	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthMask(true);
}

#pragma endregion

#pragma region Misc 
#pragma endregion Misc

#pragma endregion Draw Functions

#pragma region Setup Functions

void CS300_A1::setUpSkybox()
{
	glGenVertexArrays(1, &skyboxVAO);
	glBindVertexArray(skyboxVAO);

	glGenBuffers(1, &skyboxVBO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, skyboxVertices.size() * sizeof(glm::vec3), skyboxVertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)(0));
	glEnableVertexAttribArray(0);
}

#pragma endregion Setup Function

#pragma region ImGui
int CS300_A1::DisplayImGui()
{
	// ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);
	if (ImGui::BeginTabItem("General Settings"))
	{
		ImGui::NewLine();
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("This tab contains most of the general settings you would need.");
			// ImGui::Text("Generally, first load will take a while. Subsequent loads will be instant.");
			// ImGui::Text("Models displayed here are the files inside /resource/models folder.");
			ImGui::EndTooltip();
		}
		ImGui::NewLine();
		
#pragma region Models And Constants
		ImGui::Text("Model Selection");
		ImGui::Separator();

		if (ImGui::BeginCombo("Models", currModel.c_str()))
		{
			for (size_t i = 0; i < models.size(); ++i)
			{
				if (ImGui::Selectable(models[i]->name.c_str(), currModel == models[i]->name))
				{
					currModel = models[i]->name;
					objects[0].modelID = currModel;
				}
			}
			ImGui::EndCombo();
		}

		ImGui::SameLine();
		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("You can select the available models to load here.");
			ImGui::Text("Generally, first load will take a while. Subsequent loads will be instant.");
			ImGui::Text("Models displayed here are the files inside /resource/models folder.");
			ImGui::EndTooltip();
		}

		ImGui::NewLine();

		ImGui::Text("Light Color Constants");
		ImGui::Separator();
		ImGui::ColorEdit3("Global Ambient", (float*)&globalAmbient);
		ImGui::ColorEdit3("Material Ambient", (float*)&materialAmbient);
		ImGui::ColorEdit3("Fog Color", (float*)&fogColor);
		
		ImGui::NewLine();
		ImGui::Text("Light Attenuation Constants");
		ImGui::Separator();
		ImGui::DragFloat("Z Near", &zNear, 0.1f, 0.0f, zFar - 0.1f);
		ImGui::DragFloat("Z Far",  &zFar,  0.1f, zNear);
		ImGui::DragFloat3("Attenuation", attenuation, 0.01f, 0.0f, 1.0f);
		ImGui::Text("   Constant	   Linear      Quadratic ");

		ImGui::NewLine();
#pragma endregion Models And Constants

#pragma region Shader Variables
		ImGui::Text("Shader Settings");
		ImGui::Separator();

		if (ImGui::Button("Reload Shader", ImVec2{ 170, 50 }))
		{
			Engine::Instance()->GetSystem<Graphics>()->shaderManager.ReloadShader("PhongS");
			Engine::Instance()->GetSystem<Graphics>()->shaderManager.ReloadShader("debug");
		}

		ImGui::NewLine();

		// ImGui::Text("Shader Options");
		// ImGui::Separator();
		// displayShaderOptions();
		// ImGui::NewLine();

#pragma endregion Shader Variables

		ImGui::Text("Misc Settings");
		ImGui::Separator();

		static const char* modes[] = { "Fill", "Wireframe", "None" };
		if (ImGui::BeginCombo("Draw Mode", modes[drawMode]))
		{
			for (size_t i = 0; i < IM_ARRAYSIZE(modes); ++i)
				if (ImGui::Selectable(modes[i], drawMode == static_cast<DRAW_MODE>(i)))
				{
					drawMode = static_cast<DRAW_MODE>(i);
				}
			ImGui::EndCombo();
		}
		
		ImGui::EndTabItem();
	}
	
#pragma region Light Scenes & Per Light
	if (ImGui::BeginTabItem("Per-light Settings"))
	{
		ImGui::NewLine();
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("This tab provides more fine control over each light.");
			ImGui::Text("Light 1 is the closest light to the camera when the scene first starts.");
			ImGui::Text("The rest of the lights are counted in a CW direction about the orbit.");
			ImGui::EndTooltip();
		}
		ImGui::TextColored(ImVec4{ 0,1,0,1 }, "Lights are counted in CW direction starting from nearest light.");
		static const char* lightTypes[] = { "Point Light", "Directional Light", "Spotlight" };
		for (unsigned i = 0; i < lights.size(); ++i)
		{
			ImGui::SetNextTreeNodeOpen(lights[i].isOn, ImGuiCond_Once);
			if (ImGui::TreeNodeEx(&lights[i], ImGuiTreeNodeFlags_Framed, "Light %u", i))
			{
				ImGui::Checkbox("On", &lights[i].isOn);

				ImGui::PushItemWidth(180);
				if (ImGui::BeginCombo("Light Type", lightTypes[lights[i].type]))
				{
					for (size_t j = 0; j < IM_ARRAYSIZE(lightTypes); ++j)
						if (ImGui::Selectable(lightTypes[j], lights[i].type == static_cast<Light::LIGHT_TYPE>(j)))
						{
							lights[i].type = static_cast<Light::LIGHT_TYPE>(j);
						}
					ImGui::EndCombo();
				}
				ImGui::PopItemWidth();
				ImGui::SameLine();
				if (ImGui::Button("Apply To All"))
				{
					for (unsigned j = 0; j < lights.size(); ++j)
					{
						lights[j].type = lights[i].type;
					}
				}

				ImGui::ColorEdit3("Ambient", (float*)&lights[i].ambient);
				ImGui::ColorEdit3("Diffuse", (float*)&lights[i].diffuse);
				ImGui::ColorEdit3("Specular", (float*)&lights[i].specular);
				// ImGui::DragFloat3("Dir", (float*)&lights[i].position, 0.1f, 0.f, 1.f);


				if (lights[i].type == Light::SPOTLIGHT)
				{
					// Expose the inner and outer angle for editing
					ImGui::DragFloat("Inner Angle", &lights[i].innerAngle, 0.1f, 1.0f, lights[i].outerAngle - 1.0f);
					ImGui::DragFloat("Outer Angle", &lights[i].outerAngle, 0.1f, lights[i].innerAngle, 90.0f);
					ImGui::DragFloat("Fall Off", &lights[i].spotFalloff, 0.1f, 1.0f, 10.0f);
				}


				ImGui::NewLine();
				ImGui::TreePop();
			}
		}
		// ImGui::NewLine();
		ImGui::EndTabItem();
	}
#pragma endregion Light Scenes & Per Light
	
	return 0;
}


#pragma endregion