#pragma once
#include "IScene.h"

struct BasicScene : public IScene
{
	int Init();

	int PreRender(float dt = 1.0f/60.0f) override;
	int Render(float dt = 1.0f / 60.0f)override;
	int PostRender(float dt = 1.0f / 60.0f)override;
	void Shutdown();

private:
	// Just two triangles making up a quad
	std::vector<GLfloat> geometryBuffer = {	0.0f, 0.0f, 0.0f,
											1.0f, 0.0f, 0.0f,
											0.0f, 1.0f, 0.0f,
											1.0f, 0.0f, 0.0f,
											1.0f, 1.0f, 0.0f,
											0.0f, 1.0f, 0.0f
										  };

	GLuint  vertexbuffer;
	GLuint  VertexArrayID;
	float angleOfRotation = 0.0f;
};