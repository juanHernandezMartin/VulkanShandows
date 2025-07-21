#pragma once

#include <glm/glm.hpp>

//
// ESTRUCTURA: GESkyboxTransform
//
// DESCRIPCI�N: Estructura que describe la matriz de transformaci�n de un skybox
//
typedef struct
{
	alignas(16) glm::mat4 inverse;
} GESkyboxTransform;