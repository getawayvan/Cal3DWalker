#pragma once

#include <GL\glew.h>
#include <SDL2\SDL.h>
#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>

namespace walker {

class Camera;
class Path;
class Agent;
class Gui;

enum class WorkMode;
enum class RendMode;

class Input {
public:
	Input(glm::mat4* proj, Camera* camera, Path* path, GLuint width, GLuint height);

	GLvoid setScreenSize(const GLuint width, const GLuint height);
	GLvoid setAgent(Agent* pAgent);
	glm::vec3 getCamUpdate() const;
	glm::vec3 getDirPos() const;

	GLvoid mouseMotion(SDL_MouseMotionEvent mme, WorkMode mode);
	GLvoid mouseDown(SDL_MouseButtonEvent mse, WorkMode mode);
	GLvoid mouseUp(SDL_MouseButtonEvent mse, WorkMode mode);
	GLboolean keyDown(SDL_Keycode key, WorkMode mode);
	GLvoid keyUp(SDL_Keycode key, WorkMode mode);

private:
	glm::vec3 glmUnProject(const GLuint width, const GLuint height);

	glm::vec2* m_pPoint;
	glm::mat4* m_pProjMat;
	Camera* m_pCamera;
	Path* m_pPath;
	Agent* m_pAgent;

	glm::vec3 m_dirPos;
	glm::vec3 m_camUpdate;

	GLuint m_width;
	GLuint m_height;

	GLfloat m_radius;
	glm::ivec2 m_camRot;
	GLboolean m_isCamRot;
	GLboolean m_isCamAxi;
};

}