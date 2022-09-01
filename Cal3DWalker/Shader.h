#pragma once

#include <GL\glew.h>
#include <string>

namespace walker {

enum class ShdAttLoc {
	BASIC,
	ADVANCED
};

class Shader {
public:
	Shader(const std::string& vertexShader, const std::string& fragmentShader);

	GLvoid bindAttribLocation(const ShdAttLoc shaderType);
	GLboolean linkProgram();
	GLuint getProgramID() const;
	GLvoid useProgram() const;

private:
	std::string readShaderFile(const std::string& filePath);
	GLboolean checkShaderStatus(GLuint shaderID);
	GLboolean checkProgramStatus(GLuint programID);
	GLboolean checkStatus(GLuint objectID, PFNGLGETSHADERIVPROC objectPropertyGetterFunc,
		PFNGLGETSHADERINFOLOGPROC getInfoLogFunc, GLenum statusType);

private:
	GLuint m_programID;
};

}