#ifndef ENTRY_POINT_H_
#define ENTRY_POINT_H_

#include "Application.h"
#include "Matrix.h"

#include <iostream>
#include <stdexcept>
#include <cstdlib>

extern ObsidianEngine::Application* CreateApplication();

using namespace ObsidianEngine;

int main()
{
    //try
    //{
    //    ObsidianEngine::Application* app = CreateApplication();
    //    app->run();
    //    delete app;
    //}
    //catch (const std::exception& e)
    //{
    //    std::cerr << e.what() << std::endl;
    //    return EXIT_FAILURE;
    //}

    Matrix4x4 m{
        {2, 34, 4, 24},
        {43, 5, 6, 7},
        {3, 35, 62, 22},
        {2, 33, 67, 40}
    };

    //std::cout << m.perspective(90, 0.8, 0.4, 100) << std::endl;
    std::cout << m.getRow(0) << std::endl;
        
    return EXIT_SUCCESS;
}

#endif