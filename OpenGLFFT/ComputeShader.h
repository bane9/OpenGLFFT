#pragma once

#include "glad.h"
#include "checkGl.h"

#include <fstream>
#include <string>
#include <string_view>
#include <stdexcept>

class ComputeShader
{
public:
	ComputeShader() = delete;

	ComputeShader(const char* src, bool isAPath = true)
	{
		const char* src_raw = src;

		if (isAPath)
		{
			std::ifstream programSource(src);

			if (!programSource)
			{
				throw std::runtime_error("Cannot open shader source file");
			}

			std::string computeShaderSrcCode((std::istreambuf_iterator<char>(programSource)),
				std::istreambuf_iterator<char>());

			src_raw = computeShaderSrcCode.c_str();
		}

		loadPorgram(src_raw);
	}

	ComputeShader(const ComputeShader&) = delete;
	ComputeShader& operator=(const ComputeShader&) = delete;

	ComputeShader(ComputeShader&& rhs) noexcept
	{
		*this = std::move(rhs);
	}

	ComputeShader& operator=(ComputeShader&& rhs) noexcept
	{
		program = rhs.program;
		shader = rhs.shader;

		rhs.program = 0;
		rhs.shader = 0;
	}

	~ComputeShader()
	{
		if (program)
		{
			glDeleteProgram(program);
		}

		if (shader)
		{
			glDeleteShader(shader);
		}
	}

	void bindUniform(const char* uniformName, GLuint slot)
	{
		glCheck(glUniform1i(glGetUniformLocation(program, uniformName), slot));
	}
	
	void invoke(int x = 1, int y = 1, int z = 1, bool blocking = true)
	{
		glCheck(glDispatchCompute(x, y, z));

		if (blocking)
		{
			glCheck(glMemoryBarrier(GL_ALL_BARRIER_BITS));
		}
	}

private:
	void loadPorgram(const char* src_raw)
	{
		program = glCreateProgram();

		shader = glCreateShader(GL_COMPUTE_SHADER);
		glCheck();
		glCheck(glShaderSource(shader, 1, &src_raw, NULL));
		glCheck(glCompileShader(shader));

		int ret = 0;
		glCheck(glGetShaderiv(shader, GL_COMPILE_STATUS, &ret));

		if (!ret)
		{
			int length;
			glCheck(glGetShaderInfoLog(shader, 0, &length, nullptr));

			std::string errBuffer(length, '\0');

			glCheck(glGetShaderInfoLog(shader, errBuffer.size(), &length, const_cast<char*>(errBuffer.c_str())));

			auto err = "Error: Compiler log:\n" + errBuffer;

			throw std::runtime_error(err);
		}

		glCheck(glAttachShader(program, shader));
		glCheck(glLinkProgram(program));

		glCheck(glGetProgramiv(program, GL_LINK_STATUS, &ret));

		if (!ret)
		{
			int length;
			glCheck(glGetProgramInfoLog(program, 0, &length, nullptr));

			std::string errBuffer(length, '\0');

			glCheck(glGetProgramInfoLog(program, errBuffer.size(), &length, const_cast<char*>(errBuffer.c_str())));

			auto err = "Error: Linker log:\n" + errBuffer;

			throw std::runtime_error(err);
		}

		glCheck(glUseProgram(program));
	}

private:
	GLuint program;
	GLuint shader;
};