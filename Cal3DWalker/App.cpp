// App
#include <Windows.h>
#include <glm\glm.hpp>
#include <cal3d\cal3d.h>

#include "Model.h"
#include "Loader.h"
#include "Gui.h"
#include "App.h"

walker::App::App(GLint argc, GLchar** argv) :
	SCREEN_WIDTH(800),
	SCREEN_HEIGHT(600),
	FPS(25),
	m_argc(argc),
	m_argv(argv),
	m_projMatUniLocBasic(0),
	m_worldMatUniLocAdvanced(0),
	m_projMatUniLocAdvanced(0),
	m_deltaTime(0.0f),
	m_keyTime(0.0f) {}

GLint walker::App::run() {
	// hide console
	//::ShowWindow(::GetConsoleWindow(), SW_HIDE);

	// app initialization
	GLboolean init = false;

	// initialize all
	init = initScreen()
		&& initShader()
		&& initRender()
		&& initWorld()
		&& initActor()
		&& initInput();

	// app
	if (init)
		loop();

	// show console
	//::ShowWindow(::GetConsoleWindow(), SW_SHOW);

	// end
	return (GLint)!init;
}

GLboolean walker::App::initScreen() {
	// version
	const std::string walkerVersion("Cal3D Walker v2.5");
	std::cout << " --- " << walkerVersion << " ---" << std::endl;

	// SDL OpenGL window
	m_pWindow = std::make_shared<Window>();
	if (!m_pWindow->create(walkerVersion.c_str(), SCREEN_WIDTH, SCREEN_HEIGHT))
		return false;

	// background color
	m_pWindow->setBackgroundColor(0.5f, 0.5f, 0.5f);

	// screen
	return true;
}

GLboolean walker::App::initShader() {
	// shader basic
	m_pShader[0] = std::make_shared<Shader>("../shaders/VertShadBasic.glsl", "../shaders/FragShadBasic.glsl");
	walker::Shader* pShader = m_pShader[0].get();
	if (!pShader)
		return false;

	pShader->bindAttribLocation(ShdAttLoc::BASIC);
	if (!pShader->linkProgram()) {
		std::cout << "Shader program 'Basic' link fail" << std::endl;
		return false;
	}

	// projection matrix
	pShader->useProgram();
	m_projMatUniLocBasic = glGetUniformLocation(pShader->getProgramID(), "modelToProjectionMatrix");

	// shader advanced
	m_pShader[1] = std::make_shared<Shader>("../shaders/VertShadAdvanced.glsl", "../shaders/FragShadAdvanced.glsl");
	pShader = m_pShader[1].get();
	if (!pShader)
		return false;

	pShader->bindAttribLocation(ShdAttLoc::ADVANCED);
	if (!pShader->linkProgram()) {
		std::cout << "Shader program 'Advanced' link fail" << std::endl;
		return false;
	}

	// projection matrix
	pShader->useProgram();
	m_worldMatUniLocAdvanced = glGetUniformLocation(pShader->getProgramID(), "modelToWorldMatrix");
	m_projMatUniLocAdvanced = glGetUniformLocation(pShader->getProgramID(), "modelToProjectionMatrix");

	// shader
	return true;
}

GLboolean walker::App::initRender() {
	// light Position
	glm::vec3 lightPositionWorld(3.0f, -5.0f, 5.0f);
	GLint lightPositionWorldUniformLocation = glGetUniformLocation(m_pShader[1]->getProgramID(), "lightPositionWorld");
	glUniform3fv(lightPositionWorldUniformLocation, 1, &lightPositionWorld[0]);

	// diffuse light
	glm::vec3 diffuseLight(0.93f, 0.85f, 0.77f);
	GLint diffuseLightUniformLocation = glGetUniformLocation(m_pShader[1]->getProgramID(), "diffuseLight");
	glUniform3fv(diffuseLightUniformLocation, 1, &diffuseLight[0]);

	// specular light
	glm::vec3 specularLight(0.29f, 0.25f, 0.21f);
	GLint specularLightUniformLocation = glGetUniformLocation(m_pShader[1]->getProgramID(), "specularLight");
	glUniform3fv(specularLightUniformLocation, 1, &specularLight[0]);

	// ambient light
	glm::vec3 ambientLight(0.37f, 0.35f, 0.33f);
	GLint ambientLightUniformLocation = glGetUniformLocation(m_pShader[1]->getProgramID(), "ambientLight");
	glUniform3fv(ambientLightUniformLocation, 1, &ambientLight[0]);

	// gui setup
	m_pInterface = std::make_shared<Interface>();
	if (!m_pInterface->setup())
		return false;

	// start to render
	m_pWindow->clear();
	walker::Gui::instance()->draw();
	m_pWindow->swap();

	// render
	return true;
}

GLboolean walker::App::initWorld() {
	// camera
	m_pCamera = std::make_shared<Camera>();
	m_pCamera->setPosition(glm::vec3(-1.25f, -2.1f, 1.6f));
	m_pCamera->setViewDirection(glm::vec3(0.315f, 0.485f, -0.2f));

	// plain, path & selection
	m_pSelect = std::make_shared<Plain>();
	m_pPlain = std::make_shared<Plain>();
	m_pPath = std::make_shared<Path>();

	// world
	return true;
}

GLboolean walker::App::initActor() {
	// load models
	std::shared_ptr<Loader> pConfig = std::make_shared<Loader>(m_pWindow.get());
	if (!pConfig->load("../config"))
		return false;

	// collect models
	std::vector<std::string> coreNames;
	if (!walker::Model::instance()->getCoreModelList(coreNames))
		return false;

	// add models
	for (auto& coreName : coreNames) {
		// create new model instance
		Agent* pAgent = m_pInterface->addAgent();
		if (!pAgent->init(coreName))
			return false;
	}

	// actor
	return true;
}

GLboolean walker::App::initInput() {
	// input
	m_pInput = std::make_shared<Input>(m_pWindow->getProjMat(),
		m_pCamera.get(), m_pPath.get(), SCREEN_WIDTH, SCREEN_HEIGHT);

	// set the default agent for input
	m_pInput->setAgent(m_pInterface->setAgent());

	// gui init
	return m_pInterface->initBackground()
		&& m_pInterface->initHelp()
		&& m_pInterface->initSelection()
		&& m_pInterface->initBasic()
		&& m_pInterface->initAdvanced()
		&& m_pInterface->initTotal()
		&& m_pInterface->initMixer()
		&& m_pInterface->fill();
}

GLvoid walker::App::loop() {
	// event handler
	SDL_Event evnt;
	while (input(&evnt)) {
		// process
		update(m_deltaTime);

		// render begin
		m_pWindow->clear();

		// draw
		draw();

		// timer update
		m_deltaTime = m_pWindow->getTime(FPS);
		if (m_keyTime > 0.0f) {
			m_deltaTime -= m_keyTime;
			m_keyTime = 0.0f;
		}

		// end of render
		m_pWindow->swap();
	}
}

GLboolean walker::App::input(SDL_Event* evnt) {
	// input events
	while (SDL_PollEvent(evnt)) {
		// input
		switch (evnt->type) {
		case SDL_MOUSEMOTION:
			m_pInput->mouseMotion(evnt->motion, m_pInterface->getWorkMode());
			break;

		case SDL_MOUSEBUTTONDOWN:
			m_pInput->mouseDown(evnt->button, m_pInterface->getWorkMode());
			break;

		case SDL_MOUSEBUTTONUP:
			m_pInput->mouseUp(evnt->button, m_pInterface->getWorkMode());
			break;

		case SDL_KEYDOWN: {
			// confirm model for input
			m_pInput->setAgent(m_pInterface->getAgent());

			Uint32 keyTicksA = SDL_GetTicks();
			if (!m_pInput->keyDown(evnt->key.keysym.sym, m_pInterface->getWorkMode()))
				return false;

			Uint32 keyTicksB = SDL_GetTicks();
			m_keyTime = (GLfloat)(keyTicksB - keyTicksA) / 1000;
			break;
		}

		case SDL_KEYUP:
			m_pInput->keyUp(evnt->key.keysym.sym, m_pInterface->getWorkMode());
			break;

		case SDL_WINDOWEVENT:
			if (evnt->window.event == SDL_WINDOWEVENT_RESIZED) {
				m_pWindow->setWindowSize(evnt->window.data1, evnt->window.data2);
				m_pInput->setScreenSize(evnt->window.data1, evnt->window.data2);
			}
			break;

		case SDL_QUIT:
			// quit app
			return false;

		default:
			break;
		}

		// gui
		walker::Gui::instance()->onSDLEvent(*evnt);
	}

	// processed
	return true;
}

GLvoid walker::App::update(const GLfloat deltaTime) {
	// camera
	if (m_pInput->getCamUpdate() != glm::vec3(0.0f, 0.0f, 0.0f))
		m_pCamera->setPosition(m_pCamera->getPosition() + m_pInput->getCamUpdate());

	// currently active model instance
	Agent* pAgent = m_pInterface->getAgent();
	if (pAgent) {
		// active CalModel
		CalModel* pCalModel = pAgent->getBaseModel();
		const CalCoreModel* pCoreModel = pCalModel->getCoreModel();

		// requesting next scenario event (important for time tracking)
		const ScenarioInfo* pScInfo = walker::Model::instance()->getScenarioInfo(pCoreModel, deltaTime);
		if (pScInfo) {
			// apply scenario event
			if (!pAgent->play(pScInfo))
				return;

			// remove applied scenario event
			if (!walker::Model::instance()->removeScenarioInfo(pCoreModel, pScInfo))
				return;
		}

		// model on path update
		if (pAgent->getIsPath()) {
			// path point
			const std::vector<glm::vec2>& path = m_pPath->pathPoint;
			// Length of the local path
			GLfloat& length = m_pPath->m_currentPathLength;
			// current position
			glm::vec2 locPos = glm::vec2(pAgent->getPosition().x, pAgent->getPosition().y);

			// counter
			size_t count = 0;
			// local path length
			GLfloat curvePathLength = 0.0f;
			// Loop through the curve points (at least two)
			while (count < path.size() - 1) {
				// Path length of the active curve point
				curvePathLength += glm::distance(path[count], path[count + 1]);

				// Looking for next curve point
				if (length < curvePathLength) {
					// Local orientation vector
					glm::vec2 locDir = glm::normalize(path[count + 1] - glm::vec2(locPos.x, locPos.y));

					// Remember local position for the next frame
					glm::vec2 prePos = locPos;

					// Actual position
					locPos.x += locDir.x * pAgent->getSpeed() * deltaTime;
					locPos.y += locDir.y * pAgent->getSpeed() * deltaTime;

					// Confirm that model is (relatively) on the path
					if (glm::distance(locPos, path[count + 1]) < glm::distance(path[count], path[count + 1]))
						// Update local path length
						length += glm::distance(locPos, prePos);

					// Update position and direction
					pAgent->setPosition(glm::vec3(locPos.x, locPos.y, 0.0f));
					pAgent->setDirection(glm::vec3(locDir.x, locDir.y, 0.0f));

					// end
					break;
				}

				// count
				++count;
			}

			// loop path
			if (count == path.size() - 1) {
				pAgent->setPosition(glm::vec3(path[0].x, path[0].y, 0.0f));
				glm::vec2 firstDir = glm::normalize(path[1] - path[0]);
				pAgent->setDirection(glm::vec3(firstDir.x, firstDir.y, 0.0f));
				length = 0.0f;
			}
		}

		// coordinates of model eye focus
		const glm::vec3 look = m_pInput->getDirPos() + glm::vec3(0.0f, 0.0f, m_pInterface->getLookHeight());

		// update active CalModel
		pAgent->update(look, deltaTime);
	}

	// gui
	walker::Gui::instance()->update();
}

GLvoid walker::App::draw() {
	// Local
	static glm::mat4 traRotMat;
	static glm::mat4 transformMatrix;

	static GLfloat plainColor[] = { 0.57f, 0.65f, 0.77f };
	static GLfloat vert[] = {
		// position          // color
		-1.0f, -1.0f, +0.0f, plainColor[0], plainColor[1], plainColor[2],
		+1.0f, -1.0f, +0.0f, plainColor[0], plainColor[1], plainColor[2],
		+1.0f, +1.0f, +0.0f, plainColor[0], plainColor[1], plainColor[2],
		-1.0f, +1.0f, +0.0f, plainColor[0], plainColor[1], plainColor[2]
	};

	// grid
	glLineWidth(1.0f);
	m_pShader[0]->useProgram();
	for (GLint i = -9; i < 10; ++i) {
		for (GLint j = -9; j < 10; ++j) {
			traRotMat = glm::translate(glm::vec3(i, j, 0));

			transformMatrix = *m_pWindow->getProjMat() * m_pCamera->getWorldToViewMatrix() * traRotMat;
			glUniformMatrix4fv(m_projMatUniLocBasic, 1, GL_FALSE, &transformMatrix[0][0]);

			if (m_pPlain) m_pPlain->draw(vert);
		}
	}

	// path
	if (m_pPath && !m_pPath->checkPoint.empty()) {
		// Update transformation matrix
		traRotMat = glm::translate(glm::vec3(0.0f, 0.0f, 0.0f));

		transformMatrix = *m_pWindow->getProjMat() * m_pCamera->getWorldToViewMatrix() * traRotMat;
		glUniformMatrix4fv(m_projMatUniLocBasic, 1, GL_FALSE, &transformMatrix[0][0]);

		GLuint tmp1 = 0, tmp2 = 0;
		for (size_t i = 0; i < m_pPath->checkPoint.size(); ++i) {
			if (i % 3 == 0)
				++tmp1;
			else
				++tmp2;
		}

		// Draw checkpoints
		glPointSize(11.0f);
		glBindVertexArray(m_pPath->vao[0]);
		glDrawArrays(GL_POINTS, 0, tmp1);

		glPointSize(8.0f);
		glBindVertexArray(m_pPath->vao[1]);
		glDrawArrays(GL_POINTS, 0, tmp2);

		if (m_pPath->checkPoint.size() > 1) {
			for (size_t i = 0; i < m_pPath->checkPoint.size(); ++i) {
				if (i % 3 == 0) {
					GLint first;
					GLsizei count;

					if (i == 0) {
						first = 0;
						count = 2;
					}
					else if (i == m_pPath->checkPoint.size() - 1) {
						first = i - 1;
						count = 2;
					}
					else {
						first = i - 1;
						count = 3;
					}

					glBindVertexArray(m_pPath->vao[2]);
					glDrawArrays(GL_LINE_STRIP, first, count);
				}
			}
		}

		if (!m_pPath->pathPoint.empty()) {
			// Draw path
			glPointSize(3.0f);
			glBindVertexArray(m_pPath->vao[3]);
			glDrawArrays(GL_POINTS, 0, m_pPath->pathPoint.size());
		}
	}

	// Model
	Agent* pAgent = m_pInterface->getAgent();
	if (pAgent) {
		// CalModel
		CalModel* pCalModel = pAgent->getBaseModel();

		// Update transformation matrix
		m_pShader[1]->useProgram();
		traRotMat = glm::translate(pAgent->getPosition()) * glm::rotate(pAgent->getRotation(), glm::vec3(0.0f, 0.0f, 1.0f));

		transformMatrix = *m_pWindow->getProjMat() * m_pCamera->getWorldToViewMatrix() * traRotMat;
		glUniformMatrix4fv(m_worldMatUniLocAdvanced, 1, GL_FALSE, &traRotMat[0][0]);
		glUniformMatrix4fv(m_projMatUniLocAdvanced, 1, GL_FALSE, &transformMatrix[0][0]);

		// Set model for drawing
		glEnable(GL_DEPTH_TEST);
		glBindVertexArray(walker::Model::instance()->getVertexArrayID());

		// Draw model
		pAgent->draw(m_pInterface->getRendMode(), m_pShader[1]->getProgramID());

		glBindVertexArray(0);
	}

	// gui
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	m_pShader[0]->useProgram();
	glActiveTexture(GL_TEXTURE0);
	walker::Gui::instance()->draw();
}
