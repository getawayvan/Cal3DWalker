#pragma once

#include <GL\glew.h>
#include <glm\glm.hpp>
#include <vector>
#include <list>

namespace walker {

// Bezier curve - based on process used in De Casteljau's algorithm
class HigherOrderBezierCurve {
public:
	// Bezier path curve interpolation (smoothness range between 0 and 1)
	std::vector<glm::vec2> getPathProfile(const std::vector<glm::vec2>& vecPoints, const GLfloat smoothness);
	// Bezier speed curve quadratic interpolation (bendFactor range between -1 and 1, 0 for linear)
	GLfloat getQuadraticProfile(GLfloat deltaTime, GLfloat duration, GLfloat speedDifference, GLfloat bendFactor);
	// Bezier speed curve cubic interpolation (bendFactor range between -1 and 1, 0 for linear)
	GLfloat getCubicProfile(GLfloat deltaTime, GLfloat duration, GLfloat speedDifference, GLfloat bendFactor);

private:
	// Returns points on the curve defined by vecPoints
	std::vector<glm::vec2> getCurvePoints(const std::vector<glm::vec2>& vecPoints, const GLuint iterations);
	// Returns points for a single order of curve checkpoints
	std::vector<glm::vec2> order(const std::vector<glm::vec2>& vecPoints, const GLfloat t);
	// Returns a reference point between two checkpoints
	glm::vec2 lerp(const glm::vec2& a, const glm::vec2& b, const GLfloat t);
};

// Horizontal plain grid
class Plain {
public:
	Plain();
	~Plain();

	GLvoid draw(GLfloat* vert);

private:
	GLuint m_vao;
	GLuint m_vbo;
};

// Horizontal circle
class Circle {
public:
	Circle();
	~Circle();

	GLvoid draw(GLfloat* vert);

private:
	GLuint m_vao;
	GLuint m_vbo;
};

// B-Spline curve path
class Path {
	static const int PATH_NUM_BUFFERS = 4;
	static const int PATH_FLOATS_PER_POINT = 6;

public:
	HigherOrderBezierCurve curve;

public:
	Path();

	GLvoid clear();
	GLvoid draw();

public:
	std::list<glm::vec2> bezierCurve;
	std::vector<glm::vec2> checkPoint;
	std::vector<glm::vec2> pathPoint;

	GLfloat m_currentPathLength;

public:
	GLuint vao[PATH_NUM_BUFFERS];

private:
	GLuint m_vbo[PATH_NUM_BUFFERS];

	GLfloat* m_checkArray;
	GLfloat* m_wayPointArray;
	GLfloat* m_allPointArray;
	GLfloat* m_pathArray;
};

}