#pragma once

#include <glm\glm.hpp>

//
// ESTRUCTURA: GEVertex
//
// DESCRIPCI�N: Estructura que describe los atributos de un v�rtice
//
typedef struct 
{
	glm::vec3 pos;
	glm::vec3 norm;
	glm::vec2 tex;
} GEVertex;

