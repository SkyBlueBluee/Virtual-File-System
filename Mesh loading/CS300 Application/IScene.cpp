#include "IScene.h"
#include "Engine.h"
#include "Camera.h"

glm::mat4 IScene::GetMVP(const glm::mat4 model)
{
	Camera* camera = Engine::Instance()->GetSystem<Camera>();

	return camera->GetProjMatrix() * camera->GetViewMatrix() * model;
}
