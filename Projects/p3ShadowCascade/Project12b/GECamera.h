#pragma once

#include <glm/glm.hpp>

class GECamera {
public:
	GECamera();
	glm::mat4 getViewMatrix();

	float cameraSpeed = 25.0f;
	float cameraRotationSpeed = 1.0f;
	glm::vec3 cameraMoveDirection = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec2 cameraRotateDirection = glm::vec2(0.0f, 0.0f);

	void setPosition(glm::vec3 pos);
	void setDirection(glm::vec3 dir, glm::vec3 up);
	void setMoveStep(float step);
	void setTurnStep(float step);

	glm::vec3 getPosition();
	glm::vec3 getDirection();
	glm::vec3 getUpDirection();
	float getMoveStep();
	float getTurnStep();

	void update(float deltaTime);
	void move(float deltaTime);
	void rotate(float deltaTime);

	void turnRight();
	void turnLeft();
	void turnUp();
	void turnDown();
	void turnCW();
	void turnCCW();

private:
	glm::vec3 Pos;
	glm::vec3 Dir;
	glm::vec3 Up;
	glm::vec3 Right;

	float moveStep;
	float turnStep;
	float cosAngle;
	float sinAngle;
};

