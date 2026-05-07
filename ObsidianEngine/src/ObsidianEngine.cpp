#include "../include/Application.h"

#include "../include/Vector.h"

int main()
{
	try 
	{
		ObsidianEngine::Application app;
		app.run();
	}
	catch (const std::exception& e) 
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}