// Input
#include <algorithm>
#include <iostream>
#include <vector>
#include <cal3d\cal3d.h>

#include "Input.h"
#include "Window.h"
#include "Camera.h"
#include "Path.h"
#include "Model.h"
#include "Agent.h"
#include "Interface.h"

walker::Input::Input(glm::mat4* proj, Camera* camera, Path* path, GLuint width, GLuint height) :
	m_pProjMat(proj),
	m_pCamera(camera),
	m_pPath(path),
	m_pAgent(nullptr),
	m_width(width),
	m_height(height),
	m_isCamRot(false),
	m_isCamAxi(false) {}

GLvoid walker::Input::setScreenSize(const GLuint width, const GLuint height) {
	m_width = width;
	m_height = height;
}

GLvoid walker::Input::setAgent(Agent* pAgent) {
	m_pAgent = pAgent;
}

glm::vec3 walker::Input::getCamUpdate() const {
	return m_camUpdate;
}

glm::vec3 walker::Input::getDirPos() const {
	return m_dirPos;
}

GLvoid walker::Input::mouseMotion(SDL_MouseMotionEvent mme, WorkMode mode) {
	// position at view direction
	m_dirPos = glmUnProject(mme.x, mme.y);

	if (mode == WorkMode::CAMERA && m_isCamRot) {
		const GLfloat CEN_X = (GLfloat)m_width / 2.0f;
		const GLfloat CEN_Y = (GLfloat)m_height / 2.0f;
		if (m_isCamAxi) {
			const GLfloat NEW_X = CEN_X - 0.01f * (mme.x - m_camRot.x);
			const GLfloat CAM_Z = m_pCamera->getPosition().z;
			m_pCamera->setPosition(glm::vec3(m_radius * std::cos(NEW_X), m_radius * std::sin(NEW_X), CAM_Z));
			glm::vec3 camDirPos = m_pAgent->getPosition() - m_pCamera->getPosition() + glm::vec3(0.0f, 0.0f, 1.0f);
			m_pCamera->setViewDirection(camDirPos);
		}
		else {
			const GLuint NEW_X = (GLuint)(CEN_X - 0.1f * (mme.x - m_camRot.x));
			const GLuint NEW_Y = (GLuint)(CEN_Y - 0.1f * (mme.y - m_camRot.y));
			glm::vec3 newDirPos = glmUnProject(NEW_X, NEW_Y);
			m_pCamera->setViewDirection(newDirPos - m_pCamera->getPosition());
		}
	}
	else
		m_camRot = glm::ivec2(mme.x, mme.y);

	if (mode == WorkMode::PATH && m_pPoint && m_pPath) {
		*m_pPoint = glm::vec2(m_dirPos.x, m_dirPos.y);

		m_pPath->pathPoint.clear();
		std::vector<glm::vec2> tmpVec;
		for (size_t i = 0; i < m_pPath->checkPoint.size(); ++i) {
			if (i == 0) {
				m_pPath->pathPoint.push_back(m_pPath->checkPoint.front());
				tmpVec.push_back(m_pPath->checkPoint[i]);
				continue;
			}

			tmpVec.push_back(m_pPath->checkPoint[i]);

			if (tmpVec.size() == 4) {
				tmpVec = m_pPath->curve.getPathProfile(tmpVec, 1.0f);
				m_pPath->pathPoint.insert(m_pPath->pathPoint.end(), ++tmpVec.begin(), tmpVec.end());

				tmpVec.clear();
				tmpVec.push_back(m_pPath->checkPoint[i]);
			}
		}

		m_pPath->draw();
	}
}

GLvoid walker::Input::mouseDown(SDL_MouseButtonEvent mse, WorkMode mode) {
	//
	switch (mse.button) {
	case SDL_BUTTON_LEFT:
		if (mode == WorkMode::CAMERA) {
			if (!m_isCamRot) {
				m_isCamRot = true;
				glm::vec3 camGridPos(m_pCamera->getPosition().x, m_pCamera->getPosition().y, 0.0f);
				m_radius = glm::distance(m_pAgent->getPosition(), camGridPos);
			}

			// camera rotates around own axis
			m_isCamAxi = false;
		}
		else if (mode == WorkMode::PATH) {
			// new path checkpoint
			for (size_t i = 0; i < m_pPath->checkPoint.size(); ++i) {
				if (glm::distance(glm::vec2(m_dirPos.x, m_dirPos.y), m_pPath->checkPoint[i]) < 0.25f * 1.0f) {
					m_pPoint = &(m_pPath->checkPoint[i]);
					break;
				}
			}
		}
		break;

	case SDL_BUTTON_MIDDLE:
		break;

	case SDL_BUTTON_RIGHT:
		// set path checkpoint
		if (mode == WorkMode::CAMERA) {
			if (!m_isCamRot) {
				m_isCamRot = true;
				glm::vec3 camGridPos(m_pCamera->getPosition().x, m_pCamera->getPosition().y, 0.0f);
				m_radius = glm::distance(m_pAgent->getPosition(), camGridPos);
			}

			// camera rotates around model's axis
			m_isCamAxi = true;
		}
		else if (mode == WorkMode::PATH && m_pPath) {
			m_pPath->bezierCurve.push_back(glm::vec2(m_dirPos.x, m_dirPos.y));
			if (m_pPath->checkPoint.empty()) m_pPath->checkPoint.push_back(m_pPath->bezierCurve.back());
			if (m_pPath->pathPoint.empty()) m_pPath->pathPoint.push_back(m_pPath->bezierCurve.back());

			if (m_pPath->bezierCurve.size() % 2 == 0) {
				glm::vec2 direction = glm::normalize(m_pPath->bezierCurve.back() - m_pPath->bezierCurve.front());
				glm::vec2 firstPoint(m_pPath->bezierCurve.front() + direction);
				glm::vec2 secondPoint(m_pPath->bezierCurve.back() - direction);

				m_pPath->bezierCurve.insert(--m_pPath->bezierCurve.end(), firstPoint);
				m_pPath->bezierCurve.insert(--m_pPath->bezierCurve.end(), secondPoint);

				std::vector<glm::vec2> tmpPoint;
				for (std::list<glm::vec2>::const_iterator it = m_pPath->bezierCurve.cbegin(); it != m_pPath->bezierCurve.cend(); ++it) {
					tmpPoint.push_back(*it);
				}

				m_pPath->checkPoint.insert(m_pPath->checkPoint.end(), ++tmpPoint.begin(), tmpPoint.end());
				tmpPoint = m_pPath->curve.getPathProfile(tmpPoint, 1.0f);
				m_pPath->pathPoint.insert(m_pPath->pathPoint.end(), ++tmpPoint.begin(), tmpPoint.end());
				m_pPath->bezierCurve.erase(m_pPath->bezierCurve.begin(), --m_pPath->bezierCurve.end());
			}

			m_pPath->draw();
		}
		break;

	default:
		break;
	}
}

GLvoid walker::Input::mouseUp(SDL_MouseButtonEvent mse, WorkMode mode) {
	//
	switch (mse.button) {
	case SDL_BUTTON_LEFT:
	case SDL_BUTTON_RIGHT:
		m_isCamRot = false;
		m_pPoint = nullptr;
		break;

	default:
		break;
	}
}

GLboolean walker::Input::keyDown(SDL_Keycode key, WorkMode mode) {
	// angle for navigation
	GLfloat angleXY = atan2(m_pCamera->getViewDirection().y, m_pCamera->getViewDirection().x);

	switch (key) {
	case SDLK_w:
	case SDLK_UP:
		if (mode == WorkMode::CAMERA)
			m_camUpdate = 0.1f * glm::vec3(glm::vec2(std::cos(angleXY), std::sin(angleXY)), 0.0f);
		break;
	case SDLK_s:
	case SDLK_DOWN:
		if (mode == WorkMode::CAMERA)
			m_camUpdate = -0.1f * glm::vec3(glm::vec2(std::cos(angleXY), std::sin(angleXY)), 0.0f);
		break;
	case SDLK_a:
	case SDLK_LEFT:
		if (mode == WorkMode::CAMERA) {
			angleXY += glm::radians(90.0f);
			m_camUpdate = 0.1f * glm::vec3(glm::vec2(std::cos(angleXY), std::sin(angleXY)), 0.0f);
		}
		break;
	case SDLK_d:
	case SDLK_RIGHT:
		if (mode == WorkMode::CAMERA) {
			angleXY -= glm::radians(90.0f);
			m_camUpdate = 0.1f * glm::vec3(glm::vec2(std::cos(angleXY), std::sin(angleXY)), 0.0f);
		}
		break;
	case SDLK_q:
		if (mode == WorkMode::CAMERA)
			m_camUpdate = glm::vec3(0.0f, 0.0f, -0.1f);
		break;
	case SDLK_e:
		if (mode == WorkMode::CAMERA)
			m_camUpdate = glm::vec3(0.0f, 0.0f, 0.1f);
		break;
	case SDLK_p:
		if (mode == WorkMode::PATH && m_pAgent && m_pPath && !m_pPath->checkPoint.empty()) {
			m_pAgent->setPosition(glm::vec3(m_pPath->pathPoint.front().x, m_pPath->pathPoint.front().y, 0.0f));
			m_pAgent->setIsPath(true);
		}
		break;

	case SDLK_u:
		if (mode == WorkMode::PATH && m_pPath && m_pPath->checkPoint.size() > 3) {
			m_pPath->checkPoint.erase(m_pPath->checkPoint.end() - 3, m_pPath->checkPoint.end());

			std::vector<glm::vec2>::iterator it = std::find(m_pPath->pathPoint.begin(),
				m_pPath->pathPoint.end(),
				m_pPath->checkPoint.back());

			m_pPath->pathPoint.erase(it, m_pPath->pathPoint.end());
			m_pPath->draw();
		}
		break;

	case SDLK_r:
		if (mode == WorkMode::PATH && m_pPath && !m_pPath->checkPoint.empty()) {
			m_pPath->pathPoint.clear();
			m_pPath->checkPoint.clear();
			m_pPath->bezierCurve.clear();
			m_pPath->clear();
		}
		break;

	case SDLK_c:
		if (m_pPath) {
			m_pPath->pathPoint.clear();
			m_pPath->checkPoint.clear();
			m_pPath->bezierCurve.clear();
			m_pPath->clear();
		}

		if (m_pAgent) {
			CalModel* pCalModel = m_pAgent->getBaseModel();
			for (size_t i = 0; i < pCalModel->getMixer()->getAnimationVector().size(); ++i) {
				// clear move animations
				walker::Model::instance()->clearCycle(pCalModel);
				// clear gesture animations
				walker::Model::instance()->clearAction(pCalModel);
			}

			m_pAgent->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
			m_pAgent->setDirection(glm::vec3(0.0f, -1.0f, 0.0f));
			m_pAgent->setIsPath(false);
			m_pAgent->setSpeed(0.0f);
		}

		m_pCamera->setPosition(glm::vec3(-0.9f, -1.4f, 1.6f));
		m_pCamera->setViewDirection(glm::vec3(0.315f, 0.485f, -0.2f));
		break;

	case SDLK_ESCAPE:
		return false;

	default:
		break;
	}

	return true;
}

GLvoid walker::Input::keyUp(SDL_Keycode key, WorkMode mode) {
	switch (key) {
	case SDLK_w:
	case SDLK_s:
	case SDLK_a:
	case SDLK_d:
	case SDLK_q:
	case SDLK_e:
	case SDLK_UP:
	case SDLK_DOWN:
	case SDLK_LEFT:
	case SDLK_RIGHT:
		m_camUpdate = glm::vec3(0.0f, 0.0f, 0.0f);
		break;

	default:
		break;
	}
}

glm::vec3 walker::Input::glmUnProject(const GLuint width, const GLuint height) {
	// glm::unProject ----------------------------------------------------------
	glm::vec3 planeF(width, m_height - height, 1.0f);
	glm::vec4 viewport(0.0f, 0.0f, m_width, m_height);
	glm::vec3 unProF = glm::unProject(planeF, m_pCamera->getWorldToViewMatrix(), *m_pProjMat, viewport);
	glm::vec3 beamDir = glm::normalize(unProF - m_pCamera->getPosition());
	// --------------------------------------------------------------------------------------------
	return glm::vec3(-m_pCamera->getPosition().z / beamDir.z * beamDir + m_pCamera->getPosition());
	// --------------------------------------------------------------------------------------------
}
