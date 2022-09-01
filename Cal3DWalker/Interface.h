#pragma once

#include <CEGUI\CEGUI.h>
#include <memory>
#include <list>

namespace walker {

class Agent;

enum class WorkMode {
	NONE,
	CAMERA,
	PATH
};

enum class RendMode {
	NONE,
	SKELETON,
	BOUNDBOX,
	FACES,
	MESHES
};

class Interface {
public:
	// methods
	Interface();

	// pre-initialization
	GLboolean setup();

	// initialization
	GLboolean initBackground();
	GLboolean initHelp();
	GLboolean initSelection();
	GLboolean initBasic();
	GLboolean initAdvanced();
	GLboolean initTotal();
	GLboolean initMixer();
	GLboolean fill();

	// agents
	Agent* addAgent();
	Agent* setAgent();
	Agent* getAgent() const;

	// get state enums
	WorkMode getWorkMode() const;
	RendMode getRendMode() const;

	// look height
	GLfloat getLookHeight() const;

private:
	// slots
	bool onHelpBtn(const CEGUI::EventArgs& e);
	bool onCloseBtn(const CEGUI::EventArgs& e);

	bool onViewBtn(const CEGUI::EventArgs& e);
	bool onPathBtn(const CEGUI::EventArgs& e);
	bool onSelectBtn(const CEGUI::EventArgs& e);
	bool onLookBtn(const CEGUI::EventArgs& e);
	bool onBasicBtn(const CEGUI::EventArgs& e);
	bool onAdvancedBtn(const CEGUI::EventArgs& e);
	bool onAbsLookBtn(const CEGUI::EventArgs& e);
	bool onRelLookBtn(const CEGUI::EventArgs& e);

	bool onModelBtn(const CEGUI::EventArgs& e);
	bool onModelCmb(const CEGUI::EventArgs& e);
	bool onMeshBtn(const CEGUI::EventArgs& e);
	bool onMeshCmb(const CEGUI::EventArgs& e);
	bool onCycleBtn(const CEGUI::EventArgs& e);
	bool onCycleCmb(const CEGUI::EventArgs& e);
	bool onActionBtn(const CEGUI::EventArgs& e);
	bool onActionCmb(const CEGUI::EventArgs& e);

	bool onMeshesBtn(const CEGUI::EventArgs& e);
	bool onItemsBtn(const CEGUI::EventArgs& e);
	bool onCyclesBtn(const CEGUI::EventArgs& e);
	bool onActionsBtn(const CEGUI::EventArgs& e);
	bool onApplyAdvBtn(const CEGUI::EventArgs& e);
	bool onDrawBtn(const CEGUI::EventArgs& e);
	bool onPlayAdvBtn(const CEGUI::EventArgs& e);
	bool onAniControlSld(const CEGUI::EventArgs& e);

private:
	// gui event element
	struct ScenarioGui {
		ScenarioGui();

		CEGUI::ToggleButton* m_pActivateBtn;
		CEGUI::Spinner* m_pDelaySpn;
		CEGUI::Spinner* m_pWeightSpn;
		CEGUI::Spinner* m_pScaleSpn;
		CEGUI::Spinner* m_pBlendInSpn;
		CEGUI::Spinner* m_pBlendOutSpn;
		CEGUI::Spinner* m_pDurationSpn;
	};

private:
	// members
	CEGUI::Window* m_pRoot;
	CEGUI::FrameWindow* m_pGuiBckSelect;
	CEGUI::FrameWindow* m_pGuiBckScenario;

	// local containers
	std::vector<CEGUI::ToggleButton*> m_pMeshes;
	std::vector<CEGUI::ToggleButton*> m_pItems;
	std::vector<CEGUI::ToggleButton*> m_pCycles;
	std::vector<CEGUI::ToggleButton*> m_pActions;

	// active agent
	Agent* m_pAgent;
	// container of all agents
	std::vector<std::shared_ptr<Agent>> m_agents;
	// container of scenario events gui elements
	std::vector<std::shared_ptr<ScenarioGui>> m_scenarioGui;

	// state enums
	WorkMode m_workMode;
	RendMode m_rendMode;
};

}