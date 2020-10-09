#pragma once

#include "glad.h"
#include "checkGl.h"

class SSBO
{
public:

    template<typename T>
    SSBO(const T& t, GLuint binding)
    {

        glCheck(glGenBuffers(1, &ssbo));

        glCheck(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, ssbo));

        glCheck(glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(T), &t, GL_DYNAMIC_DRAW));
    }

    SSBO(const SSBO&) = delete;
    SSBO& operator=(const SSBO&) = delete;

    SSBO(SSBO&& rhs) noexcept
    {
        *this = static_cast<SSBO&&>(rhs);
    }

    SSBO& operator=(SSBO&& rhs) noexcept
    {
        ssbo = rhs.ssbo;
        rhs.ssbo = 0;
    }

    ~SSBO()
    {
        if (ssbo)
        {
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
            glDeleteBuffers(1, &ssbo);
        }
    }

private:
    GLuint ssbo = 0;
};
