#pragma once
#include "GraphicsIncludes.h"
#include "ISystem.h"


class Camera : public ISystem
{
public:
	// Ctor/init
	Camera();
	int Init() override;

	// So that InputSystem can call our function
	enum CAMERA_DIR
	{
		FORWARD = 0,
		LEFT,
		RIGHT,
		BACK,
		UP,
		DOWN
	};

	// Camera coordinate system
	glm::vec3 worldUp = glm::vec3{ 0.0f, 1.0f , 0.0f };
	glm::vec3 position = glm::vec3{ 0.0f, 0.0f , 3.0f };
	glm::vec3 forward = glm::vec3{0.0f, 0.0f ,-1.0f};
	glm::vec3 up;
	glm::vec3 right;

	// Euler Angles
	float yaw = -90.0f;
	float pitch = 0.0f;

	// Camera options
	float movSpeed = 2.5f;
	float sensitivity = 0.05f;
	float zoom = 45.0f;

	// Functions
	glm::mat4 GetViewMatrix();
	glm::mat4 GetProjMatrix();
	void MovementInput(CAMERA_DIR dir, float dt);
	void MouseMovementInput(float x, float y, GLboolean constrainPitch = true);
	void MouseScrollInput(float offset);

	void Reset();
	void updateCameraVectors();
private:
	// Called whenever the vectors are changed
	
};