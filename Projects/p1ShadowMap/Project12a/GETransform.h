#pragma once

//
// ESTRUCTURA: GETransform
//
// DESCRIPCI�N: Estructura que almacena los datos necesarios para girar las figuras
//
typedef struct
{
	alignas(16) glm::mat4 MVP;
	alignas(16) glm::mat4 ViewMatrix;
	alignas(16) glm::mat4 ModelViewMatrix;
	alignas(16) glm::mat4 ShadowMatrix;
} GETransform;