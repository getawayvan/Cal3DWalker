#pragma once

#include "Window.h"
#include "Shader.h"
#include "Camera.h"
#include "Path.h"
#include "Agent.h"
#include "Interface.h"
#include "Input.h"

namespace walker {

class App {
public:
	// consts
	const GLuint SCREEN_WIDTH;
	const GLuint SCREEN_HEIGHT;
	const GLuint FPS;

public:
	// methods
	App(GLint argc, GLchar** argv);
	GLint run();

private:
	// methods
	GLboolean initScreen();
	GLboolean initShader();
	GLboolean initRender();
	GLboolean initWorld();
	GLboolean initActor();
	GLboolean initInput();

	GLvoid loop();
	GLboolean input(SDL_Event* evnt);
	GLvoid update(const GLfloat deltaTime);
	GLvoid draw();

private:
	// arguments
	GLint m_argc;
	GLchar** m_argv;

	//members
	std::shared_ptr<Window> m_pWindow;
	std::shared_ptr<Shader> m_pShader[2];

	std::shared_ptr<Camera> m_pCamera;
	std::shared_ptr<Plain> m_pSelect;
	std::shared_ptr<Plain> m_pPlain;
	std::shared_ptr<Path> m_pPath;

	std::shared_ptr<Interface> m_pInterface;
	std::shared_ptr<Input> m_pInput;

	GLint m_projMatUniLocBasic;
	GLint m_worldMatUniLocAdvanced;
	GLint m_projMatUniLocAdvanced;

	GLfloat m_deltaTime;
	GLfloat m_keyTime;
};

}