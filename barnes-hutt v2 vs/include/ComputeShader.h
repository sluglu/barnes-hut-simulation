#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


class ComputeShader
{
public:
    unsigned int ID;
    ComputeShader()
    {
    }
    ComputeShader(const char* computePath)
    {
        // 1. retrieve the vertex/fragment source code from filePath
        std::string ComputeCode;
        std::ifstream cShaderFile;
        // ensure ifstream objects can throw exceptions:
        cShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // open files
            cShaderFile.open(computePath);
            std::stringstream cShaderStream;
            // read file's buffer contents into streams
            cShaderStream << cShaderFile.rdbuf();
            // close file handlers
            cShaderFile.close();
            // convert stream into string
            ComputeCode = cShaderStream.str();
        }
        catch (std::ifstream::failure e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        }
        const char* cShaderCode = ComputeCode.c_str();
        // 2. compile shaders
        unsigned int compute;
        int success;
        char infoLog[512];

        // compute shader
        compute = glCreateShader(GL_COMPUTE_SHADER);
        glShaderSource(compute, 1, &cShaderCode, NULL);
        glCompileShader(compute);

        // shader Program
        ID = glCreateProgram();
        glAttachShader(ID, compute);
        glLinkProgram(ID);


        // print compile errors if any
        glGetShaderiv(compute, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(compute, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        };
        // shader Program
        ID = glCreateProgram();
        glAttachShader(ID, compute);
        glLinkProgram(ID);
        // print linking errors if any
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(ID, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }

        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(compute);
    }

    void use()
    {
        glUseProgram(ID);
    }
    void setBool(const std::string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    void setInt(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    void setFloat(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
};