#pragma once

#include <glm/glm.hpp>

//
// ESTRUCTURA: GEShadowTransform
//
// DESCRIPCIÓN: Estructura que describe la matriz de transformación utilizada en el shadowmap
//
typedef struct
{
	alignas(16) glm::mat4 MVP;
} GEShadowTransform;