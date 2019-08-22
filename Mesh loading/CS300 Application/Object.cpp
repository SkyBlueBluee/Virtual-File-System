#include "Object.h"
#include "Engine.h"
#include "Graphics.h"

glm::mat4 Object::GetModelMat()
{
	// Returns the model to world matrix
	return	glm::translate(position) *
			glm::rotate(angle, axis) *
			glm::scale(scale);
}
glm::mat4 Object::GetModelMatInv()
{
	return	glm::scale(glm::vec3{1.0/scale.x, 1.0 / scale.y,1.0 / scale.z}) *
			glm::rotate(-angle, axis) * 
			glm::translate(-position);
}

glm::mat4 Object::GetNormalMat()
{
	return	glm::inverse(glm::rotate(angle, axis) * glm::scale(scale));
}

int Object::Draw()
{
	// Draw the object as per normal.
	Model* model = Engine::Instance()->GetSystem<Graphics>()->GetModel(modelID);
	assert(model != nullptr);
	return model->Draw();
}

