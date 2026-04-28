#include "GECamera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//
// FUNCIÓN: GECamera::GECamera()
//
// PROPÓSITO: Construye una cámara
//
// COMENTARIOS: 
//     La posición inicial es (0,0,0).
//     La orientación incial es el sistema de coordenadas del modelo
//     El tamaño del paso inicial es 0.1
//     El támaño del giro inicial es 1.0 grados
//
GECamera::GECamera()
{
	Pos = glm::vec3(0.0f, 0.0f, 0.0f);
	Dir = glm::vec3(0.0f, 0.0f, 1.0f);
	Up = glm::vec3(0.0f, 1.0f, 0.0f);
	Right = glm::vec3(1.0f, 0.0f, 0.0f);

	moveStep = 0.1f;
	turnStep = 1.0f;
	cosAngle = (float)cos(glm::radians(turnStep));
	sinAngle = (float)sin(glm::radians(turnStep));
}

//
// FUNCIÓN: GECamera::getViewMatrix()
//
// PROPÓSITO: Obtiene la matriz View para situar la cámara.
//
glm::mat4 GECamera::getViewMatrix()
{
	return glm::lookAt(Pos, Pos - Dir, Up);
}

//
// FUNCIÓN: GECamera::setPosition(glm::vec3 pos)
//
// PROPÓSITO: Asigna la posición de la cámara con respecto al sistema de coordenadas del modelo.
//
void GECamera::setPosition(glm::vec3 pos)
{
	Pos = glm::vec3(pos);
}

//
// FUNCIÓN: GECamera::setDirection(glm::vec3 dir, glm::vec3 up)
//
// PROPÓSITO: Asigna la orientación de la cámara.
//
void GECamera::setDirection(glm::vec3 dir, glm::vec3 up)
{
	Dir = glm::vec3(dir);
	Up = glm::vec3(up);
	Right = glm::cross(Up, Dir);
}

//
// FUNCIÓN: GECamera::setMoveStep(float step)
//
// PROPÓSITO: Asigna el avance en cada paso.
//
void GECamera::setMoveStep(float step)
{
	moveStep = step;
}

//
// FUNCIÓN: GECamera::setTurnStep(float step)
//
// PROPÓSITO: Asigna el ángulo de giro en cada paso.
//
void GECamera::setTurnStep(float step)
{
	turnStep = step;
	cosAngle = (float)cos(glm::radians(turnStep));
	sinAngle = (float)sin(glm::radians(turnStep));
}

//
// FUNCIÓN: GECamera::getPosition()
//
// PROPÓSITO: Obtiene la posición de la cámara.
//
glm::vec3 GECamera::getPosition()
{
	return Pos;
}

//
// FUNCIÓN: GECamera::getDirection()
//
// PROPÓSITO: Obtiene la orientación de la cámara (eje Z).
//
glm::vec3 GECamera::getDirection()
{
	return Dir;
}

//
// FUNCIÓN: GECamera::getUpDirection()
//
// PROPÓSITO: Obtiene la orientación cenital de la cámara (eje Y).
//
glm::vec3 GECamera::getUpDirection()
{
	return Up;
}

//
// FUNCIÓN: GECamera::getMoveStep()
//
// PROPÓSITO: Obtiene el avance en cada paso.
//
float GECamera::getMoveStep()
{
	return moveStep;
}

//
// FUNCIÓN: GECamera::getTurnStep()
//
// PROPÓSITO: Obtiene el ángulo de giro en cada paso.
//
float GECamera::getTurnStep()
{
	return turnStep;
}

//
// FUNCIÓN: GECamera::update()
//
// PROPÓSITO: Actualiza la posición y orientación de la cámara 
//
void GECamera::update(float deltaTime)
{
	move(deltaTime);
	rotate(deltaTime);
}

// PROPOSITO: Mueve el observador un paso (moveStep) en la dirección -Dir 
void GECamera::move(float deltaTime)
{
	glm::vec3 forwardMove = glm::normalize(glm::vec3(Dir.x, 0.0f, Dir.z)) * cameraMoveDirection.z;
	glm::vec3 rightMove = Right * -cameraMoveDirection.x;
	glm::vec3 upMove = glm::vec3(0.0f, 1.0f, 0.0f) * cameraMoveDirection.y;
	glm::vec3 moveVector = forwardMove + rightMove + upMove;

	// Normalizar para evitar mayor velocidad en diagonales
	if (glm::length(moveVector) > 0.0f) {
		moveVector = glm::normalize(moveVector);
	}

	if (isSprinting) {
		moveVector *= sprintMultiplier;  // Aumentar la velocidad si se está corriendo
	}
	
	Pos -= moveVector * deltaTime * cameraSpeed;
}

// PROPOSITO: Mueve el observador un paso (moveStep) en la dirección -Dir 
void GECamera::rotate(float deltaTime)
{
	float xOffset = cameraRotationSpeed * cameraRotateDirection.x * deltaTime;
	float yOffset = cameraRotationSpeed * cameraRotateDirection.y * deltaTime;;

	// Calcular ángulos de rotación usando trigonometría
	float cosYaw = cos(glm::radians(xOffset));
	float sinYaw = sin(glm::radians(xOffset));
	float cosPitch = cos(glm::radians(yOffset));
	float sinPitch = sin(glm::radians(yOffset));

	// Rotar la dirección de la cámara (horizontalmente)
	glm::vec3 newDir;
	newDir.x = cosYaw * Dir.x - sinYaw * Dir.z;
	newDir.z = sinYaw * Dir.x + cosYaw * Dir.z;
	newDir.y = Dir.y;  // Mantiene la altura estable en la rotación horizontal

	// Aplicar el límite en el movimiento vertical
	newDir.y += sinPitch;
	if (newDir.y > 0.99f) newDir.y = 0.99f;  // Limitar la inclinación hacia arriba
	if (newDir.y < -0.99f) newDir.y = -0.99f;  // Limitar la inclinación hacia abajo

	// Normalizar el vector para mantener la magnitud correcta
	Dir = glm::normalize(newDir);

	// Recalcular los ejes Right y Up para que la cámara siempre esté nivelada
	Right = glm::normalize(glm::cross(Dir, glm::vec3(0.0f, 1.0f, 0.0f)));
	Up = glm::normalize(glm::cross(Right, Dir));
}



