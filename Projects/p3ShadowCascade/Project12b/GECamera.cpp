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

	Pos -= moveVector * deltaTime * cameraSpeed;
}

// PROPOSITO: Mueve el observador un paso (moveStep) en la dirección -Dir 
void GECamera::rotate(float deltaTime)
{
	
}


//
// FUNCIÓN: GECamera::turnRight()
//
// PROPÓSITO: Rota el observador un paso (angleStep) hacia su derecha.
//
void GECamera::turnRight()
{
	Dir.x = cosAngle * Dir.x - sinAngle * Right.x;
	Dir.y = cosAngle * Dir.y - sinAngle * Right.y;
	Dir.z = cosAngle * Dir.z - sinAngle * Right.z;

	// Right = Up x Dir
	Right = glm::cross(Up, Dir);
}

//
// FUNCIÓN: CACamera::turnLeft()
//
// PROPÓSITO: Rota el observador un paso (angleStep) hacia su izquierda.
//
void GECamera::turnLeft()
{
	Dir.x = cosAngle * Dir.x + sinAngle * Right.x;
	Dir.y = cosAngle * Dir.y + sinAngle * Right.y;
	Dir.z = cosAngle * Dir.z + sinAngle * Right.z;

	// Right = Up x Dir
	Right = glm::cross(Up, Dir);
}

//
// FUNCIÓN: GECamera::turnUp()
//
// PROPÓSITO: Rota el observador un paso (angleStep) hacia arriba.
//
void GECamera::turnUp()
{
	Dir.x = cosAngle * Dir.x - sinAngle * Up.x;
	Dir.y = cosAngle * Dir.y - sinAngle * Up.y;
	Dir.z = cosAngle * Dir.z - sinAngle * Up.z;

	// Up = Dir x Right
	Up = glm::cross(Dir, Right);
}

//
// FUNCIÓN: GECamera::turnDown()
//
// PROPÓSITO: Rota el observador un paso (angleStep) hacia abajo.
//
void GECamera::turnDown()
{
	Dir.x = cosAngle * Dir.x + sinAngle * Up.x;
	Dir.y = cosAngle * Dir.y + sinAngle * Up.y;
	Dir.z = cosAngle * Dir.z + sinAngle * Up.z;

	// Up = Dir x Right
	Up = glm::cross(Dir, Right);
}

//
// FUNCIÓN: GECamera::turnCW()
//
// PROPÓSITO: Rota el observador un paso (angleStep) en sentido del reloj.
//
void GECamera::turnCW()
{
	Up.x = cosAngle * Up.x + sinAngle * Right.x;
	Up.y = cosAngle * Up.y + sinAngle * Right.y;
	Up.z = cosAngle * Up.z + sinAngle * Right.z;

	// Right = Up x Dir
	Right = glm::cross(Up, Dir);
}

//
// FUNCIÓN: GECamera::turnCCW()
//
// PROPÓSITO: Rota el observador un paso (angleStep) en sentido contrario al reloj.
//
void GECamera::turnCCW()
{
	Up.x = cosAngle * Up.x - sinAngle * Right.x;
	Up.y = cosAngle * Up.y - sinAngle * Right.y;
	Up.z = cosAngle * Up.z - sinAngle * Right.z;

	// Right = Up x Dir
	Right = glm::cross(Up, Dir);
}
