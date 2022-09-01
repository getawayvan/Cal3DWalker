#pragma once

#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>

namespace walker {

class Camera {
public:
	Camera();

	void setWorldToViewMatrix(const glm::vec3 position, const glm::vec3 direction);
	void setPosition(const glm::vec3 position);
	void setViewDirection(const glm::vec3 direction);

	glm::mat4 getWorldToViewMatrix() const;
	glm::vec3 getPosition() const;
	glm::vec3 getViewDirection() const;

private:
	glm::vec3 m_position;
	glm::vec3 m_viewDirection;
	const glm::vec3 UP;
};

}