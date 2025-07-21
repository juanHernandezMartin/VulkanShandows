#include "GEApplication.h"
#include <iostream>
#include <stdexcept>

//
// PROYECTO: Project12a
//
// DESCRIPCI�N: Desarrolla la t�cnica de shadowmap b�sico
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