#include "Camera.h"
#include "Engine.h"
#include "Graphics.h"

Camera::Camera()
{
	updateCameraVectors();
}

int Camera::Init()
{
	// Nothing here for now.
	updateCameraVectors();
	return 0;
}

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(position, position + forward, up);
}

glm::mat4 Camera::GetProjMatrix()
{
	Graphics* graphics = Engine::Instance()->GetSystem<Graphics>();
	return glm::perspective(glm::radians(zoom), (float)graphics->GetWindowWidth() / (float)graphics->GetWindowHeight(), 0.1f, 100.0f);
}

void Camera::MovementInput(CAMERA_DIR dir, float dt)
{
	// This is to handle camera movement. Called by input system when opengl input callback triggers.
	float velocity = movSpeed * dt;
	if (dir == FORWARD)
		position += forward * velocity;
	if (dir == BACK)
		position -= forward * velocity;
	if (dir == LEFT)
		position -= right * velocity;
	if (dir == RIGHT)
		position += right * velocity;
	if (dir == UP)
		position += up * velocity;
	if (dir == DOWN)
		position -= up * velocity;
}

void Camera::MouseMovementInput(float x, float y, GLboolean constrainPitch)
{
	// Handle mouse inputs
	// Move according to sensitivity
	x *= sensitivity;
	y *= sensitivity;

	yaw += x;
	pitch += y;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch)
	{
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
	}

	// Update camera vectors
	updateCameraVectors();
}

void Camera::MouseScrollInput(float offset)
{
	// Scroll controls zoom
	if (zoom >= 1.0f && zoom <= 45.0f)
		zoom -= offset;
	if (zoom <= 1.0f)
		zoom = 1.0f;
	if (zoom >= 45.0f)
		zoom = 45.0f;
}

void Camera::Reset()
{
	// Reset the camera's position and angles
	position = glm::vec3{ 0.0f, 0.0f , 3.0f };
	forward = glm::vec3{ 0.0f, 0.0f ,-1.0f };
	yaw = -90.0f;
	pitch = 0.0f;
	zoom = 45.0f;
	updateCameraVectors();
}

void Camera::updateCameraVectors()
{
	// Calculate the new Front vector
	glm::vec3 newForward;

	float sinYaw = sin(glm::radians(yaw));
	float cosYaw = cos(glm::radians(yaw));
	
	float cosPitch = cos(glm::radians(pitch));
	float sinPitch = sin(glm::radians(pitch));

	newForward.x = cosYaw * cosPitch;
	newForward.y = sinPitch;
	newForward.z = sinYaw * cosPitch;
	forward = glm::normalize(newForward);

	// Also re-calculate the Right and Up vector
	// Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	right = glm::normalize(glm::cross(forward, worldUp)); 
	up = glm::normalize(glm::cross(right, forward));
}
