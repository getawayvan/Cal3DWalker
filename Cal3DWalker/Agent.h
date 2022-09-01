#pragma once

#include <GL\glew.h>
#include <glm\glm.hpp>
#include <memory>
#include <vector>

class CalModel;

namespace walker {

struct ScenarioInfo;

enum class RendMode;

class Agent {
public:
	Agent();
	~Agent();

	// Seters
	GLvoid setPosition(glm::vec3 position);
	GLvoid setDirection(glm::vec3 direction);
	GLvoid setSpeed(GLfloat speed);
	GLvoid setIsPath(GLboolean isPath);
	GLvoid setIsAbsLook(GLboolean isLook);
	GLvoid setIsRelLook(GLboolean isLook);

	// Geters
	CalModel* getBaseModel();
	glm::vec3 getPosition() const;
	glm::vec3 getDirection() const;
	GLfloat getRotation();
	GLfloat getSpeed() const;
	GLboolean getIsPath() const;

	// Initialization
	GLboolean init(const std::string& coreModelName);

	// Information
	GLboolean play(const ScenarioInfo* pScInfo);
	GLvoid update(const glm::vec3 look, const GLfloat deltaTime);
	GLvoid draw(const RendMode mode, const GLuint programID);

private:
	// CalModel methods
	GLboolean addModel(CalModel* pCalModel, const ScenarioInfo* pScInfo);
	CalModel* getModel(const std::string& coreModelName);

private:
	// CalModel members
	std::list<std::shared_ptr<CalModel>> m_models;

	// Model type
	std::string m_type;
	// Model on path
	GLboolean m_isPath;

	// Global position
	glm::vec3 m_position;
	// Global direction
	glm::vec3 m_direction;
	// Current speed
	GLfloat m_speed;
	// View absolute
	GLboolean m_isAbsLook;
	// View relative
	GLboolean m_isRelLook;
};

}