// Shader
#include <iostream>
#include <fstream>

#include "Shader.h"

walker::Shader::Shader(const std::string& vertexShader, const std::string& fragmentShader) : m_programID(0) {
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	std::string sShader;
	const GLchar* cShader = nullptr;
	const GLchar** pShader = &cShader;

	sShader = readShaderFile(vertexShader);
	cShader = sShader.c_str();
	glShaderSource(vertexShaderID, 1, pShader, 0);

	sShader = readShaderFile(fragmentShader);
	cShader = sShader.c_str();
	glShaderSource(fragmentShaderID, 1, pShader, 0);

	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);

	// compile shaders
	if (checkShaderStatus(vertexShaderID) && checkShaderStatus(fragmentShaderID)) {
		// create and attach
		m_programID = glCreateProgram();
		glAttachShader(m_programID, vertexShaderID);
		glAttachShader(m_programID, fragmentShaderID);
	}
	else
		std::cout << "Shader program compile fail" << std::endl;

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
}

GLvoid walker::Shader::bindAttribLocation(const ShdAttLoc shaderType) {
	// set attributes order
	switch (shaderType) {
	case ShdAttLoc::BASIC:
		glBindAttribLocation(m_programID, 0, "inPosition");
		glBindAttribLocation(m_programID, 1, "inColor");
		break;

	case ShdAttLoc::ADVANCED:
		glBindAttribLocation(m_programID, 0, "inPosition");
		glBindAttribLocation(m_programID, 1, "inNormal");
		glBindAttribLocation(m_programID, 2, "inTexCoord");
		break;

	default:
		std::cout << "shader type not available" << std::endl;
		break;
	}
}

GLboolean walker::Shader::linkProgram() {
	glLinkProgram(m_programID);
	return checkProgramStatus(m_programID);
}

GLuint walker::Shader::getProgramID() const {
	return m_programID;
}

GLvoid walker::Shader::useProgram() const {
	glUseProgram(m_programID);
}

std::string walker::Shader::readShaderFile(const std::string& filePath) {
	std::ifstream input(filePath);
	if (!input.good()) {
		std::cout << "Shader file load fail: " << filePath << std::endl;
		exit(1);
	}

	return std::string(std::istreambuf_iterator<char>(input),
		std::istreambuf_iterator<char>());
}

GLboolean walker::Shader::checkShaderStatus(GLuint shaderID) {
	return checkStatus(shaderID, glGetShaderiv, glGetShaderInfoLog, GL_COMPILE_STATUS);
}

GLboolean walker::Shader::checkProgramStatus(GLuint programID) {
	return checkStatus(programID, glGetProgramiv, glGetProgramInfoLog, GL_LINK_STATUS);
}

GLboolean walker::Shader::checkStatus(GLuint objectID, PFNGLGETSHADERIVPROC objectPropertyGetterFunc,
	PFNGLGETSHADERINFOLOGPROC getInfoLogFunc, GLenum statusType) {
	GLint status;
	objectPropertyGetterFunc(objectID, statusType, &status);
	if (status != GL_TRUE) {
		GLint infoLogLength;
		objectPropertyGetterFunc(objectID, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* buffer = new GLchar[infoLogLength];

		GLsizei bufferSize;
		getInfoLogFunc(objectID, infoLogLength, &bufferSize, buffer);
		std::cout << buffer << std::endl;

		delete[] buffer;
		return false;
	}

	return true;
}
