// Path
#include <iostream>
#include <fstream>

#include "Path.h"

// Returns path points for smoothness (0-1) interpolation value
std::vector<glm::vec2> walker::HigherOrderBezierCurve::getPathProfile(const std::vector<glm::vec2>& vecPoints, const GLfloat smoothness) {
	// Curve creation condition (at least three points required)
	if (vecPoints.size() < 3) return vecPoints;

	// Local containers
	std::vector<glm::vec2> bezierPoints; // Bezier curve points
	std::vector<glm::vec2> directLinePoints; // Straight line (with smooth edges) points
	std::vector<glm::vec2> interpolationPoints; // Final (iterpolated) curve points

	// Local const (for now)
	const GLfloat CURVE_LOD = 3.0f; // Curve level of detail
	const GLfloat EDGE_SMOOTHNESS = 5.0f; // Path smoothness minimum
										  
	// Local variables
	GLfloat pointPathLength = 0.0f;
	GLfloat bezierPathLength = 0.0f;
	GLfloat bezierPathLocalLength = 0.0f;
	std::vector<GLfloat> pointPathLengthLocal;

	// Direct line checkpoints length
	for (size_t i = 0; i < vecPoints.size() - 1; ++i) {
		pointPathLength += glm::distance(vecPoints[i], vecPoints[i + 1]);
		pointPathLengthLocal.push_back(pointPathLength);
	}

	// Bezier curve
	GLuint iterations = (GLuint)(pointPathLength * CURVE_LOD);
	bezierPoints = getCurvePoints(vecPoints, iterations);
	if (smoothness == 1.0f) return bezierPoints;

	// bezier curve length
	for (size_t i = 0; i < bezierPoints.size() - 1; ++i) {
		bezierPathLength += glm::distance(bezierPoints[i], bezierPoints[i + 1]);
	}

	// Direct line first interpolation point
	directLinePoints.push_back(vecPoints.front());

	// Interpolation values based on bezier curve
	for (size_t i = 0; i < bezierPoints.size() - 1; ++i) {
		// bezier curve checkpoints length
		bezierPathLocalLength += glm::distance(bezierPoints[i], bezierPoints[i + 1]);

		// this should never happen, but just to avoid division with 0
		if (bezierPathLength == 0) bezierPathLength = 0.0001f;
		// bezier curve checkpoints length (%)
		GLfloat bezierPathLocalRelative = bezierPathLocalLength / bezierPathLength;
		// direct line checkpoints length
		GLfloat directLinePathlocal = bezierPathLocalRelative * pointPathLength;

		// Direct line middle interpolation points
		for (size_t i = 0; i < vecPoints.size() - 1; ++i) {
			// Current line segment
			if (directLinePathlocal < pointPathLengthLocal[i]) {
				// Direction of current line segment
				glm::vec2 direction = glm::normalize(vecPoints[i + 1] - vecPoints[i]);
				// Local point path length
				GLfloat localLength = directLinePathlocal;
				if (i != 0) localLength = directLinePathlocal - pointPathLengthLocal[i - 1];

				// Point creation
				directLinePoints.push_back(vecPoints[i] + glm::vec2(localLength * direction.x, localLength * direction.y));
				break;
			}
		}
	}

	// Looking for possible hard edges (ignoring first and last checkpoint)
	for (size_t i = 0; i < pointPathLengthLocal.size() - 1; ++i) {
		// Smoothing direct line segments between middle checkpoints
		std::vector<glm::vec2> edgePoints;
		GLfloat directLinePathLocalLength = 0.0f;
		for (size_t j = 0; j < directLinePoints.size() - 1; ++j) {
			// direct line local length
			directLinePathLocalLength += glm::distance(directLinePoints[j], directLinePoints[j + 1]);

			// adding selected point
			if (std::fabs(pointPathLengthLocal[i] - directLinePathLocalLength) < EDGE_SMOOTHNESS) {
				edgePoints.push_back(directLinePoints[j]);
			}
		}

		// Removing hard edges
		if (edgePoints.size() > 2) {
			// Replacing the points around edges (bezier adds last point)
			std::vector<glm::vec2> newEdgePoints = getCurvePoints(edgePoints, (GLuint)edgePoints.size() - 1);
			for (size_t i = 0; i < directLinePoints.size(); ++i) {
				for (size_t j = 0; j < edgePoints.size(); ++j) {
					if (edgePoints[j] == directLinePoints[i]) {
						directLinePoints[i] = newEdgePoints[j];
					}
				}
			}
		}
	}

	// Direct line last interpolation point
	directLinePoints.push_back(vecPoints.back());
	if (smoothness == 0.0f) return directLinePoints;

	// Interpolation line points creation
	if (directLinePoints.size() == bezierPoints.size()) {
		// insert first checkpoint
		interpolationPoints.push_back(directLinePoints.front());

		// insert middle checkpoints
		for (size_t i = 1; i < directLinePoints.size() - 1; ++i) {
			GLfloat distance = glm::distance(directLinePoints[i], bezierPoints[i]);
			if (distance == 0) {
				interpolationPoints.push_back(directLinePoints[i]);
			}
			else {
				glm::vec2 direction = glm::normalize(bezierPoints[i] - directLinePoints[i]);
				interpolationPoints.push_back(directLinePoints[i]
					+ glm::vec2(distance * smoothness * direction.x, distance * smoothness * direction.y));
			}
		}

		// insert last checkpoint
		interpolationPoints.push_back(directLinePoints.back());
	}
	else {
		std::cout << "Path curve creation failed. Bezier and direct Line vector not the same size." << std::endl;
		return directLinePoints;
	}

	// Curve created
	return interpolationPoints;
}

// Returns interpolated (quadratic) speed value for a portion of animation time
GLfloat walker::HigherOrderBezierCurve::getQuadraticProfile(GLfloat deltaTime, GLfloat duration, GLfloat speedDifference, GLfloat bendFactor) {
	std::vector<glm::vec2> profilePoints;

	// Curve points definition
	profilePoints.push_back(glm::vec2(0.0f, 0.0f));
	profilePoints.push_back(glm::vec2(0.5f * duration * (1.0f + bendFactor), 0.5f * speedDifference * (1.0f - bendFactor)));
	profilePoints.push_back(glm::vec2(duration, speedDifference));

	if (duration == 0) return 0.0f;

	// Interpolation value
	GLfloat t = deltaTime / duration;

	while (profilePoints.size() > 2) {
		profilePoints = order(profilePoints, t);
	}

	// Interpolated speedDifference for deltaTime
	return lerp(profilePoints[0], profilePoints[1], t).y;
}

// Returns interpolated (cubic) speed value for a portion of animation time
GLfloat walker::HigherOrderBezierCurve::getCubicProfile(GLfloat deltaTime, GLfloat duration, GLfloat speedDifference, GLfloat bendFactor) {
	std::vector<glm::vec2> profilePoints;

	// Curve points definition
	profilePoints.push_back(glm::vec2(0.0f, 0.0f));
	profilePoints.push_back(glm::vec2(0.5f * duration * (1.0f + bendFactor), 0.5f * speedDifference * (1.0f - bendFactor)));
	profilePoints.push_back(glm::vec2(0.5f * duration * (1.0f - bendFactor), 0.5f * speedDifference * (1.0f + bendFactor)));
	profilePoints.push_back(glm::vec2(duration, speedDifference));

	if (duration == 0) return 0.0f;

	// Interpolation value
	GLfloat t = deltaTime / duration;

	while (profilePoints.size() > 2) {
		profilePoints = order(profilePoints, t);
	}

	// Interpolated speedDifference for deltaTime
	return lerp(profilePoints[0], profilePoints[1], t).y;
}

// Returns points on the curve defined by vecPoints
std::vector<glm::vec2> walker::HigherOrderBezierCurve::getCurvePoints(const std::vector<glm::vec2>& vecPoints, const GLuint iterations) {
	std::vector<glm::vec2> vecCurvePoints;

	if (vecPoints.size() < 2) {
		std::cout << "vector size incorrect (less than 2)" << std::endl;
	}
	else {
		vecCurvePoints.reserve(iterations);

		for (GLuint i = 0; i < iterations; ++i) {
			std::vector<glm::vec2> newVecPoints = vecPoints;
			float t = (float)i / iterations;

			while (newVecPoints.size() > 2) {
				newVecPoints = order(newVecPoints, t);
			}

			vecCurvePoints.push_back(lerp(newVecPoints[0], newVecPoints[1], t));
		}

		vecCurvePoints.push_back(vecPoints.back());
	}

	return vecCurvePoints;
}

// Returns points for a single order of curve checkpoints
std::vector<glm::vec2> walker::HigherOrderBezierCurve::order(const std::vector<glm::vec2>& vecPoints, const GLfloat t) {
	std::vector<glm::vec2> vecCurvePoints;
	vecCurvePoints.reserve(vecPoints.size());

	for (size_t i = 0; i < vecPoints.size() - 1; ++i) {
		vecCurvePoints.push_back(lerp(vecPoints[i], vecPoints[i + 1], t));
	}

	return vecCurvePoints;
}

// Returns a reference point between two checkpoints
glm::vec2 walker::HigherOrderBezierCurve::lerp(const glm::vec2& a, const glm::vec2& b, const float t) {
	return glm::vec2(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t);
}

// Horizontal plain grid
walker::Plain::Plain() {
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(GLfloat), nullptr, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

	glBindVertexArray(0);
}

walker::Plain::~Plain() {
	glDeleteBuffers(1, &m_vbo);
	glDeleteVertexArrays(1, &m_vao);
}

GLvoid walker::Plain::draw(GLfloat* vert) {
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 24 * sizeof(GLfloat), vert);
	glDrawArrays(GL_LINE_LOOP, 0, 4);
	glBindVertexArray(0);
}

// Horizontal circle
walker::Circle::Circle() {
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, 108 * sizeof(GLfloat), nullptr, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

	glBindVertexArray(0);
}

walker::Circle::~Circle() {
	glDeleteBuffers(1, &m_vbo);
	glDeleteVertexArrays(1, &m_vao);
}

GLvoid walker::Circle::draw(GLfloat* vert) {
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 108 * sizeof(GLfloat), vert);
	glDrawArrays(GL_LINE_LOOP, 0, 18);
	glBindVertexArray(0);
}

// B-Spline curve path
walker::Path::Path()
	: m_checkArray(nullptr),
	m_wayPointArray(nullptr),
	m_allPointArray(nullptr),
	m_pathArray(nullptr),
	m_currentPathLength(0.0f)
{
	// Buffers
	glGenVertexArrays(PATH_NUM_BUFFERS, vao);
	glGenBuffers(PATH_NUM_BUFFERS, m_vbo);
}

GLvoid walker::Path::clear() {
	if (m_pathArray) delete[] m_pathArray;
	if (m_wayPointArray) delete[] m_wayPointArray;
	if (m_checkArray) delete[] m_checkArray;
	if (m_allPointArray) delete[] m_allPointArray;

	m_pathArray = nullptr;
	m_wayPointArray = nullptr;
	m_checkArray = nullptr;
	m_allPointArray = nullptr;
}

GLvoid walker::Path::draw() {
	// Clear buffers
	clear();

	// Setting data for drawing
	if (!checkPoint.empty()) {
		GLuint tmp1 = 0, tmp2 = 0, allP = 0;
		for (size_t i = 0; i < checkPoint.size(); ++i) {
			if (i % 3 == 0)
				++tmp1;
			else
				++tmp2;
		}

		m_checkArray = new GLfloat[PATH_FLOATS_PER_POINT * tmp1];
		m_wayPointArray = new GLfloat[PATH_FLOATS_PER_POINT * tmp2];
		m_allPointArray = new GLfloat[PATH_FLOATS_PER_POINT * checkPoint.size()];

		tmp1 = 0, tmp2 = 0;
		for (size_t i = 0; i < checkPoint.size(); ++i) {

			if (i % 3 == 0) {
				m_checkArray[tmp1++] = checkPoint[i].x;
				m_checkArray[tmp1++] = checkPoint[i].y;
				m_checkArray[tmp1++] = 0.0f;
				m_checkArray[tmp1++] = 0.05f;
				m_checkArray[tmp1++] = 0.55f;
				m_checkArray[tmp1++] = 0.85f;
			}
			else {
				m_wayPointArray[tmp2++] = checkPoint[i].x;
				m_wayPointArray[tmp2++] = checkPoint[i].y;
				m_wayPointArray[tmp2++] = 0.0f;
				m_wayPointArray[tmp2++] = 0.35f;
				m_wayPointArray[tmp2++] = 0.95f;
				m_wayPointArray[tmp2++] = 0.15f;
			}

			m_allPointArray[allP++] = checkPoint[i].x;
			m_allPointArray[allP++] = checkPoint[i].y;
			m_allPointArray[allP++] = 0.0f;
			m_allPointArray[allP++] = 0.55f;
			m_allPointArray[allP++] = 0.85f;
			m_allPointArray[allP++] = 0.25f;
		}

		glBindVertexArray(vao[0]);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, PATH_FLOATS_PER_POINT * tmp1 * sizeof(GLfloat), m_checkArray, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, PATH_FLOATS_PER_POINT * sizeof(GLfloat), nullptr);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, PATH_FLOATS_PER_POINT * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

		glBindVertexArray(vao[1]);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, PATH_FLOATS_PER_POINT * tmp2 * sizeof(GLfloat), m_wayPointArray, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, PATH_FLOATS_PER_POINT * sizeof(GLfloat), nullptr);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, PATH_FLOATS_PER_POINT * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

		glBindVertexArray(vao[2]);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo[2]);
		glBufferData(GL_ARRAY_BUFFER, PATH_FLOATS_PER_POINT * checkPoint.size() * sizeof(GLfloat), m_allPointArray, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, PATH_FLOATS_PER_POINT * sizeof(GLfloat), nullptr);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, PATH_FLOATS_PER_POINT * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	}

	if (!pathPoint.empty()) {
		GLuint arrInd = 0;
		m_pathArray = new GLfloat[PATH_FLOATS_PER_POINT * pathPoint.size()];
		for (size_t i = 0; i < pathPoint.size(); ++i) {
			m_pathArray[arrInd++] = pathPoint[i].x;
			m_pathArray[arrInd++] = pathPoint[i].y;
			m_pathArray[arrInd++] = 0.0f;
			m_pathArray[arrInd++] = 0.97f;
			m_pathArray[arrInd++] = 0.83f;
			m_pathArray[arrInd++] = 0.25f;
		}

		glBindVertexArray(vao[3]);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo[3]);
		glBufferData(GL_ARRAY_BUFFER, PATH_FLOATS_PER_POINT * pathPoint.size() * sizeof(GLfloat), m_pathArray, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, PATH_FLOATS_PER_POINT * sizeof(GLfloat), nullptr);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, PATH_FLOATS_PER_POINT * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	}

	glBindVertexArray(0);
}
