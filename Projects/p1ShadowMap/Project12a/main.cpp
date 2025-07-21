#include "GEApplication.h"
#include <iostream>
#include <stdexcept>

//
// PROYECTO: Project12a
//
// DESCRIPCIÓN: Desarrolla la técnica de shadowmap básico
//
int main()
{
	GEApplication app;

	try
	{
		app.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}