#include "Shader.hpp"

namespace gps {
    std::string Shader::readShaderFile(std::string fileName)
    {
        std::ifstream shaderFileHandle;
        std::string shaderContent;

        //attempt to open shader file
        shaderFileHandle.open(fileName);

        std::stringstream shaderBuffer;

        //extract shader content
        shaderBuffer << shaderFileHandle.rdbuf();

        //terminate shader file access
        shaderFileHandle.close();

        //store content as string
        shaderContent = shaderBuffer.str();
        return shaderContent;
    }

    void Shader::shaderCompileLog(GLuint shaderIdentifier)
    {
        GLint status;
        GLchar logDetails[512];

        //verify compilation status
        glGetShaderiv(shaderIdentifier, GL_COMPILE_STATUS, &status);
        if (!status)
        {
            glGetShaderInfoLog(shaderIdentifier, 256, NULL, logDetails);
            std::cout << "Shader compilation error\n" << infoLog << std::endl;
        }
    }

    void Shader::shaderLinkLog(GLuint shaderProgramIdentifier)
    {
        GLint linkStatus;
        GLchar linkDetails[256];

        //check program linkage
        glGetProgramiv(shaderProgramIdentifier, GL_LINK_STATUS, &linkStatus);
        if (!linkStatus) {
            glGetProgramInfoLog(shaderProgramIdentifier, 256, NULL, linkDetails);
            std::cout << "Shader linking error\n" << infoLog << std::endl;
        }
    }

    void Shader::loadShader(std::string vertexFile, std::string fragmentFile)
    {
        //retrieve and compile vertex shader
        std::string vertexCode = readShaderFile(vertexFile);
        const GLchar* vertexString = vertexCode.c_str();
        GLuint vertexShaderID;
        vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShaderID, 1, &vertexString, NULL);
        glCompileShader(vertexShaderID);
        shaderCompileLog(vertexShaderID);

        //retrieve and compile fragment shader
        std::string fragmentCode = readShaderFile(fragmentFile);
        const GLchar* fragmentString = fragmentCode.c_str();
        GLuint fragmentShaderID;
        fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShaderID, 1, &fragmentString, NULL);
        glCompileShader(fragmentShaderID);
        shaderCompileLog(fragmentShaderID);

        //assemble and validate program
        this->shaderProgram = glCreateProgram();
        glAttachShader(this->shaderProgram, vertexShaderID);
        glAttachShader(this->shaderProgram, fragmentShaderID);
        glLinkProgram(this->shaderProgram);
        glDeleteShader(vertexShaderID);
        glDeleteShader(fragmentShaderID);
        shaderLinkLog(this->shaderProgram);
    }

    void Shader::useShaderProgram()
    {
        glUseProgram(this->shaderProgram);
    }
}
