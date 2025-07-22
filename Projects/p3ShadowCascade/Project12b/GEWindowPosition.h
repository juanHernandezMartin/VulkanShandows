#pragma once

//
// ESTRUCTURA: GEWindowPosition
//
// DESCRIPCI�N: Estructura que almacena la posici�n y el tama�o de la ventana gr�fica
//
typedef struct {
	int width;
	int height;
	int Xpos;
	int Ypos;
	int screenWidth;
	int screenHeight;
	bool fullScreen;
} GEWindowPosition;
