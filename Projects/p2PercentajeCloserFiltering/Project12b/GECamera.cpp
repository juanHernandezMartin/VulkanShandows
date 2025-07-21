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

	turnLeftPressed = false;
	turnRightPressed = false;
	turnUpPressed = false;
	turnDownPressed = false;
	turnCWPressed = false;
	turnCCWPressed = false;
	moveLeftPressed = false;
	moveRightPressed = false;
	moveUpPressed = false;
	moveDownPressed = false;
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
void GECamera::update()
{
	if (turnLeftPressed && !turnRightPressed) turnLeft();
	if (!turnLeftPressed && turnRightPressed) turnRight();
	if (turnUpPressed && !turnDownPressed) turnUp();
	if (!turnUpPressed && turnDownPressed) turnDown();
	if (turnCWPressed && !turnCCWPressed) turnCW();
	if (!turnCWPressed && turnCCWPressed) turnCCW();
	if (moveLeftPressed && !moveRightPressed) moveLeft();
	if (!moveLeftPressed && moveRightPressed) moveRight();
	if (moveUpPressed && !moveDownPressed) moveUp();
	if (!moveUpPressed && moveDownPressed) moveDown();

	moveFront();
}

//
// FUNCI�N: GECamera::moveFront()
//
// PROP�SITO: Mueve el observador un paso (moveStep) en la direcci�n -Dir 
//
void GECamera::moveFront()
{
	Pos -= moveStep * Dir;
}

//
// FUNCI�N: GECamera::moveBack()
//
// PROP�SITO: Mueve el observador un paso (moveStep) hacia atr�s en la direcci�n Dir 
//
void GECamera::moveBack()
{
	Pos += moveStep * Dir;
}

//
// FUNCI�N: GECamera::moveLeft()
//
// PROP�SITO: Mueve el observador un paso (moveStep) hacia la izquierda. 
//
void GECamera::moveLeft()
{
//	Pos -= moveStep * Right;
	Pos -= 0.1f * Right;
}

//
// FUNCI�N: GECamera::moveRight()
//
// PROP�SITO: Mueve el observador un paso (moveStep) hacia la derecha. 
//
void GECamera::moveRight()
{
//	Pos += moveStep * Right;
	Pos += 0.1f * Right;
}

//
// FUNCI�N: GECamera::moveUp()
//
// PROP�SITO: Mueve el observador un paso (moveStep) hacia arriba. 
//
void GECamera::moveUp()
{
//	Pos += moveStep * Up;
	Pos += 0.1f * Up;
}

//
// FUNCI�N: GECamera::moveDown()
//
// PROP�SITO: Mueve el observador un paso (moveStep) hacia abajo. 
//
void GECamera::moveDown()
{
//	Pos -= moveStep * Up;
	Pos -= 0.1f * Up;
}

//
// FUNCI�N: GECamera::turnRight()
//
// PROP�SITO: Rota el observador un paso (angleStep) hacia su derecha.
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
// FUNCI�N: CACamera::turnLeft()
//
// PROP�SITO: Rota el observador un paso (angleStep) hacia su izquierda.
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
// FUNCI�N: GECamera::turnUp()
//
// PROP�SITO: Rota el observador un paso (angleStep) hacia arriba.
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
// FUNCI�N: GECamera::turnDown()
//
// PROP�SITO: Rota el observador un paso (angleStep) hacia abajo.
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
// FUNCI�N: GECamera::turnCW()
//
// PROP�SITO: Rota el observador un paso (angleStep) en sentido del reloj.
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
// FUNCI�N: GECamera::turnCCW()
//
// PROP�SITO: Rota el observador un paso (angleStep) en sentido contrario al reloj.
//
void GECamera::turnCCW()
{
	Up.x = cosAngle * Up.x - sinAngle * Right.x;
	Up.y = cosAngle * Up.y - sinAngle * Right.y;
	Up.z = cosAngle * Up.z - sinAngle * Right.z;

	// Right = Up x Dir
	Right = glm::cross(Up, Dir);
}

//
// FUNCI�N: GECamera::setTurnLeft(bool flag)
//
// PROP�SITO: Activa o desactiva el giro a la izquierda
//
void GECamera::setTurnLeft(bool flag)
{
	turnLeftPressed = flag;
}

//
// FUNCI�N: GECamera::setTurnRight(bool flag)
//
// PROP�SITO: Activa o desactiva el giro a la derecha
//
void GECamera::setTurnRight(bool flag)
{
	turnRightPressed = flag;
}

//
// FUNCI�N: GECamera::setTurnUp(bool flag)
//
// PROP�SITO: Activa o desactiva el giro hacia arriba
//
void GECamera::setTurnUp(bool flag)
{
	turnUpPressed = flag;
}

//
// FUNCI�N: GECamera::setTurnDown(bool flag)
//
// PROP�SITO: Activa o desactiva el giro hacia abajo
//
void GECamera::setTurnDown(bool flag)
{
	turnDownPressed = flag;
}

//
// FUNCI�N: GECamera::setTurnCW(bool flag)
//
// PROP�SITO: Activa o desactiva el giro horario
//
void GECamera::setTurnCW(bool flag)
{
	turnCWPressed = flag;
}

//
// FUNCI�N: GECamera::setTurnCCW(bool flag)
//
// PROP�SITO: Activa o desactiva el giro antihorario
//
void GECamera::setTurnCCW(bool flag)
{
	turnCCWPressed = flag;
}

//
// FUNCI�N: GECamera::setMoveLeft(bool flag)
//
// PROP�SITO: Activa o desactiva el desplazamiento a la izquierda
//
void GECamera::setMoveLeft(bool flag)
{
	moveLeftPressed = flag;
}

//
// FUNCI�N: GECamera::setMoveRight(bool flag)
//
// PROP�SITO: Activa o desactiva el desplazamiento a la derecha
//
void GECamera::setMoveRight(bool flag)
{
	moveRightPressed = flag;
}

//
// FUNCI�N: GECamera::setMoveUp(bool flag)
//
// PROP�SITO: Activa o desactiva el desplazamiento hacia arriba
//
void GECamera::setMoveUp(bool flag)
{
	moveUpPressed = flag;
}

//
// FUNCI�N: GECamera::setMoveDown(bool flag)
//
// PROP�SITO: Activa o desactiva el desplazamiento hacia abajo
//
void GECamera::setMoveDown(bool flag)
{
	moveDownPressed = flag;
}