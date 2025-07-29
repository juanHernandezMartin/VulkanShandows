#include "GECamera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//
// FUNCI�N: GECamera::GECamera()
//
// PROP�SITO: Construye una c�mara
//
// COMENTARIOS: 
//     La posici�n inicial es (0,0,0).
//     La orientaci�n incial es el sistema de coordenadas del modelo
//     El tama�o del paso inicial es 0.1
//     El t�ma�o del giro inicial es 1.0 grados
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
// FUNCI�N: GECamera::getViewMatrix()
//
// PROP�SITO: Obtiene la matriz View para situar la c�mara.
//
glm::mat4 GECamera::getViewMatrix()
{
	return glm::lookAt(Pos, Pos - Dir, Up);
}

//
// FUNCI�N: GECamera::setPosition(glm::vec3 pos)
//
// PROP�SITO: Asigna la posici�n de la c�mara con respecto al sistema de coordenadas del modelo.
//
void GECamera::setPosition(glm::vec3 pos)
{
	Pos = glm::vec3(pos);
}

//
// FUNCI�N: GECamera::setDirection(glm::vec3 dir, glm::vec3 up)
//
// PROP�SITO: Asigna la orientaci�n de la c�mara.
//
void GECamera::setDirection(glm::vec3 dir, glm::vec3 up)
{
	Dir = glm::vec3(dir);
	Up = glm::vec3(up);
	Right = glm::cross(Up, Dir);
}

//
// FUNCI�N: GECamera::setMoveStep(float step)
//
// PROP�SITO: Asigna el avance en cada paso.
//
void GECamera::setMoveStep(float step)
{
	moveStep = step;
}

//
// FUNCI�N: GECamera::setTurnStep(float step)
//
// PROP�SITO: Asigna el �ngulo de giro en cada paso.
//
void GECamera::setTurnStep(float step)
{
	turnStep = step;
	cosAngle = (float)cos(glm::radians(turnStep));
	sinAngle = (float)sin(glm::radians(turnStep));
}

//
// FUNCI�N: GECamera::getPosition()
//
// PROP�SITO: Obtiene la posici�n de la c�mara.
//
glm::vec3 GECamera::getPosition()
{
	return Pos;
}

//
// FUNCI�N: GECamera::getDirection()
//
// PROP�SITO: Obtiene la orientaci�n de la c�mara (eje Z).
//
glm::vec3 GECamera::getDirection()
{
	return Dir;
}

//
// FUNCI�N: GECamera::getUpDirection()
//
// PROP�SITO: Obtiene la orientaci�n cenital de la c�mara (eje Y).
//
glm::vec3 GECamera::getUpDirection()
{
	return Up;
}

//
// FUNCI�N: GECamera::getMoveStep()
//
// PROP�SITO: Obtiene el avance en cada paso.
//
float GECamera::getMoveStep()
{
	return moveStep;
}

//
// FUNCI�N: GECamera::getTurnStep()
//
// PROP�SITO: Obtiene el �ngulo de giro en cada paso.
//
float GECamera::getTurnStep()
{
	return turnStep;
}

//
// FUNCI�N: GECamera::update()
//
// PROP�SITO: Actualiza la posici�n y orientaci�n de la c�mara 
//
void GECamera::update(float deltaTime)
{
	move(deltaTime);
	rotate(deltaTime);
}

// PROPOSITO: Mueve el observador un paso (moveStep) en la direcci�n -Dir 
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
		moveVector *= sprintMultiplier;  // Aumentar la velocidad si se est� corriendo
	}
	
	Pos -= moveVector * deltaTime * cameraSpeed;
}

// PROPOSITO: Mueve el observador un paso (moveStep) en la direcci�n -Dir 
void GECamera::rotate(float deltaTime)
{
	float xOffset = cameraRotationSpeed * cameraRotateDirection.x * deltaTime;
	float yOffset = cameraRotationSpeed * cameraRotateDirection.y * deltaTime;;

	// Calcular �ngulos de rotaci�n usando trigonometr�a
	float cosYaw = cos(glm::radians(xOffset));
	float sinYaw = sin(glm::radians(xOffset));
	float cosPitch = cos(glm::radians(yOffset));
	float sinPitch = sin(glm::radians(yOffset));

	// Rotar la direcci�n de la c�mara (horizontalmente)
	glm::vec3 newDir;
	newDir.x = cosYaw * Dir.x - sinYaw * Dir.z;
	newDir.z = sinYaw * Dir.x + cosYaw * Dir.z;
	newDir.y = Dir.y;  // Mantiene la altura estable en la rotaci�n horizontal

	// Aplicar el l�mite en el movimiento vertical
	newDir.y += sinPitch;
	if (newDir.y > 0.99f) newDir.y = 0.99f;  // Limitar la inclinaci�n hacia arriba
	if (newDir.y < -0.99f) newDir.y = -0.99f;  // Limitar la inclinaci�n hacia abajo

	// Normalizar el vector para mantener la magnitud correcta
	Dir = glm::normalize(newDir);

	// Recalcular los ejes Right y Up para que la c�mara siempre est� nivelada
	Right = glm::normalize(glm::cross(Dir, glm::vec3(0.0f, 1.0f, 0.0f)));
	Up = glm::normalize(glm::cross(Right, Dir));
}



