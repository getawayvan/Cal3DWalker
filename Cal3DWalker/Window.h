#pragma once

#include <GL\glew.h>
#include <SDL2\SDL.h>
#include <glm\glm.hpp>
#include <memory>
#include <string>

namespace walker {

class Window {
public:
	Window();
	~Window();

	GLboolean create(const GLchar* windowName, const GLuint width, const GLuint height);
	GLvoid setWindowSize(const GLuint width, const GLuint height);
	GLvoid setBackgroundColor(const GLfloat red = 0.0f, const GLfloat green = 0.0f, const GLfloat blue = 0.0f);

	glm::mat4* getProjMat() const;
	GLfloat getTime(const GLuint maxFPS);
	GLvoid getWindowSize(GLint* width, GLint* height);

	GLvoid clear();
	GLvoid swap();

private:
	SDL_Window* m_pWindow;
	SDL_GLContext m_glContext;

	std::shared_ptr<glm::mat4> m_pProjectionMatrix;
};

}
