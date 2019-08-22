#include "BasiceScene.h"
#include "Engine.h"
#include "Graphics.h"

int BasicScene::Init()
{
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	glGenBuffers(1, &vertexbuffer);

	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, geometryBuffer.size() * sizeof(GLfloat),
		geometryBuffer.data(), GL_STATIC_DRAW);
	return 0;
}

int BasicScene::PreRender(float dt)
{
	return 0;
}

int BasicScene::Render(float dt)
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Calls glUseProgram
	Shader* shader = Engine::Instance()->GetSystem<Graphics>()->GetShader("default");
	shader->Use();

	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(	0,
							3,
							GL_FLOAT,
							GL_FALSE,
							0,
							(void *)0);

	// Draw the triangle !
	// T * R * S * Vertex
	glm::mat4 modelMat = glm::mat4(1.0f);
	glm::vec3 scaleVector = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 centroid = glm::vec3(-0.5f, -0.5f, 0.0f);
	modelMat =	glm::rotate(angleOfRotation, glm::vec3(0.0f, 0.0f, 1.0f)) *
				glm::scale(scaleVector) *
				glm::translate(centroid);

	// Uniform matrix
	// Uniform transformation (vertex shader)
	// GLint vTransformLoc = glGetUniformLocation(prog, "vertexTransform");
	shader->SetMat4("vertexTransform", GetMVP(modelMat));
	

	// glUniformMatrix4fv(vTransformLoc, 1, GL_FALSE, &modelMat[0][0]);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(0);
	return 0;
}

int BasicScene::PostRender(float dt)
{
	angleOfRotation += 0.01f;
	return 0;
}

void BasicScene::Shutdown()
{
	// Cleanup VBO
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
}
