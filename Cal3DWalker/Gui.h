#pragma once

#include <CEGUI\CEGUI.h>
#include <CEGUI\RendererModules\OpenGL\GL3Renderer.h>
#include <SDL2\SDL_events.h>
#include <glm\glm.hpp>
#include <memory>

namespace walker {

class Gui {
public:
	Gui();
	Gui(const Gui&);
	Gui& operator=(const Gui&);

public:
	virtual ~Gui();
	static Gui* instance();

	GLvoid init(const std::string& resourceDirectory);
	GLvoid loadScheme(const std::string& schemeFile);

	CEGUI::Window* createWidget(CEGUI::Window* parent, const std::string& type,
		const glm::vec4& destRectPerc, const glm::vec4& destRectPix, const std::string& name);

	GLvoid setFont(const std::string& fontFile);
	GLvoid setMouseCursor(const std::string& imageFile);
	static GLvoid setWidgetDestRect(CEGUI::Window* widget, const glm::vec4& destRectPerc, const glm::vec4& destRectPix);

	GLvoid onSDLEvent(SDL_Event& evnt);
	GLvoid update();

	static CEGUI::OpenGL3Renderer* getRenderer();
	const CEGUI::GUIContext* getContext();
	CEGUI::Window* getRoot();

	GLvoid showMouseCursor();
	GLvoid hideMouseCursor();

	GLvoid draw();

private:
	static std::unique_ptr<Gui> m_pGui;
	static CEGUI::OpenGL3Renderer* m_pRenderer;
	CEGUI::GUIContext* m_pContext;
	CEGUI::Window* m_pRoot;
	GLuint m_lastTime;
};

}
