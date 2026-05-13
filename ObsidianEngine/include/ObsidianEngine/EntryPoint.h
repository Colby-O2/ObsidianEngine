#ifndef ENTRY_POINT_H_
#define ENTRY_POINT_H_

#include "Application.h"
#include "ObsidianEngine/Math/Math.h"

#include <iostream>
#include <stdexcept>
#include <cstdlib>


extern ObsidianEngine::Application* CreateApplication();

using namespace ObsidianEngine;

void printHeader(const std::string& text)
{
    std::cout << "\n--- " << text << " ---\n";
}

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

    //Vector2 v{2, 3};
    //Vector3Double v2{1.2, 2.2, 3.8};

    //Vector4Int r = v2.resizeAs<4, int>();
    //v = r.swizzle<"yx">().cast<float>();

    //auto t = v.resizeAs<3, double>() * v2;

    //std::cout << v << std::endl;
    //std::cout << v2.resize<2>() << std::endl;
    //std::cout << r << std::endl;

    //std::cout << "\n\n\n";

    //Quaternion q1 = Quaternion::fromEuler(10, 20, 30);
    //QuaternionDouble q2 = QuaternionDouble::fromEuler(45, 45, 10);
    //Quaternion q3 = Quaternion::fromEuler(45, 45, 10);

    //Vector3 v3(1, 2, 3);

    //auto r2 = q1 * q2.cast<float>();

    //auto r3 = q1 * v3;
    //auto r4 = q2.cast<float>() * v3;
    //auto r5 = q2 * v3.cast<double>();

    //std::cout << r3 << std::endl;
    //std::cout << r4 << std::endl;
    //std::cout << r5 << std::endl;
    //std::cout << (r4 == r5.cast<float>()) << std::endl;
    //std::cout << "\n\n";
    //std::cout << r2 << std::endl;
    //std::cout << r2.cast<double>() << std::endl;
    //std::cout << q1 * q3 << std::endl;
    //std::cout << (r2 == q1 * q3) << std::endl;

    //std::cout << "\n\n\n";

    //Matirx4x4 m1{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    //Matrix4x4Int m2 { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };

    //auto m3 = m1 * m2.cast<float>();

    //std::cout << m2 << std::endl;
    //std::cout << m3 << std::endl;
    //std::cout << m3.reshape<2, 2>() << std::endl;
    //std::cout << m3.reshapeAs<2, 3, char>() << std::endl;
}

#endif