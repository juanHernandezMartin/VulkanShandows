#pragma once

#include <glm/glm.hpp>

//
// ESTRUCTURA: GEShadowTransform
//
// DESCRIPCI�N: Estructura que describe la matriz de transformaci�n utilizada en el shadowmap
//
typedef struct
{
	alignas(16) glm::mat4 MVP;
} GEShadowTransform;