#include "GEApplication.h"
#include <iostream>
#include <stdexcept>

//
// PROYECTO: Project12b
//
// DESCRIPCIÓN: Desarrolla la técnica de shadowmap con percentage-closer filtering
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