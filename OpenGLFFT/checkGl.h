#pragma once

#include "glad.h"
#include <sstream>
#include <iostream>

void checkGlError(int line, const char* file)
{
    auto errTranslation = [](GLenum err)
    {
        switch (err)
        {
        case GL_NO_ERROR:
            return "GL_NO_ERROR";
        case GL_INVALID_ENUM:
            return "GL_INVALID_ENUM";
        case GL_INVALID_VALUE:
            return "GL_INVALID_VALUE";
        case GL_INVALID_OPERATION:
            return "GL_INVALID_OPERATION";
        case GL_STACK_OVERFLOW:
            return "GL_STACK_OVERFLOW";
        case GL_STACK_UNDERFLOW:
            return "GL_STACK_UNDERFLOW";
        case GL_OUT_OF_MEMORY:
            return "GL_OUT_OF_MEMORY";
        case GL_TABLE_TOO_LARGE:
            return "GL_TABLE_TOO_LARGE";
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            return "GL_INVALID_FRAMEBUFFER_OPERATION";
        default:
            return "Unkown openGL Error";
        }
    };

    GLenum err;
    std::stringstream ss;
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        ss << "File: " << file <<"\nLine " << line << ":\n" << errTranslation(err) << '\n';
    }

    auto str = ss.str();

    if (!str.empty())
    {
        std::cout << str;
        exit(EXIT_FAILURE);
    }
}

#define glCheck(e) do {e; checkGlError(__LINE__, __FILE__); } while(false)
