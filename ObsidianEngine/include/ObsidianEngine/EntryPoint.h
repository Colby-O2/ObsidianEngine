#ifndef ENTRY_POINT_H_
#define ENTRY_POINT_H_

#include "Application.h"

#include <iostream>
#include <stdexcept>
#include <cstdlib>


extern ObsidianEngine::Application* CreateApplication();

using namespace ObsidianEngine;

int main()
{
    try
    {
        ObsidianEngine::Application* app = CreateApplication();
        app->run();
        delete app;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

#endif