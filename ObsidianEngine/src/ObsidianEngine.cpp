#include "../include/Application.h"

#include "../include/Vector.h"

template<class T>
concept test = requires (T t) { t++; };

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

	ObsidianEngine::Vector3 vec = ObsidianEngine::Vector3();

	ObsidianEngine::Vector4 v{ 1, 2, 3, 4 };

	ObsidianEngine::Vector2 xy = v.swizzle<"xy">();

    std::cout << xy << std::endl;


    v.swizzle<"xy">() = ObsidianEngine::Vector2{ 10, 20 };
    std::cout << v << std::endl;


    v.swizzle<"yx">() = ObsidianEngine::Vector2{ 100, 200 };

    std::cout << v << std::endl;

	ObsidianEngine::Vector3 xyz = v.swizzle<"xyz">();

    std::cout << xyz << std::endl;


	ObsidianEngine::Vector2 xx = v.swizzle<"xx">();

	ObsidianEngine::Vector3::dot(xyz, xyz);

	std::cout << xy << std::endl;
	std::cout << xx << std::endl;

    std::cout << ObsidianEngine::Vector2::lerp(xx, xy, 0.5).swizzle<"xxxx">() << std::endl;

    //v.swizzle<"xx">() = ObsidianEngine::Vector2{1,2};

	return EXIT_SUCCESS;
}