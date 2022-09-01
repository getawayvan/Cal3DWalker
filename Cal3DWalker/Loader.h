#pragma once

#include <GL\glew.h>
#include <memory>
#include <string>
#include <vector>
#include <map>

namespace cal3d {
	class TiXmlNode;
}

namespace walker {

class Window;

class Loader {
public:
	// methods
	Loader(Window* window);
	GLboolean load(const std::string& directory);

private:
	// methods
	GLboolean loadDirectory(const std::string& directory, std::vector<std::string>& xmlFiles);

	GLboolean loadStructure(cal3d::TiXmlNode* pNode);
	GLboolean loadGraphics(cal3d::TiXmlNode* pNode);
	GLboolean loadDynamics(cal3d::TiXmlNode* pNode);
	GLuint loadTexture(const std::string& filePath);
	GLvoid updateProBar();

private:
	// members
	std::string m_filepath;
	Window* m_pWindow;
};

}