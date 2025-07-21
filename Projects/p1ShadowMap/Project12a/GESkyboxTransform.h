#pragma once

#include <glm/glm.hpp>

//
// ESTRUCTURA: GESkyboxTransform
//
// DESCRIPCIÓN: Estructura que describe la matriz de transformación de un skybox
//
typedef struct
{
	alignas(16) glm::mat4 inverse;
} GESkyboxTransform;