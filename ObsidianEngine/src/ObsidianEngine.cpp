#include "Application.h"

#include "Vector.h"

using namespace ObsidianEngine;

int main()
{
	//try 
	//{
	//	ObsidianEngine::Application app;
	//	app.run();
	//}
	//catch (const std::exception& e) 
	//{
	//	std::cerr << e.what() << std::endl;
	//	return EXIT_FAILURE;
	//}

	Vector3 v { 2, 3, 4 };

	v.swizzle<"yx">() = { 10, 20 };
	v.set<"yz">(1, 2);
	
	

	std::cout << -v << std::endl;

	return EXIT_SUCCESS;
}