// Window
#include <glm\gtx\transform.hpp>
#include <iostream>

#include "Window.h"

walker::Window::Window() : m_pWindow(nullptr) {
	// SDL init
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
}

walker::Window::~Window() {
	SDL_GL_DeleteContext(m_glContext);
	SDL_DestroyWindow(m_pWindow);
	SDL_Quit();
}

GLboolean walker::Window::create(const GLchar* windowName, const GLuint width, const GLuint height) {
	// Create a window
	m_pWindow = SDL_CreateWindow(windowName, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (!m_pWindow) {
		std::cout << "error: SDL window create fail" << std::endl;
		return false;
	}

	// Create an OpenGL context
	m_glContext = SDL_GL_CreateContext(m_pWindow);
	if (!m_glContext) {
		std::cout << "error: SDL_GL context create fail" << std::endl;
		return false;
	}

	// Glew init
	GLenum glError = glewInit();
	if (glError != GLEW_OK) {
		std::cout << "error: glew init fail" << std::endl;
		return false;
	}

	// Turn on V-SYNC
	SDL_GL_SetSwapInterval(0);

	// Global OpenGL states
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// projection matrix
	m_pProjectionMatrix = std::make_shared<glm::mat4>(glm::perspective(glm::radians(60.0f), (GLfloat)width / height, 0.1f, 100.0f));

	// window created
	return true;
}

GLvoid walker::Window::setWindowSize(const GLuint width, const GLuint height) {
	SDL_SetWindowSize(m_pWindow, width, height);
	glViewport(0, 0, width, height);

	*m_pProjectionMatrix = glm::perspective(glm::radians(60.0f), (GLfloat)width / height, 0.1f, 100.0f);
}

GLvoid walker::Window::setBackgroundColor(const GLfloat red, const GLfloat green, const GLfloat blue) {
	glClearColor(red, green, blue, 1.0f);
}

glm::mat4* walker::Window::getProjMat() const {
	return m_pProjectionMatrix.get();
}

GLfloat walker::Window::getTime(const GLuint maxFPS) {
	// static valriables
	static const Uint16 MIL_SEC = 1000;
	static Uint32 lastTick = 0;

	// calculate the amount of elapsed seconds
	Uint32 frameTicks = SDL_GetTicks() - lastTick;
	GLfloat elapsedSeconds = (GLfloat)frameTicks / MIL_SEC;

	// Hold FPS at MAX_FPS
	if ((Uint32)(MIL_SEC / maxFPS) > frameTicks) {
		Uint32 delay = (Uint32)(MIL_SEC / maxFPS - frameTicks);
		elapsedSeconds += (GLfloat)delay / MIL_SEC;
		SDL_Delay(delay);
	}

	// Current tick will be last tick next round
	lastTick = SDL_GetTicks();

	return elapsedSeconds;
}

GLvoid walker::Window::getWindowSize(GLint* width, GLint* height) {
	SDL_GetWindowSize(m_pWindow, width, height);
}

GLvoid walker::Window::clear() {
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

GLvoid walker::Window::swap() {
	SDL_GL_SwapWindow(m_pWindow);
}
