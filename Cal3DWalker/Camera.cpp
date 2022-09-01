// Camera
#include "Camera.h"

walker::Camera::Camera() :
	m_position(0.0f, 0.0f, 0.0f),
	m_viewDirection(0.0f, 1.0f, 0.0f),
	UP(0.0f, 0.0f, 1.0f) {}

void walker::Camera::setWorldToViewMatrix(const glm::vec3 position, const glm::vec3 direction) {
	m_position = position;
	m_viewDirection = glm::normalize(direction);
}

void walker::Camera::setPosition(const glm::vec3 position) {
	m_position = position;
}

void walker::Camera::setViewDirection(const glm::vec3 direction) {
	m_viewDirection = glm::normalize(direction);
}

glm::mat4 walker::Camera::getWorldToViewMatrix() const {
	return glm::lookAt(m_position, m_position + m_viewDirection, UP);
}

glm::vec3 walker::Camera::getPosition() const {
	return m_position;
}

glm::vec3 walker::Camera::getViewDirection() const {
	return m_viewDirection;
}
