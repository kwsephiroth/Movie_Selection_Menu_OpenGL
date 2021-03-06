#ifndef SHADER_H
#define SHADER_H
#include <GL/glew.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cassert>

namespace DSS
{
	class Shader
	{
	public:
		GLuint Program;

		Shader(const char* vShaderCode, const char* fShaderCode)
		{
			compile_shaders(vShaderCode, fShaderCode);
		}

		void compile_shaders(const char* vShaderCode, const char* fShaderCode)
		{
			//Compile shaders
			GLuint vertex, fragment;
			GLint success;
			GLchar infoLog[512];
			// Vertex Shader
			vertex = glCreateShader(GL_VERTEX_SHADER);
			glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertex, 1, &vShaderCode, NULL);
			glCompileShader(vertex);
			// Print compile errors if any
			glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(vertex, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
				//assert(success);
			}
			// Fragment Shader
			fragment = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragment, 1, &fShaderCode, NULL);
			glCompileShader(fragment);
			// Print compile errors if any
			glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(fragment, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
				//assert(success);
			}
			// Shader Program
			this->Program = glCreateProgram();
			glAttachShader(this->Program, vertex);
			glAttachShader(this->Program, fragment);
			glLinkProgram(this->Program);
			// Print linking errors if any
			glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
				//assert(success);
			}
			// Delete the shaders as they're linked into our program now and no longer necessery
			glDeleteShader(vertex);
			glDeleteShader(fragment);
		}

		// Uses the current shader
		void Use()
		{
			glUseProgram(this->Program);
		}
	};
}
#endif