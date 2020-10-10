#include "FFT2D.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <array>

GLFWwindow* window;

void initGL(int width, int height, const char* title = "", bool windowVisible = false)
{
    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    if (!windowVisible)
    {
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    }

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);

    if (!window)
    {
        glfwTerminate();
        std::cout << "Failed to create glfw window\n";
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    
    if (!gladLoadGL())
    {
        std::cout << "Failed to initalize OpenGL\n";
        exit(EXIT_FAILURE);
    }
}

void screenshot(int width, int height, const char* filename)
{
    std::vector<uint8_t> data((std::size_t)width * height * 3);

    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data.data());

    stbi_write_png(filename, width, height, 3, data.data(), 0);
}

int main(int argc, char *argv[])
{
    std::array<char*, 5> paths = {};

    {
        int i = 1;

        while (i < argc - 1)
        {
            if (strcmp(argv[i], "-input") == 0)
            {
                paths[0] = argv[i + 1];
            }
            else if (strcmp(argv[i], "-spectrum") == 0)
            {
                paths[1] = argv[i + 1];
            }
            else if (strcmp(argv[i], "-real") == 0)
            {
                paths[2] = argv[i + 1];
            }
            else if (strcmp(argv[i], "-imaginary") == 0)
            {
                paths[3] = argv[i + 1];
            }
            else if (strcmp(argv[i], "-inverse") == 0)
            {
                paths[4] = argv[i + 1];
            }
            else
            {
                std::cout << "Unrecognized option: " << argv[i] << '\n';
                exit(EXIT_FAILURE);
            }

            i += 2;
        }
    }

    if (!paths[0])
    {
        std::cout << "No input image provided\n";
        exit(EXIT_FAILURE);
    }

    initGL(1, 1);

    try
    {
        FFT2D fft(paths[0]);

        fft.foward();

        if(paths[1])
        {
            auto power_spectrum = fft.generatePowerSpectrum();

            power_spectrum.bindAsFrameBuffer();

            screenshot(power_spectrum.get_width(), power_spectrum.get_height(), paths[1]);
        }

        if (paths[2])
        {
            fft.realPart.bindAsFrameBuffer();
            screenshot(fft.realPart.get_width(), fft.realPart.get_height(), paths[2]);
        }

        if (paths[3])
        {
            fft.imaginaryPart.bindAsFrameBuffer();
            screenshot(fft.imaginaryPart.get_width(), fft.imaginaryPart.get_height(), paths[3]);
        }

        if (paths[4])
        {
            fft.inverse();

            fft.originalImage.bindAsFrameBuffer();

            screenshot(fft.originalImage.get_width(), fft.originalImage.get_height(), paths[4]);
        }
    }
    catch (const std::exception& except)
    {
        std::cout << "Exception:\n" << except.what() << '\n';
        exit(EXIT_FAILURE);
    }

    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}
