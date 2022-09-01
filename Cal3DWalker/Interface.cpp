// Interface
#include <cal3d\cal3d.h>

#include "Model.h"
#include "Agent.h"
#include "Gui.h"
#include "Interface.h"

walker::Interface::Interface() :
	m_pRoot(nullptr),
	m_pGuiBckSelect(nullptr),
	m_pGuiBckScenario(nullptr),
	m_pAgent(nullptr),
	m_workMode(WorkMode::NONE),
	m_rendMode(RendMode::NONE) {}

GLboolean walker::Interface::setup() {
	// gui setup
	walker::Gui::instance()->init("../gui");
	walker::Gui::instance()->loadScheme("WindowsLook.scheme");
	walker::Gui::instance()->setFont("DejaVuSans-12");
	m_pRoot = walker::Gui::instance()->getRoot();
	if (!m_pRoot)
		return false;

	// title label
	CEGUI::DefaultWindow* pTitleLbl = static_cast<CEGUI::DefaultWindow*>(walker::Gui::instance()->createWidget(
		m_pRoot, "WindowsLook/Label", glm::vec4(0.0f), glm::vec4(375.0f, 260.0f, 50.0f, 20.0f), "titleLbl"));

	if (pTitleLbl) {
		pTitleLbl->setInheritsAlpha(false);
		pTitleLbl->setText("loading");
		pTitleLbl->setEnabled(true);
		pTitleLbl->setVisible(true);
	}

	// progress bar
	CEGUI::ProgressBar* pProBar = static_cast<CEGUI::ProgressBar*>(walker::Gui::instance()->createWidget(
		m_pRoot, "WindowsLook/ProgressBar", glm::vec4(0.35f, 0.475f, 0.3f, 0.025f), glm::vec4(0.0f), "loadProBar"));

	if (pProBar) {
		pProBar->setEnabled(true);
		pProBar->setVisible(true);
		pProBar->setAlpha(0.75f);
		pProBar->setProgress(0.0f);
	}

	// setup
	return pTitleLbl && pProBar;
}

GLboolean walker::Interface::initBackground() {
	// selection background
	m_pGuiBckSelect = static_cast<CEGUI::FrameWindow*>(walker::Gui::instance()->createWidget(
		m_pRoot, "WindowsLook/FrameWindow", glm::vec4(0.0f), glm::vec4(0.0f, 0.0f, 90.0f, 185.0f), "bckSelectWnd"));

	if (m_pGuiBckSelect) {
		m_pGuiBckSelect->setAlpha(0.0f);
		m_pGuiBckSelect->setTitleBarEnabled(false);
		m_pGuiBckSelect->setCloseButtonEnabled(false);
		m_pGuiBckSelect->setEnabled(false);
		m_pGuiBckSelect->setVisible(false);
		m_pGuiBckSelect->setDragMovingEnabled(false);
		m_pGuiBckSelect->setSizingEnabled(false);
	}

	// basic background
	m_pGuiBckScenario = static_cast<CEGUI::FrameWindow*>(walker::Gui::instance()->createWidget(
		m_pRoot, "WindowsLook/FrameWindow", glm::vec4(0.0f), glm::vec4(90.0f, 0.0f, 695.0f, 535.0f), "bckScenarioWnd"));

	if (m_pGuiBckScenario) {
		m_pGuiBckScenario->setAlpha(0.0f);
		m_pGuiBckScenario->setTitleBarEnabled(false);
		m_pGuiBckScenario->setCloseButtonEnabled(false);
		m_pGuiBckScenario->setEnabled(false);
		m_pGuiBckScenario->setVisible(false);
		m_pGuiBckScenario->setDragMovingEnabled(false);
		m_pGuiBckScenario->setSizingEnabled(false);
	}

	// init backgrounds
	return m_pGuiBckSelect && m_pGuiBckScenario;
}

GLboolean walker::Interface::initHelp() {
	// help button
	CEGUI::PushButton* pHelpBtn = static_cast<CEGUI::PushButton*>(walker::Gui::instance()->createWidget(
		m_pRoot, "WindowsLook/Button", glm::vec4(0.0f), glm::vec4(725.0f, 10.0f, 55.0f, 30.0f), "helpBtn"));

	if (pHelpBtn) {
		pHelpBtn->setText("help");
		pHelpBtn->setEnabled(true);
		pHelpBtn->setVisible(true);
		pHelpBtn->setAlwaysOnTop(true);
		pHelpBtn->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&walker::Interface::onHelpBtn, this));
	}

	// help background
	CEGUI::FrameWindow* m_pHelpBck = static_cast<CEGUI::FrameWindow*>(walker::Gui::instance()->createWidget(
		m_pRoot, "WindowsLook/FrameWindow", glm::vec4(0.0f), glm::vec4(200.0f, 10.0f, 400.0f, 570.0f), "bckHelpWnd"));

	if (m_pHelpBck) {
		m_pHelpBck->setAlpha(0.75f);
		m_pHelpBck->setTitleBarEnabled(false);
		m_pHelpBck->setCloseButtonEnabled(false);
		m_pHelpBck->setFrameEnabled(false);
		m_pHelpBck->setEnabled(false);
		m_pHelpBck->setVisible(false);
		m_pHelpBck->setAlwaysOnTop(true);
		m_pHelpBck->setDragMovingEnabled(false);
		m_pHelpBck->setSizingEnabled(false);
	}

	// help text
	CEGUI::DefaultWindow* pHelpTxt = static_cast<CEGUI::DefaultWindow*>(walker::Gui::instance()->createWidget(
		m_pHelpBck, "WindowsLook/StaticText", glm::vec4(0.0f), glm::vec4(10.0f, 10.0f, 380.0f, 515.0f), "helpTxt"));

	CEGUI::String helpText("[padding='l:125 t:0 r:0 b:0']Cal3D Walker v2.5\n");

	helpText.append("[padding='l:10 t:25 r:0 b:0']view:\n");
	helpText.append("[padding='l:10 t:10 r:0 b:0']- use WASD or arrows to move camera horizontally\n");
	helpText.append("[padding='l:10 t:0 r:0 b:0']- use Q & E key to move camera vertically\n");
	helpText.append("- left mouse click rotates camera around its axis\n");
	helpText.append("- right mouse click rotates camera around model\n");

	helpText.append("[padding='l:10 t:25 r:0 b:0']path:\n");
	helpText.append("[padding='l:10 t:10 r:0 b:0']- right mouse click adds a checkpoint\n");
	helpText.append("[padding='l:10 t:0 r:0 b:0']- left mouse click moves a checkpoint\n");
	helpText.append("- left mouse click moves anchor point\n");
	helpText.append("- use 'u' key to undo last added checkpoint\n");
	helpText.append("- use 'r' key to remove path entirely\n");
	helpText.append("- use 'p' key to place model on path\n");

	helpText.append("[padding='l:10 t:25 r:0 b:0']select:\n");
	helpText.append("[padding='l:10 t:10 r:0 b:0']- basic mode adds a single mesh and animation\n");
	helpText.append("[padding='l:10 t:0 r:0 b:0']- total mode adds multiple meshes and animations\n");

	helpText.append("[padding='l:10 t:25 r:0 b:0']joint:\n");
	helpText.append("[padding='l:10 t:10 r:0 b:0']- absolute mode rotates in global coordinates\n");
	helpText.append("[padding='l:10 t:0 r:0 b:0']- relative mode rotates in model coordinates\n");

	helpText.append("[padding='l:10 t:25 r:0 b:0']use 'c' key to reset to default\n");
	helpText.append("[padding='l:10 t:0 r:0 b:0']use Esc key to quit\n");

	if (pHelpTxt) {
		pHelpTxt->setMargin(CEGUI::UBox(CEGUI::UDim(5.0, 5.0)));
		pHelpTxt->setInheritsAlpha(false);
		pHelpTxt->setEnabled(true);
		pHelpTxt->setVisible(true);
		pHelpTxt->setText(helpText);
		pHelpTxt->setProperty("FrameEnabled", "false");
		pHelpTxt->setProperty("TextColours", "tl:FFF5F5F5 tr:FFF5F5F5 bl:FFF5F5F5 br:FFF5F5F5");
		pHelpTxt->setProperty("BackgroundColours", "tl:DD888888 tr:DD888888 bl:DD888888 br:DD888888");
	}

	// close button
	CEGUI::PushButton* pCloseBtn = static_cast<CEGUI::PushButton*>(walker::Gui::instance()->createWidget(
		m_pHelpBck, "WindowsLook/Button", glm::vec4(0.0f), glm::vec4(172.5f, 532.0f, 55.0f, 30.0f), "closeBtn"));

	if (pCloseBtn) {
		pCloseBtn->setInheritsAlpha(false);
		pCloseBtn->setText("close");
		pCloseBtn->setEnabled(true);
		pCloseBtn->setVisible(true);
		pCloseBtn->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&walker::Interface::onCloseBtn, this));
	}

	// init help
	return pHelpBtn && m_pHelpBck && pHelpTxt && pCloseBtn;
}

GLboolean walker::Interface::initSelection() {
	// view button
	CEGUI::RadioButton* pViewBtn = static_cast<CEGUI::RadioButton*>(walker::Gui::instance()->createWidget(
		m_pGuiBckSelect, "WindowsLook/RadioButton", glm::vec4(0.0f), glm::vec4(12.0f, 7.0f, 70.0f, 20.0f), "viewBtn"));

	if (pViewBtn) {
		pViewBtn->setGroupID(1);
		pViewBtn->setInheritsAlpha(false);
		pViewBtn->setSelected(true);
		pViewBtn->setText("view");
		pViewBtn->subscribeEvent(CEGUI::RadioButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&walker::Interface::onViewBtn, this));

		m_workMode = WorkMode::CAMERA;
	}

	// path button
	CEGUI::RadioButton* pPathBtn = static_cast<CEGUI::RadioButton*>(walker::Gui::instance()->createWidget(
		m_pGuiBckSelect, "WindowsLook/RadioButton", glm::vec4(0.0f), glm::vec4(12.0f, 27.0f, 70.0f, 20.0f), "pathBtn"));

	if (pPathBtn) {
		pPathBtn->setGroupID(1);
		pPathBtn->setInheritsAlpha(false);
		pPathBtn->setText("path");
		pPathBtn->subscribeEvent(CEGUI::RadioButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&walker::Interface::onPathBtn, this));
	}

	// select button
	CEGUI::RadioButton* pSelectBtn = static_cast<CEGUI::RadioButton*>(walker::Gui::instance()->createWidget(
		m_pGuiBckSelect, "WindowsLook/RadioButton", glm::vec4(0.0f), glm::vec4(12.0f, 47.0f, 70.0f, 20.0f), "selectBtn"));

	if (pSelectBtn) {
		pSelectBtn->setGroupID(1);
		pSelectBtn->setInheritsAlpha(false);
		pSelectBtn->setText("select");
		pSelectBtn->subscribeEvent(CEGUI::RadioButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&walker::Interface::onSelectBtn, this));
	}

	// look button
	CEGUI::RadioButton* pLookBtn = static_cast<CEGUI::RadioButton*>(walker::Gui::instance()->createWidget(
		m_pGuiBckSelect, "WindowsLook/RadioButton", glm::vec4(0.0f), glm::vec4(12.0f, 67.0f, 70.0f, 20.0f), "lookBtn"));

	if (pLookBtn) {
		pLookBtn->setGroupID(1);
		pLookBtn->setInheritsAlpha(false);
		pLookBtn->setText("joint");
		pLookBtn->subscribeEvent(CEGUI::RadioButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&walker::Interface::onLookBtn, this));
	}

	// basic button
	CEGUI::RadioButton* pBasicBtn = static_cast<CEGUI::RadioButton*>(walker::Gui::instance()->createWidget(
		m_pGuiBckSelect, "WindowsLook/RadioButton", glm::vec4(0.0f), glm::vec4(12.0f, 100.0f, 70.0f, 15.0f), "basicBtn"));

	if (pBasicBtn) {
		pBasicBtn->setGroupID(2);
		pBasicBtn->setInheritsAlpha(false);
		pBasicBtn->setSelected(true);
		pBasicBtn->setText("basic");
		pBasicBtn->setEnabled(false);
		pBasicBtn->setVisible(false);
		pBasicBtn->subscribeEvent(CEGUI::RadioButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&walker::Interface::onBasicBtn, this));
	}

	// advanced button
	CEGUI::RadioButton* pAdvancedBtn = static_cast<CEGUI::RadioButton*>(walker::Gui::instance()->createWidget(
		m_pGuiBckSelect, "WindowsLook/RadioButton", glm::vec4(0.0f), glm::vec4(12.0f, 120.0f, 70.0f, 15.0f), "advancedBtn"));

	if (pAdvancedBtn) {
		pAdvancedBtn->setGroupID(2);
		pAdvancedBtn->setInheritsAlpha(false);
		pAdvancedBtn->setText("total");
		pAdvancedBtn->setEnabled(false);
		pAdvancedBtn->setVisible(false);
		pAdvancedBtn->subscribeEvent(CEGUI::RadioButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&walker::Interface::onAdvancedBtn, this));
	}

	// absolute look button
	CEGUI::RadioButton* pAbsLookBtn = static_cast<CEGUI::RadioButton*>(walker::Gui::instance()->createWidget(
		m_pGuiBckSelect, "WindowsLook/RadioButton", glm::vec4(0.0f), glm::vec4(12.0f, 100.0f, 70.0f, 15.0f), "absLookBtn"));

	if (pAbsLookBtn) {
		pAbsLookBtn->setGroupID(3);
		pAbsLookBtn->setInheritsAlpha(false);
		pAbsLookBtn->setSelected(true);
		pAbsLookBtn->setText("absolute");
		pAbsLookBtn->setEnabled(false);
		pAbsLookBtn->setVisible(false);
		pAbsLookBtn->subscribeEvent(CEGUI::RadioButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&walker::Interface::onAbsLookBtn, this));
	}

	// relative look button
	CEGUI::RadioButton* pRelLookBtn = static_cast<CEGUI::RadioButton*>(walker::Gui::instance()->createWidget(
		m_pGuiBckSelect, "WindowsLook/RadioButton", glm::vec4(0.0f), glm::vec4(12.0f, 120.0f, 70.0f, 15.0f), "relLookBtn"));

	if (pRelLookBtn) {
		pRelLookBtn->setGroupID(3);
		pRelLookBtn->setInheritsAlpha(false);
		pRelLookBtn->setText("relative");
		pRelLookBtn->setEnabled(false);
		pRelLookBtn->setVisible(false);
		pRelLookBtn->subscribeEvent(CEGUI::RadioButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&walker::Interface::onRelLookBtn, this));
	}

	// look height spinner
	CEGUI::Spinner* pLookSpn = static_cast<CEGUI::Spinner*>(walker::Gui::instance()->createWidget(
		m_pGuiBckSelect, "WindowsLook/Spinner", glm::vec4(0.0f), glm::vec4(12.0f, 140.0f, 50.0f, 30.0f), "lookSpn"));

	if (pLookSpn) {
		pLookSpn->setInheritsAlpha(false);
		pLookSpn->setTextInputMode(CEGUI::Spinner::FloatingPoint);
		pLookSpn->setMinimumValue(0.0f);
		pLookSpn->setMaximumValue(3.0f);
		pLookSpn->setCurrentValue(0.0f);
		pLookSpn->setStepSize(0.1f);
		pLookSpn->setEnabled(false);
		pLookSpn->setVisible(false);
	}

	// init selection
	return pViewBtn && pPathBtn && pSelectBtn && pLookBtn && pBasicBtn && pAdvancedBtn && pAbsLookBtn && pRelLookBtn && pLookSpn;
}

GLboolean walker::Interface::initBasic() {
	// model button
	CEGUI::ToggleButton* pModelBtn = static_cast<CEGUI::ToggleButton*>(walker::Gui::instance()->createWidget(
		m_pGuiBckScenario, "WindowsLook/Checkbox", glm::vec4(0.0f), glm::vec4(5.0f, 15.0f, 55.0f, 15.0f), "modelBtn"));

	if (pModelBtn) {
		pModelBtn->setInheritsAlpha(false);
		pModelBtn->setSelected(true);
		pModelBtn->setText("model");
		pModelBtn->subscribeEvent(CEGUI::ToggleButton::EventMouseClick, CEGUI::Event::Subscriber(&walker::Interface::onModelBtn, this));
	}

	// model combobox
	CEGUI::Combobox* pModelCmb = static_cast<CEGUI::Combobox*>(walker::Gui::instance()->createWidget(
		m_pGuiBckScenario, "WindowsLook/Combobox", glm::vec4(0.0f), glm::vec4(70.0f, 10.0f, 160.0f, 200.0f), "modelCmb"));

	if (pModelCmb) {
		pModelCmb->setReadOnly(true);
		pModelCmb->setInheritsAlpha(false);
		pModelCmb->setClippedByParent(false);
		pModelCmb->setAutoSizeListHeightToContent(true);
		pModelCmb->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted, CEGUI::Event::Subscriber(&walker::Interface::onModelCmb, this));
	}

	// mesh button
	CEGUI::ToggleButton* pMeshBtn = static_cast<CEGUI::ToggleButton*>(walker::Gui::instance()->createWidget(
		m_pGuiBckScenario, "WindowsLook/Checkbox", glm::vec4(0.0f), glm::vec4(5.0f, 50.0f, 55.0f, 15.0f), "meshBtn"));

	if (pMeshBtn) {
		pMeshBtn->setInheritsAlpha(false);
		pMeshBtn->setSelected(true);
		pMeshBtn->setText("mesh");
		pMeshBtn->subscribeEvent(CEGUI::ToggleButton::EventMouseClick, CEGUI::Event::Subscriber(&walker::Interface::onMeshBtn, this));

		m_rendMode = RendMode::MESHES;
	}

	// mesh combobox
	CEGUI::Combobox* pMeshCmb = static_cast<CEGUI::Combobox*>(walker::Gui::instance()->createWidget(
		m_pGuiBckScenario, "WindowsLook/Combobox", glm::vec4(0.0f), glm::vec4(70.0f, 45.0f, 160.0f, 200.0f), "meshCmb"));

	if (pMeshCmb) {
		pMeshCmb->setReadOnly(true);
		pMeshCmb->setInheritsAlpha(false);
		pMeshCmb->setClippedByParent(false);
		pMeshCmb->setAutoSizeListHeightToContent(true);
		pMeshCmb->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted, CEGUI::Event::Subscriber(&walker::Interface::onMeshCmb, this));
	}

	// cycle button
	CEGUI::ToggleButton* pCycleBtn = static_cast<CEGUI::ToggleButton*>(walker::Gui::instance()->createWidget(
		m_pGuiBckScenario, "WindowsLook/Checkbox", glm::vec4(0.0f), glm::vec4(5.0f, 85.0f, 55.0f, 15.0f), "cycleBtn"));

	if (pCycleBtn) {
		pCycleBtn->setInheritsAlpha(false);
		pCycleBtn->setText("cycle");
		pCycleBtn->subscribeEvent(CEGUI::ToggleButton::EventMouseClick, CEGUI::Event::Subscriber(&walker::Interface::onCycleBtn, this));
	}

	// cycle combobox
	CEGUI::Combobox* pCycleCmb = static_cast<CEGUI::Combobox*>(walker::Gui::instance()->createWidget(
		m_pGuiBckScenario, "WindowsLook/Combobox", glm::vec4(0.0f), glm::vec4(70.0f, 80.0f, 160.0f, 200.0f), "cycleCmb"));

	if (pCycleCmb) {
		pCycleCmb->setEnabled(false);
		pCycleCmb->setReadOnly(true);
		pCycleCmb->setInheritsAlpha(false);
		pCycleCmb->setClippedByParent(false);
		pCycleCmb->setAutoSizeListHeightToContent(true);
		pCycleCmb->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted, CEGUI::Event::Subscriber(&walker::Interface::onCycleCmb, this));
	}

	// action button
	CEGUI::ToggleButton* pActionBtn = static_cast<CEGUI::ToggleButton*>(walker::Gui::instance()->createWidget(
		m_pGuiBckScenario, "WindowsLook/Checkbox", glm::vec4(0.0f), glm::vec4(5.0f, 120.0f, 55.0f, 15.0f), "actionBtn"));

	if (pActionBtn) {
		pActionBtn->setInheritsAlpha(false);
		pActionBtn->setText("action");
		pActionBtn->subscribeEvent(CEGUI::ToggleButton::EventMouseClick, CEGUI::Event::Subscriber(&walker::Interface::onActionBtn, this));
	}

	// action combobox
	CEGUI::Combobox* pActionCmb = static_cast<CEGUI::Combobox*>(walker::Gui::instance()->createWidget(
		m_pGuiBckScenario, "WindowsLook/Combobox", glm::vec4(0.0f), glm::vec4(70.0f, 115.0f, 160.0f, 200.0f), "actionCmb"));

	if (pActionCmb) {
		pActionCmb->setEnabled(false);
		pActionCmb->setReadOnly(true);
		pActionCmb->setInheritsAlpha(false);
		pActionCmb->setClippedByParent(false);
		pActionCmb->setAutoSizeListHeightToContent(true);
		pActionCmb->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted, CEGUI::Event::Subscriber(&walker::Interface::onActionCmb, this));
	}

	// init basic
	return pModelBtn && pModelCmb && pMeshBtn && pMeshCmb && pCycleBtn && pCycleCmb && pActionBtn && pActionCmb;
}

GLboolean walker::Interface::initAdvanced() {
	// meshes button
	CEGUI::RadioButton* pMeshesBtn = static_cast<CEGUI::RadioButton*>(walker::Gui::instance()->createWidget(
		m_pGuiBckScenario, "WindowsLook/RadioButton", glm::vec4(0.0f), glm::vec4(5.0f, 50.0f, 55.0f, 15.0f), "meshesBtn"));

	if (pMeshesBtn) {
		pMeshesBtn->setGroupID(4);
		pMeshesBtn->setInheritsAlpha(false);
		pMeshesBtn->setText("mesh");
		pMeshesBtn->setEnabled(false);
		pMeshesBtn->setVisible(false);
		pMeshesBtn->subscribeEvent(CEGUI::RadioButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&walker::Interface::onMeshesBtn, this));
	}

	// items button
	CEGUI::RadioButton* pItemsBtn = static_cast<CEGUI::RadioButton*>(walker::Gui::instance()->createWidget(
		m_pGuiBckScenario, "WindowsLook/RadioButton", glm::vec4(0.0f), glm::vec4(5.0f, 70.0f, 55.0f, 15.0f), "itemsBtn"));

	if (pItemsBtn) {
		pItemsBtn->setGroupID(4);
		pItemsBtn->setInheritsAlpha(false);
		pItemsBtn->setText("item");
		pItemsBtn->setEnabled(false);
		pItemsBtn->setVisible(false);
		pItemsBtn->subscribeEvent(CEGUI::RadioButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&walker::Interface::onItemsBtn, this));
	}

	// cycles button
	CEGUI::RadioButton* pCyclesBtn = static_cast<CEGUI::RadioButton*>(walker::Gui::instance()->createWidget(
		m_pGuiBckScenario, "WindowsLook/RadioButton", glm::vec4(0.0f), glm::vec4(5.0f, 90.0f, 55.0f, 15.0f), "cyclesBtn"));

	if (pCyclesBtn) {
		pCyclesBtn->setGroupID(4);
		pCyclesBtn->setInheritsAlpha(false);
		pCyclesBtn->setText("cycle");
		pCyclesBtn->setEnabled(false);
		pCyclesBtn->setVisible(false);
		pCyclesBtn->subscribeEvent(CEGUI::RadioButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&walker::Interface::onCyclesBtn, this));
	}

	// actions button
	CEGUI::RadioButton* pActionsBtn = static_cast<CEGUI::RadioButton*>(walker::Gui::instance()->createWidget(
		m_pGuiBckScenario, "WindowsLook/RadioButton", glm::vec4(0.0f), glm::vec4(5.0f, 110.0f, 55.0f, 15.0f), "actionsBtn"));

	if (pActionsBtn) {
		pActionsBtn->setGroupID(4);
		pActionsBtn->setInheritsAlpha(false);
		pActionsBtn->setText("action");
		pActionsBtn->setEnabled(false);
		pActionsBtn->setVisible(false);
		pActionsBtn->subscribeEvent(CEGUI::RadioButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&walker::Interface::onActionsBtn, this));
	}

	// advanced apply button
	CEGUI::PushButton* pApplyAdvBtn = static_cast<CEGUI::PushButton*>(walker::Gui::instance()->createWidget(
		m_pGuiBckScenario, "WindowsLook/Button", glm::vec4(0.0f), glm::vec4(5.0f, 135.0f, 55.0f, 30.0f), "applyAdvBtn"));

	if (pApplyAdvBtn) {
		pApplyAdvBtn->setText("apply");
		pApplyAdvBtn->setInheritsAlpha(false);
		pApplyAdvBtn->setEnabled(false);
		pApplyAdvBtn->setVisible(false);
		pApplyAdvBtn->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&walker::Interface::onApplyAdvBtn, this));
	}

	// init advanced
	return pMeshesBtn && pItemsBtn && pCyclesBtn && pActionsBtn && pApplyAdvBtn;
}

GLboolean walker::Interface::initTotal() {
	// list background
	CEGUI::FrameWindow* pBckList = static_cast<CEGUI::FrameWindow*>(walker::Gui::instance()->createWidget(
		m_pGuiBckScenario, "WindowsLook/FrameWindow", glm::vec4(0.0f), glm::vec4(70.0f, 45.0f, 160.0f, 380.0f), "bckListWnd"));

	if (pBckList) {
		pBckList->setInheritsAlpha(false);
		pBckList->setAlpha(0.75f);
		pBckList->setTitleBarEnabled(false);
		pBckList->setCloseButtonEnabled(false);
		pBckList->setEnabled(false);
		pBckList->setVisible(false);
		pBckList->setDragMovingEnabled(false);
		pBckList->setSizingEnabled(false);
		pBckList->setFrameEnabled(false);
	}

	// skeleton button
	CEGUI::RadioButton* pSkelViewBtn = static_cast<CEGUI::RadioButton*>(walker::Gui::instance()->createWidget(
		m_pGuiBckScenario, "WindowsLook/RadioButton", glm::vec4(0.0f), glm::vec4(240.0f, 50.0f, 55.0f, 15.0f), "skelViewBtn"));

	if (pSkelViewBtn) {
		pSkelViewBtn->setGroupID(5);
		pSkelViewBtn->setInheritsAlpha(false);
		pSkelViewBtn->setSelected(false);
		pSkelViewBtn->setText("bone");
		pSkelViewBtn->setEnabled(false);
		pSkelViewBtn->setVisible(false);
		pSkelViewBtn->subscribeEvent(CEGUI::RadioButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&walker::Interface::onDrawBtn, this));
	}

	// bounding box button
	CEGUI::RadioButton* pBndbViewBtn = static_cast<CEGUI::RadioButton*>(walker::Gui::instance()->createWidget(
		m_pGuiBckScenario, "WindowsLook/RadioButton", glm::vec4(0.0f), glm::vec4(240.0f, 70.0f, 55.0f, 15.0f), "bndbViewBtn"));

	if (pBndbViewBtn) {
		pBndbViewBtn->setGroupID(5);
		pBndbViewBtn->setInheritsAlpha(false);
		pBndbViewBtn->setSelected(false);
		pBndbViewBtn->setText("box");
		pBndbViewBtn->setEnabled(false);
		pBndbViewBtn->setVisible(false);
		pBndbViewBtn->subscribeEvent(CEGUI::RadioButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&walker::Interface::onDrawBtn, this));
	}

	// faces button
	CEGUI::RadioButton* pFaceViewBtn = static_cast<CEGUI::RadioButton*>(walker::Gui::instance()->createWidget(
		m_pGuiBckScenario, "WindowsLook/RadioButton", glm::vec4(0.0f), glm::vec4(240.0f, 90.0f, 55.0f, 15.0f), "faceViewBtn"));

	if (pFaceViewBtn) {
		pFaceViewBtn->setGroupID(5);
		pFaceViewBtn->setInheritsAlpha(false);
		pFaceViewBtn->setSelected(false);
		pFaceViewBtn->setText("wire");
		pFaceViewBtn->setEnabled(false);
		pFaceViewBtn->setVisible(false);
		pFaceViewBtn->subscribeEvent(CEGUI::RadioButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&walker::Interface::onDrawBtn, this));
	}

	// meshes button
	CEGUI::RadioButton* pMeshViewBtn = static_cast<CEGUI::RadioButton*>(walker::Gui::instance()->createWidget(
		m_pGuiBckScenario, "WindowsLook/RadioButton", glm::vec4(0.0f), glm::vec4(240.0f, 110.0f, 55.0f, 15.0f), "meshViewBtn"));

	if (pMeshViewBtn) {
		pMeshViewBtn->setGroupID(5);
		pMeshViewBtn->setInheritsAlpha(false);
		pMeshViewBtn->setSelected(true);
		pMeshViewBtn->setText("solid");
		pMeshViewBtn->setEnabled(false);
		pMeshViewBtn->setVisible(false);
		pMeshViewBtn->subscribeEvent(CEGUI::RadioButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&walker::Interface::onDrawBtn, this));
	}

	// advanced play button
	CEGUI::PushButton* pPlayAdvBtn = static_cast<CEGUI::PushButton*>(walker::Gui::instance()->createWidget(
		m_pGuiBckScenario, "WindowsLook/Button", glm::vec4(0.0f), glm::vec4(240.0f, 135.0f, 55.0f, 30.0f), "playAdvBtn"));

	if (pPlayAdvBtn) {
		pPlayAdvBtn->setText("play");
		pPlayAdvBtn->setInheritsAlpha(false);
		pPlayAdvBtn->setEnabled(false);
		pPlayAdvBtn->setVisible(false);
		pPlayAdvBtn->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&walker::Interface::onPlayAdvBtn, this));
	}

	// animation weight control slider
	CEGUI::Slider* pAniControlSld = static_cast<CEGUI::Slider*>(walker::Gui::instance()->createWidget(
		m_pRoot, "WindowsLook/Slider", glm::vec4(0.0f), glm::vec4(200.0f, 540.0f, 400.0f, 40.0f), "aniControlSld"));

	if (pAniControlSld) {
		pAniControlSld->setInheritsAlpha(false);
		pAniControlSld->setEnabled(false);
		pAniControlSld->setVisible(false);
		pAniControlSld->setMaxValue(1.0f);
		pAniControlSld->setCurrentValue(0.3f);
		pAniControlSld->setClickStep(0.1f);
		pAniControlSld->subscribeEvent(CEGUI::Slider::EventValueChanged, CEGUI::Event::Subscriber(&walker::Interface::onAniControlSld, this));
	}

	// init total
	return pBckList && pSkelViewBtn && pBndbViewBtn && pFaceViewBtn && pMeshViewBtn && pPlayAdvBtn && pAniControlSld;
}

GLboolean walker::Interface::initMixer() {
	// mixer background
	CEGUI::FrameWindow* pBckMixer = static_cast<CEGUI::FrameWindow*>(walker::Gui::instance()->createWidget(
		m_pGuiBckScenario, "WindowsLook/FrameWindow", glm::vec4(0.0f), glm::vec4(305.0f, 40.0f, 380.0f, 555.0f), "bckMixerWnd"));

	if (pBckMixer) {
		pBckMixer->setInheritsAlpha(false);
		pBckMixer->setAlpha(0.75f);
		pBckMixer->setTitleBarEnabled(false);
		pBckMixer->setCloseButtonEnabled(false);
		pBckMixer->setEnabled(false);
		pBckMixer->setVisible(false);
		pBckMixer->setDragMovingEnabled(false);
		pBckMixer->setSizingEnabled(false);
		pBckMixer->setFrameEnabled(false);
	}

	// cycles title
	CEGUI::DefaultWindow* pCycleLbl = static_cast<CEGUI::DefaultWindow*>(walker::Gui::instance()->createWidget(
		pBckMixer, "WindowsLook/Label", glm::vec4(0.0f), glm::vec4(0.0f, 5.0f, 370.0f, 20.0f), "cycleLbl"));

	if (pCycleLbl) {
		pCycleLbl->setInheritsAlpha(false);
		pCycleLbl->setText("cycles                   delay     weight     blend      scale");
	}

	// action title
	CEGUI::DefaultWindow* pActionLbl = static_cast<CEGUI::DefaultWindow*>(walker::Gui::instance()->createWidget(
		pBckMixer, "WindowsLook/Label", glm::vec4(0.0f), glm::vec4(0.0f, 25.0f, 360.0f, 20.0f), "actionLbl"));

	if (pActionLbl) {
		pActionLbl->setInheritsAlpha(false);
		pActionLbl->setText(" actions                 delay      intro       outro       time");
	}

	// init mixer
	return pBckMixer && pCycleLbl && pActionLbl;
}

GLboolean walker::Interface::fill() {
	// find comboboxes
	CEGUI::Combobox* pModelCmb = static_cast<CEGUI::Combobox*>(m_pGuiBckScenario->getChild("modelCmb"));
	CEGUI::Combobox* pMeshCmb = static_cast<CEGUI::Combobox*>(m_pGuiBckScenario->getChild("meshCmb"));
	CEGUI::Combobox* pCycleCmb = static_cast<CEGUI::Combobox*>(m_pGuiBckScenario->getChild("cycleCmb"));
	CEGUI::Combobox* pActionCmb = static_cast<CEGUI::Combobox*>(m_pGuiBckScenario->getChild("actionCmb"));
	CEGUI::FrameWindow* pBckList = static_cast<CEGUI::FrameWindow*>(m_pGuiBckScenario->getChild("bckListWnd"));

	// fill comboboxes
	if (pModelCmb && pMeshCmb && pCycleCmb && pActionCmb && pBckList) {
		// collect models
		std::vector<std::string> coreNames;
		if (!walker::Model::instance()->getCoreModelList(coreNames))
			return false;

		// add models
		for (auto& coreName : coreNames) {
			const CalCoreModel* pCoreModel = walker::Model::instance()->getCoreModel(coreName);
			const AssetsInfo* pAssetsInfo = walker::Model::instance()->getAssetsInfo(pCoreModel);
			if (pAssetsInfo) {
				// add models to the list
				CEGUI::ListboxTextItem* itemCombobox = new CEGUI::ListboxTextItem(coreName);
				if (itemCombobox) {
					itemCombobox->setTextColours(CEGUI::Colour(0.0f, 0.0f, 0.0f));
					pModelCmb->addItem(itemCombobox);

					// select first model as default
					if (coreName == coreNames.at(0)) {
						pModelCmb->setItemSelectState(itemCombobox, true);

						// add meshes to the list
						const std::vector<std::string>& meshes = pAssetsInfo->m_meshes;
						for (size_t i = 0; i < meshes.size(); ++i) {
							itemCombobox = new CEGUI::ListboxTextItem(meshes.at(i));
							if (itemCombobox) {
								itemCombobox->setTextColours(CEGUI::Colour(0.0f, 0.0f, 0.0f));
								pMeshCmb->addItem(itemCombobox);

								// advanced mesh button
								CEGUI::ToggleButton* pMeshBtn = static_cast<CEGUI::ToggleButton*>(walker::Gui::instance()->createWidget(pBckList,
									"WindowsLook/Checkbox", glm::vec4(0.0f), glm::vec4(7.0f, 5.0f + 20.0f * i, 150.0f, 15.0f), "meshBtn" + std::to_string(i)));

								if (pMeshBtn) {
									pMeshBtn->setInheritsAlpha(false);
									pMeshBtn->setText(meshes.at(i));
									pMeshBtn->setEnabled(true);
									pMeshBtn->setVisible(true);
									pMeshBtn->subscribeEvent(CEGUI::ToggleButton::EventMouseClick, CEGUI::Event::Subscriber(&walker::Interface::onMeshCmb, this));

									// append mesh
									const CalModel* pCalModel = m_pAgent->getBaseModel();
									const walker::MeshData* pMeshData = walker::Model::instance()->getMeshData(pCalModel->getCoreModel(), pMeshBtn->getText().c_str());
									if (pMeshData && pMeshData->m_append) {
										pMeshBtn->setSelected(true);
										pMeshCmb->setItemSelectState(itemCombobox, true);
									}

									// add mesh to the list
									m_pMeshes.push_back(pMeshBtn);
								}
							}
							else
								// no item
								return false;
						}

						// add items to the list
						const std::vector<std::string>& items = pAssetsInfo->m_items;
						for (size_t i = 0; i < items.size(); ++i) {
							// advanced item button
							CEGUI::ToggleButton* pItemBtn = static_cast<CEGUI::ToggleButton*>(walker::Gui::instance()->createWidget(pBckList,
								"WindowsLook/Checkbox", glm::vec4(0.0f), glm::vec4(7.0f, 5.0f + 20.0f * i, 150.0f, 15.0f), "itemBtn" + std::to_string(i)));

							if (pItemBtn) {
								pItemBtn->setInheritsAlpha(false);
								pItemBtn->setText(items.at(i));
								pItemBtn->setEnabled(false);
								pItemBtn->setVisible(false);
								pItemBtn->subscribeEvent(CEGUI::ToggleButton::EventMouseClick, CEGUI::Event::Subscriber(&walker::Interface::onMeshCmb, this));

								// append item
								const CalModel* pCalModel = m_pAgent->getBaseModel();
								const walker::MeshData* pMeshData = walker::Model::instance()->getMeshData(pCalModel->getCoreModel(), pItemBtn->getText().c_str());
								if (pMeshData && pMeshData->m_append) {
									pItemBtn->setSelected(true);
								}

								// add item to the list
								m_pItems.push_back(pItemBtn);
							}
						}

						// add cycles to the list
						const std::vector<std::string>& cycles = pAssetsInfo->m_cycles;
						for (size_t i = 0; i < cycles.size(); ++i) {
							itemCombobox = new CEGUI::ListboxTextItem(cycles.at(i));
							if (itemCombobox) {
								itemCombobox->setTextColours(CEGUI::Colour(0.0f, 0.0f, 0.0f));
								pCycleCmb->addItem(itemCombobox);

								// advanced cycle button
								CEGUI::ToggleButton* pCycleBtn = static_cast<CEGUI::ToggleButton*>(walker::Gui::instance()->createWidget(pBckList,
									"WindowsLook/Checkbox", glm::vec4(0.0f), glm::vec4(7.0f, 5.0f + 20.0f * i, 150.0f, 15.0f), "cycleBtn" + std::to_string(i)));

								if (pCycleBtn) {
									pCycleBtn->setInheritsAlpha(false);
									pCycleBtn->setText(cycles.at(i));
									pCycleBtn->setEnabled(false);
									pCycleBtn->setVisible(false);

									// add cycle to the list
									m_pCycles.push_back(pCycleBtn);
								}
							}
							else
								// no item
								return false;
						}

						// add actions to the list
						const std::vector<std::string>& actions = pAssetsInfo->m_actions;
						for (size_t i = 0; i < actions.size(); ++i) {
							itemCombobox = new CEGUI::ListboxTextItem(actions.at(i));
							if (itemCombobox) {
								itemCombobox->setTextColours(CEGUI::Colour(0.0f, 0.0f, 0.0f));
								pActionCmb->addItem(itemCombobox);

								// advanced action button
								CEGUI::ToggleButton* pActionBtn = static_cast<CEGUI::ToggleButton*>(walker::Gui::instance()->createWidget(pBckList,
									"WindowsLook/Checkbox", glm::vec4(0.0f), glm::vec4(7.0f, 5.0f + 20.0f * i, 150.0f, 15.0f), "actionBtn" + std::to_string(i)));

								if (pActionBtn) {
									pActionBtn->setInheritsAlpha(false);
									pActionBtn->setText(actions.at(i));
									pActionBtn->setEnabled(false);
									pActionBtn->setVisible(false);

									// add action to the list
									m_pActions.push_back(pActionBtn);
								}
							}
							else
								// no item
								return false;
						}
					}
				}
				else
					// no item
					return false;
			}
		}

		// default mesh
		CEGUI::RadioButton* pMeshesBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckScenario->getChild("meshesBtn"));
		if (pMeshesBtn) pMeshesBtn->setSelected(true);
	}

	// title label and progress bar close
	CEGUI::DefaultWindow* pTitleLbl = static_cast<CEGUI::DefaultWindow*>(m_pRoot->getChild("titleLbl"));
	CEGUI::ProgressBar* pProBar = static_cast<CEGUI::ProgressBar*>(m_pRoot->getChild("loadProBar"));
	if (pTitleLbl && pProBar) {
		pTitleLbl->setEnabled(false);
		pTitleLbl->setVisible(false);
		pProBar->setEnabled(false);
		pProBar->setVisible(false);
	}

	// select menu open
	m_pGuiBckSelect->setEnabled(true);
	m_pGuiBckSelect->setVisible(true);

	// fill
	return true;
}

walker::Agent* walker::Interface::addAgent() {
	// add new agent
	m_agents.push_back(std::make_shared<Agent>());
	return m_agents.back().get();
}

walker::Agent* walker::Interface::setAgent() {
	// set one agent
	m_pAgent = m_agents.front().get();
	return m_pAgent;
}

walker::Agent* walker::Interface::getAgent() const {
	return m_pAgent;
}

walker::WorkMode walker::Interface::getWorkMode() const {
	return m_workMode;
}

walker::RendMode walker::Interface::getRendMode() const {
	return m_rendMode;
}

GLfloat walker::Interface::getLookHeight() const {
	// look spinner
	CEGUI::Spinner* pLookSpn = static_cast<CEGUI::Spinner*>(m_pGuiBckSelect->getChild("lookSpn"));
	if (pLookSpn)
		return (GLfloat)pLookSpn->getCurrentValue();

	return 0.0f;
}

bool walker::Interface::onHelpBtn(const CEGUI::EventArgs& e) {
	// help background
	CEGUI::FrameWindow* m_pHelpBck = static_cast<CEGUI::FrameWindow*>(m_pRoot->getChild("bckHelpWnd"));
	if (m_pHelpBck) {
		m_pHelpBck->setEnabled(true);
		m_pHelpBck->setVisible(true);

		m_pGuiBckSelect->setEnabled(false);
		m_pGuiBckScenario->setEnabled(false);

		// help button
		CEGUI::PushButton* pHelpBtn = static_cast<CEGUI::PushButton*>(m_pRoot->getChild("helpBtn"));
		if (pHelpBtn) pHelpBtn->setEnabled(false);
	}

	return true;
}

bool walker::Interface::onCloseBtn(const CEGUI::EventArgs& e) {
	// help background
	CEGUI::FrameWindow* m_pHelpBck = static_cast<CEGUI::FrameWindow*>(m_pRoot->getChild("bckHelpWnd"));
	if (m_pHelpBck) {
		m_pHelpBck->setEnabled(false);
		m_pHelpBck->setVisible(false);

		m_pGuiBckSelect->setEnabled(true);
		m_pGuiBckScenario->setEnabled(true);

		// help button
		CEGUI::PushButton* pHelpBtn = static_cast<CEGUI::PushButton*>(m_pRoot->getChild("helpBtn"));
		if (pHelpBtn) pHelpBtn->setEnabled(true);
	}

	return true;
}

bool walker::Interface::onViewBtn(const CEGUI::EventArgs& e) {
	// view button
	CEGUI::RadioButton* pViewBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckSelect->getChild("viewBtn"));
	if (pViewBtn && pViewBtn->isSelected())
		// working mode
		m_workMode = WorkMode::CAMERA;

	m_pGuiBckScenario->setEnabled(false);
	m_pGuiBckScenario->setVisible(false);

	return true;
}

bool walker::Interface::onPathBtn(const CEGUI::EventArgs& e) {
	// path button
	CEGUI::RadioButton* pPathBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckSelect->getChild("pathBtn"));
	if (pPathBtn && pPathBtn->isSelected())
		// working mode
		m_workMode = WorkMode::PATH;

	return true;
}

bool walker::Interface::onSelectBtn(const CEGUI::EventArgs& e) {
	// select button
	CEGUI::RadioButton* pSelectBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckSelect->getChild("selectBtn"));
	CEGUI::RadioButton* pBasicBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckSelect->getChild("basicBtn"));
	CEGUI::RadioButton* pAdvancedBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckSelect->getChild("advancedBtn"));
	CEGUI::Slider* pAniControlSld = static_cast<CEGUI::Slider*>(m_pRoot->getChild("aniControlSld"));
	if (pSelectBtn && pBasicBtn && pAdvancedBtn && pAniControlSld) {
		// working mode
		m_workMode = WorkMode::NONE;

		// select button selection
		const GLboolean isSel = pSelectBtn->isSelected();

		pBasicBtn->setEnabled(isSel);
		pBasicBtn->setVisible(isSel);
		pAdvancedBtn->setEnabled(isSel);
		pAdvancedBtn->setVisible(isSel);
		m_pGuiBckScenario->setEnabled(isSel);
		m_pGuiBckScenario->setVisible(isSel);
		pAniControlSld->setEnabled(isSel);
		pAniControlSld->setVisible(isSel);

		CEGUI::RadioButton* pSkelViewBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckScenario->getChild("skelViewBtn"));
		CEGUI::RadioButton* pBndbViewBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckScenario->getChild("bndbViewBtn"));
		CEGUI::RadioButton* pFaceViewBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckScenario->getChild("faceViewBtn"));
		CEGUI::RadioButton* pMeshViewBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckScenario->getChild("meshViewBtn"));
		CEGUI::PushButton* pPlayAdvBtn = static_cast<CEGUI::PushButton*>(m_pGuiBckScenario->getChild("playAdvBtn"));
		if (pSkelViewBtn && pBndbViewBtn && pFaceViewBtn && pMeshViewBtn && pPlayAdvBtn) {
			pSkelViewBtn->setEnabled(!isSel);
			pSkelViewBtn->setVisible(!isSel);

			pBndbViewBtn->setEnabled(!isSel);
			pBndbViewBtn->setVisible(!isSel);

			pFaceViewBtn->setEnabled(!isSel);
			pFaceViewBtn->setVisible(!isSel);

			pMeshViewBtn->setEnabled(!isSel);
			pMeshViewBtn->setVisible(!isSel);

			pPlayAdvBtn->setEnabled(!isSel);
			pPlayAdvBtn->setVisible(!isSel);
		}
	}

	CEGUI::FrameWindow* pBckMixer = static_cast<CEGUI::FrameWindow*>(m_pGuiBckScenario->getChild("bckMixerWnd"));
	if (pBckMixer) {
		pBckMixer->setEnabled(false);
		pBckMixer->setVisible(false);
	}

	return true;
}

bool walker::Interface::onLookBtn(const CEGUI::EventArgs& e) {
	// look button, absolute, relative and spinner
	CEGUI::RadioButton* pLookBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckSelect->getChild("lookBtn"));
	CEGUI::RadioButton* pAbsLookBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckSelect->getChild("absLookBtn"));
	CEGUI::RadioButton* pRelLookBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckSelect->getChild("relLookBtn"));
	CEGUI::Spinner* pLookSpn = static_cast<CEGUI::Spinner*>(m_pGuiBckSelect->getChild("lookSpn"));
	if (pLookBtn && pAbsLookBtn && pRelLookBtn && pLookSpn) {
		// look button selection
		const GLboolean isSel = pLookBtn->isSelected();

		pLookSpn->setEnabled(isSel);
		pLookSpn->setVisible(isSel);

		pAbsLookBtn->setEnabled(isSel);
		pAbsLookBtn->setVisible(isSel);

		pRelLookBtn->setEnabled(isSel);
		pRelLookBtn->setVisible(isSel);

		if (isSel) {
			m_pAgent->setIsAbsLook(pAbsLookBtn->isSelected());
			m_pAgent->setIsRelLook(pRelLookBtn->isSelected());
		}
		else {
			m_pAgent->setIsAbsLook(false);
			m_pAgent->setIsRelLook(false);
		}
	}

	return true;
}

bool walker::Interface::onBasicBtn(const CEGUI::EventArgs& e) {
	// basic button
	CEGUI::RadioButton* pBasicBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckSelect->getChild("basicBtn"));

	CEGUI::ToggleButton* pMeshBtn = static_cast<CEGUI::ToggleButton*>(m_pGuiBckScenario->getChild("meshBtn"));
	CEGUI::ToggleButton* pCycleBtn = static_cast<CEGUI::ToggleButton*>(m_pGuiBckScenario->getChild("cycleBtn"));
	CEGUI::ToggleButton* pActionBtn = static_cast<CEGUI::ToggleButton*>(m_pGuiBckScenario->getChild("actionBtn"));

	CEGUI::Combobox* pMeshCmb = static_cast<CEGUI::Combobox*>(m_pGuiBckScenario->getChild("meshCmb"));
	CEGUI::Combobox* pCycleCmb = static_cast<CEGUI::Combobox*>(m_pGuiBckScenario->getChild("cycleCmb"));
	CEGUI::Combobox* pActionCmb = static_cast<CEGUI::Combobox*>(m_pGuiBckScenario->getChild("actionCmb"));
	if (pBasicBtn && pMeshBtn && pCycleBtn && pActionBtn && pMeshCmb && pCycleCmb && pActionCmb) {
		// basic button selection
		const GLboolean isSel = pBasicBtn->isSelected();

		pMeshBtn->setEnabled(isSel);
		pMeshBtn->setVisible(isSel);
		pCycleBtn->setEnabled(isSel);
		pCycleBtn->setVisible(isSel);
		pActionBtn->setEnabled(isSel);
		pActionBtn->setVisible(isSel);

		pMeshCmb->setEnabled(isSel);
		pMeshCmb->setVisible(isSel);
		pCycleCmb->setEnabled(isSel);
		pCycleCmb->setVisible(isSel);
		pActionCmb->setEnabled(isSel);
		pActionCmb->setVisible(isSel);
	}

	return true;
}

bool walker::Interface::onAdvancedBtn(const CEGUI::EventArgs& e) {
	// total button
	CEGUI::RadioButton* pAdvancedBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckSelect->getChild("advancedBtn"));
	CEGUI::RadioButton* pMeshesBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckScenario->getChild("meshesBtn"));
	CEGUI::RadioButton* pItemsBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckScenario->getChild("itemsBtn"));
	CEGUI::RadioButton* pCyclesBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckScenario->getChild("cyclesBtn"));
	CEGUI::RadioButton* pActionsBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckScenario->getChild("actionsBtn"));
	CEGUI::PushButton* pApplyAdvBtn = static_cast<CEGUI::PushButton*>(m_pGuiBckScenario->getChild("applyAdvBtn"));
	CEGUI::FrameWindow* m_pGuiBckList = static_cast<CEGUI::FrameWindow*>(m_pGuiBckScenario->getChild("bckListWnd"));
	if (pAdvancedBtn) {
		// advanced button selection
		const GLboolean isSel = pAdvancedBtn->isSelected();

		pMeshesBtn->setEnabled(isSel);
		pMeshesBtn->setVisible(isSel);
		pItemsBtn->setEnabled(isSel);
		pItemsBtn->setVisible(isSel);
		pCyclesBtn->setEnabled(isSel);
		pCyclesBtn->setVisible(isSel);
		pActionsBtn->setEnabled(isSel);
		pActionsBtn->setVisible(isSel);
		pApplyAdvBtn->setEnabled(isSel);
		pApplyAdvBtn->setVisible(isSel);
		m_pGuiBckList->setEnabled(isSel);
		m_pGuiBckList->setVisible(isSel);

		if (!isSel) {
			// skeleton, faces, meshes, play, mixer, cycle label
			CEGUI::RadioButton* pSkelViewBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckScenario->getChild("skelViewBtn"));
			CEGUI::RadioButton* pBndbViewBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckScenario->getChild("bndbViewBtn"));
			CEGUI::RadioButton* pFaceViewBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckScenario->getChild("faceViewBtn"));
			CEGUI::RadioButton* pMeshViewBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckScenario->getChild("meshViewBtn"));
			CEGUI::PushButton* pPlayAdvBtn = static_cast<CEGUI::PushButton*>(m_pGuiBckScenario->getChild("playAdvBtn"));
			CEGUI::FrameWindow* pBckMixer = static_cast<CEGUI::FrameWindow*>(m_pGuiBckScenario->getChild("bckMixerWnd"));
			if (pSkelViewBtn && pBndbViewBtn && pFaceViewBtn && pMeshViewBtn && pPlayAdvBtn && pBckMixer) {
				pSkelViewBtn->setEnabled(false);
				pSkelViewBtn->setVisible(false);

				pBndbViewBtn->setEnabled(false);
				pBndbViewBtn->setVisible(false);

				pFaceViewBtn->setEnabled(false);
				pFaceViewBtn->setVisible(false);

				pMeshViewBtn->setEnabled(false);
				pMeshViewBtn->setVisible(false);

				pPlayAdvBtn->setEnabled(false);
				pPlayAdvBtn->setVisible(false);

				pBckMixer->setEnabled(false);
				pBckMixer->setVisible(false);
			}
		}
	}

	return true;
}

bool walker::Interface::onAbsLookBtn(const CEGUI::EventArgs& e) {
	// look absolute
	CEGUI::RadioButton* pAbsLookBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckSelect->getChild("absLookBtn"));
	if (pAbsLookBtn && m_pAgent)
		m_pAgent->setIsAbsLook(pAbsLookBtn->isSelected());

	return true;
}

bool walker::Interface::onRelLookBtn(const CEGUI::EventArgs& e) {
	// look relative
	CEGUI::RadioButton* pRelLookBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckSelect->getChild("relLookBtn"));
	if (pRelLookBtn && m_pAgent)
		m_pAgent->setIsRelLook(pRelLookBtn->isSelected());

	return true;
}

bool walker::Interface::onModelBtn(const CEGUI::EventArgs& e) {
	// agent button
	CEGUI::ToggleButton* pModelBtn = static_cast<CEGUI::ToggleButton*>(m_pGuiBckScenario->getChild("modelBtn"));
	CEGUI::Combobox* pModelCmb = static_cast<CEGUI::Combobox*>(m_pGuiBckScenario->getChild("modelCmb"));
	if (pModelBtn && pModelCmb) {
		if (pModelBtn->isSelected()) {
			// agent set
			for (size_t i = 0; i < m_agents.size(); ++i) {
				// look for a right coremodel
				Agent* pAgent = m_agents.at(i).get();
				const CalModel* pCalModel = pAgent->getBaseModel();
				const CalCoreModel* pCoreModel = pCalModel->getCoreModel();
				if (pCoreModel->getName() == pModelCmb->getText()) {
					// agent enabled
					m_pAgent = pAgent;
					break;
				}
			}

			// agent set
		}
		else {
			// agent disabled
			m_pAgent = nullptr;
		}
	}

	return true;
}

bool walker::Interface::onModelCmb(const CEGUI::EventArgs& e) {
	// agent combobox
	CEGUI::Combobox* pModelCmb = static_cast<CEGUI::Combobox*>(m_pGuiBckScenario->getChild("modelCmb"));
	if (pModelCmb) {
		// agent set
		for (size_t i = 0; i < m_agents.size(); ++i) {
			// look for a right coremodel
			Agent* pAgent = m_agents.at(i).get();
			const CalModel* pCalModel = pAgent->getBaseModel();
			const CalCoreModel* pCoreModel = pCalModel->getCoreModel();
			if (pCoreModel->getName() == pModelCmb->getText()) {
				// assign new agent
				m_pAgent = pAgent;
				break;
			}
		}

		// comboboxes
		CEGUI::Combobox* pMeshCmb = static_cast<CEGUI::Combobox*>(m_pGuiBckScenario->getChild("meshCmb"));
		CEGUI::Combobox* pCycleCmb = static_cast<CEGUI::Combobox*>(m_pGuiBckScenario->getChild("cycleCmb"));
		CEGUI::Combobox* pActionCmb = static_cast<CEGUI::Combobox*>(m_pGuiBckScenario->getChild("actionCmb"));
		CEGUI::FrameWindow* pBckList = static_cast<CEGUI::FrameWindow*>(m_pGuiBckScenario->getChild("bckListWnd"));
		if (pMeshCmb && pCycleCmb && pActionCmb && pBckList) {
			// current model assets
			const CalCoreModel* pCoreModel = walker::Model::instance()->getCoreModel(pModelCmb->getText().c_str());
			const AssetsInfo* pAssetsInfo = walker::Model::instance()->getAssetsInfo(pCoreModel);
			if (pAssetsInfo) {
				// clear all comboboxes
				pMeshCmb->resetList();
				pCycleCmb->resetList();
				pActionCmb->resetList();

				// clear all containers
				for (size_t i = 0; i < m_pMeshes.size(); ++i) m_pMeshes.at(i)->destroy();
				for (size_t i = 0; i < m_pItems.size(); ++i) m_pItems.at(i)->destroy();
				for (size_t i = 0; i < m_pCycles.size(); ++i) m_pCycles.at(i)->destroy();
				for (size_t i = 0; i < m_pActions.size(); ++i) m_pActions.at(i)->destroy();
				m_pMeshes.clear();
				m_pItems.clear();
				m_pCycles.clear();
				m_pActions.clear();

				// list background size
				pBckList->setHeight(CEGUI::UDim(0.0f, 380.0f));

				// locals
				CEGUI::ListboxTextItem* itemCombobox = nullptr;

				// add meshes to the list
				const std::vector<std::string>& meshes = pAssetsInfo->m_meshes;
				for (size_t i = 0; i < meshes.size(); ++i) {
					itemCombobox = new CEGUI::ListboxTextItem(meshes.at(i));
					itemCombobox->setTextColours(CEGUI::Colour(0.0f, 0.0f, 0.0f));
					pMeshCmb->addItem(itemCombobox);

					// advanced mesh button
					CEGUI::ToggleButton* pMeshBtn = static_cast<CEGUI::ToggleButton*>(walker::Gui::instance()->createWidget(pBckList,
						"WindowsLook/Checkbox", glm::vec4(0.0f), glm::vec4(7.0f, 5.0f + 20.0f * i, 150.0f, 15.0f), "meshBtn" + std::to_string(i)));

					if (pMeshBtn) {
						pMeshBtn->setInheritsAlpha(false);
						pMeshBtn->setText(meshes.at(i));
						pMeshBtn->setEnabled(true);
						pMeshBtn->setVisible(true);
						pMeshBtn->subscribeEvent(CEGUI::ToggleButton::EventMouseClick, CEGUI::Event::Subscriber(&walker::Interface::onMeshCmb, this));

						// append mesh
						const CalModel* pCalModel = m_pAgent->getBaseModel();
						const walker::MeshData* pMeshData = walker::Model::instance()->getMeshData(pCalModel->getCoreModel(), pMeshBtn->getText().c_str());
						if (pMeshData && pMeshData->m_append) {
							pMeshBtn->setSelected(true);
							pMeshCmb->setItemSelectState(itemCombobox, true);
						}

						// add mesh to the list
						m_pMeshes.push_back(pMeshBtn);
					}
				}

				// add items to the list
				const std::vector<std::string>& items = pAssetsInfo->m_items;
				for (size_t i = 0; i < items.size(); ++i) {
					// advanced item button
					CEGUI::ToggleButton* pItemBtn = static_cast<CEGUI::ToggleButton*>(walker::Gui::instance()->createWidget(pBckList,
						"WindowsLook/Checkbox", glm::vec4(0.0f), glm::vec4(7.0f, 5.0f + 20.0f * i, 150.0f, 15.0f), "itemBtn" + std::to_string(i)));

					if (pItemBtn) {
						pItemBtn->setInheritsAlpha(false);
						pItemBtn->setSelected(false);
						pItemBtn->setText(items.at(i));
						pItemBtn->setEnabled(false);
						pItemBtn->setVisible(false);
						pItemBtn->subscribeEvent(CEGUI::ToggleButton::EventMouseClick, CEGUI::Event::Subscriber(&walker::Interface::onMeshCmb, this));

						// append item
						const CalModel* pCalModel = m_pAgent->getBaseModel();
						const walker::MeshData* pMeshData = walker::Model::instance()->getMeshData(pCalModel->getCoreModel(), pItemBtn->getText().c_str());
						if (pMeshData && pMeshData->m_append) {
							pItemBtn->setSelected(true);
						}

						// add item to the list
						m_pItems.push_back(pItemBtn);
					}
				}

				// add cycles to the list
				const std::vector<std::string>& cycles = pAssetsInfo->m_cycles;
				for (size_t i = 0; i < cycles.size(); ++i) {
					itemCombobox = new CEGUI::ListboxTextItem(cycles.at(i));
					itemCombobox->setTextColours(CEGUI::Colour(0.0f, 0.0f, 0.0f));
					pCycleCmb->addItem(itemCombobox);

					// advanced cycle button
					CEGUI::ToggleButton* pCycleBtn = static_cast<CEGUI::ToggleButton*>(walker::Gui::instance()->createWidget(pBckList,
						"WindowsLook/Checkbox", glm::vec4(0.0f), glm::vec4(7.0f, 5.0f + 20.0f * i, 150.0f, 15.0f), "cycleBtn" + std::to_string(i)));

					if (pCycleBtn) {
						pCycleBtn->setInheritsAlpha(false);
						pCycleBtn->setSelected(false);
						pCycleBtn->setText(cycles.at(i));
						pCycleBtn->setEnabled(false);
						pCycleBtn->setVisible(false);

						// add cycle to the list
						m_pCycles.push_back(pCycleBtn);
					}
				}

				// add actions to the list
				const std::vector<std::string>& actions = pAssetsInfo->m_actions;
				for (size_t i = 0; i < actions.size(); ++i) {
					itemCombobox = new CEGUI::ListboxTextItem(actions.at(i));
					itemCombobox->setTextColours(CEGUI::Colour(0.0f, 0.0f, 0.0f));
					pActionCmb->addItem(itemCombobox);

					// advanced action button
					CEGUI::ToggleButton* pActionBtn = static_cast<CEGUI::ToggleButton*>(walker::Gui::instance()->createWidget(pBckList,
						"WindowsLook/Checkbox", glm::vec4(0.0f), glm::vec4(7.0f, 5.0f + 20.0f * i, 150.0f, 15.0f), "actionBtn" + std::to_string(i)));

					if (pActionBtn) {
						pActionBtn->setInheritsAlpha(false);
						pActionBtn->setSelected(false);
						pActionBtn->setText(actions.at(i));
						pActionBtn->setEnabled(false);
						pActionBtn->setVisible(false);

						// add action to the list
						m_pActions.push_back(pActionBtn);
					}
				}

				// list background size
				pBckList->setHeight(CEGUI::UDim(0.0f, m_pMeshes.size() * 20.0f + 10.0f));

				// select mesh button
				CEGUI::RadioButton* pMeshesBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckScenario->getChild("meshesBtn"));
				if (pMeshesBtn) pMeshesBtn->setSelected(true);
			}
		}

		// skeleton, faces, meshes, play, mixer, cycle label
		CEGUI::RadioButton* pSkelViewBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckScenario->getChild("skelViewBtn"));
		CEGUI::RadioButton* pBndbViewBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckScenario->getChild("bndbViewBtn"));
		CEGUI::RadioButton* pFaceViewBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckScenario->getChild("faceViewBtn"));
		CEGUI::RadioButton* pMeshViewBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckScenario->getChild("meshViewBtn"));
		CEGUI::PushButton* pPlayAdvBtn = static_cast<CEGUI::PushButton*>(m_pGuiBckScenario->getChild("playAdvBtn"));
		CEGUI::FrameWindow* pBckMixer = static_cast<CEGUI::FrameWindow*>(m_pGuiBckScenario->getChild("bckMixerWnd"));
		if (pSkelViewBtn && pBndbViewBtn && pFaceViewBtn && pMeshViewBtn && pPlayAdvBtn && pBckMixer) {
			pSkelViewBtn->setEnabled(false);
			pSkelViewBtn->setVisible(false);

			pBndbViewBtn->setEnabled(false);
			pBndbViewBtn->setVisible(false);

			pFaceViewBtn->setEnabled(false);
			pFaceViewBtn->setVisible(false);

			pMeshViewBtn->setEnabled(false);
			pMeshViewBtn->setVisible(false);

			pPlayAdvBtn->setEnabled(false);
			pPlayAdvBtn->setVisible(false);

			pBckMixer->setEnabled(false);
			pBckMixer->setVisible(false);
		}
	}

	return true;
}

bool walker::Interface::onMeshBtn(const CEGUI::EventArgs& e) {
	// mesh button
	CEGUI::ToggleButton* pMeshBtn = static_cast<CEGUI::ToggleButton*>(m_pGuiBckScenario->getChild("meshBtn"));
	if (pMeshBtn) {
		//
		if (pMeshBtn->isSelected()) {
			// faces visible
			m_rendMode = RendMode::MESHES;
		}
		else {
			// lines visible
			m_rendMode = RendMode::SKELETON;
		}
	}

	return true;
}

bool walker::Interface::onMeshCmb(const CEGUI::EventArgs& e) {
	// mesh combobox
	CEGUI::RadioButton* pBasicBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckSelect->getChild("basicBtn"));
	CEGUI::RadioButton* pAdvancedBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckSelect->getChild("advancedBtn"));
	CEGUI::Combobox* pModelCmb = static_cast<CEGUI::Combobox*>(m_pGuiBckScenario->getChild("modelCmb"));
	CEGUI::Combobox* pMeshCmb = static_cast<CEGUI::Combobox*>(m_pGuiBckScenario->getChild("meshCmb"));
	if (pBasicBtn && pAdvancedBtn && pModelCmb && pMeshCmb) {
		// current model assets
		const std::string coreName = pModelCmb->getText().c_str();
		const CalCoreModel* pCoreModel = walker::Model::instance()->getCoreModel(coreName);
		const AssetsInfo* pAssetsInfo = walker::Model::instance()->getAssetsInfo(pCoreModel);
		if (pAssetsInfo) {
			// set all meshes for detachment
			for (size_t i = 0; i < pAssetsInfo->m_meshes.size(); ++i)
				walker::Model::instance()->setMeshAppend(coreName, pAssetsInfo->m_meshes[i], false);

			// set all items for detachment
			for (size_t i = 0; i < pAssetsInfo->m_items.size(); ++i)
				walker::Model::instance()->setMeshAppend(coreName, pAssetsInfo->m_items[i], false);

			// basic option
			if (pBasicBtn->isSelected()) {
				// append current mesh
				walker::Model::instance()->setMeshAppend(coreName, pMeshCmb->getText().c_str(), true);
			}
			// advanced option
			else if (pAdvancedBtn->isSelected()) {
				for (size_t i = 0; i < m_pMeshes.size(); ++i) {
					CEGUI::ToggleButton* pMeshBtn = m_pMeshes.at(i);
					if (pMeshBtn->isSelected())
						// append current mesh
						walker::Model::instance()->setMeshAppend(coreName, pMeshBtn->getText().c_str(), true);
				}

				for (size_t i = 0; i < m_pItems.size(); ++i) {
					CEGUI::ToggleButton* pItemBtn = m_pItems.at(i);
					if (pItemBtn->isSelected())
						// append current item
						walker::Model::instance()->setMeshAppend(coreName, pItemBtn->getText().c_str(), true);
				}
			}

			// attach current mesh
			walker::Model::instance()->detachMesh(m_pAgent->getBaseModel(), false);
			walker::Model::instance()->attachMesh(m_pAgent->getBaseModel());
		}
	}

	return true;
}

bool walker::Interface::onCycleBtn(const CEGUI::EventArgs& e) {
	// cycle button
	CEGUI::ToggleButton* pCycleBtn = static_cast<CEGUI::ToggleButton*>(m_pGuiBckScenario->getChild("cycleBtn"));
	CEGUI::Combobox* pCycleCmb = static_cast<CEGUI::Combobox*>(m_pGuiBckScenario->getChild("cycleCmb"));
	if (pCycleBtn && pCycleCmb) {
		if (pCycleBtn->isSelected()) {
			// apply cycle
			pCycleCmb->setEnabled(true);
			CEGUI::ListboxItem* pItem = pCycleCmb->getListboxItemFromIndex(0);
			pCycleCmb->setItemSelectState(pItem, true);

			const std::string cycleName(pItem->getText().c_str());
			const CalModel* pCalModel = m_pAgent->getBaseModel();
			const CalCoreModel* pCoreModel = pCalModel->getCoreModel();

			const walker::AnimationData* pAniData = walker::Model::instance()->getAnimationData(pCoreModel, cycleName);
			if (pAniData) {
				// cycle set
				ScenarioInfo scenario(cycleName);
				scenario.m_blendIn = pAniData->m_blend;

				// set scenario event
				if (!walker::Model::instance()->setScenarioInfo(pCoreModel->getName(), scenario))
					return false;
			}
		}
		else {
			// Remove active animation from the list
			walker::Model::instance()->clearCycle(m_pAgent->getBaseModel(), pCycleCmb->getText().c_str());

			// clear cycle
			pCycleCmb->setEnabled(false);
			pCycleCmb->clearAllSelections();
			pCycleCmb->setText("");
		}
	}

	return true;
}

bool walker::Interface::onCycleCmb(const CEGUI::EventArgs& e) {
	// cycle combobox
	CEGUI::RadioButton* pBasicBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckSelect->getChild("basicBtn"));
	CEGUI::Combobox* pCycleCmb = static_cast<CEGUI::Combobox*>(m_pGuiBckScenario->getChild("cycleCmb"));
	if (pBasicBtn && pCycleCmb) {
		// Remove all active animation from the list
		CalModel* pCalModel = m_pAgent->getBaseModel();
		walker::Model::instance()->clearCycle(pCalModel);

		// animation data
		const std::string cycleName(pCycleCmb->getText().c_str());
		const CalCoreModel* pCoreModel = pCalModel->getCoreModel();
		const walker::AnimationData* pAniData = walker::Model::instance()->getAnimationData(pCoreModel, cycleName);
		if (pAniData) {
			// cycle set
			ScenarioInfo scenario(cycleName);
			scenario.m_blendIn = pAniData->m_blend;

			// set scenario event
			if (!walker::Model::instance()->setScenarioInfo(pCoreModel->getName(), scenario))
				return false;

			// set speed
			m_pAgent->setSpeed(pAniData->m_speed);
		}
	}

	return true;
}

bool walker::Interface::onActionBtn(const CEGUI::EventArgs& e) {
	// action button
	CEGUI::ToggleButton* pActionBtn = static_cast<CEGUI::ToggleButton*>(m_pGuiBckScenario->getChild("actionBtn"));
	CEGUI::Combobox* pActionCmb = static_cast<CEGUI::Combobox*>(m_pGuiBckScenario->getChild("actionCmb"));
	if (pActionBtn && pActionCmb) {
		// CalModel
		CalModel* pCalModel = m_pAgent->getBaseModel();
		// apply action
		if (pActionBtn->isSelected()) {
			pActionCmb->setEnabled(true);
			CEGUI::ListboxItem* pItem = pActionCmb->getListboxItemFromIndex(0);
			pActionCmb->setItemSelectState(pItem, true);

			const std::string actionName(pItem->getText().c_str());
			const CalCoreModel* pCoreModel = pCalModel->getCoreModel();
			const GLint aniID = pCoreModel->getCoreAnimationId(actionName);
			const CalCoreAnimation* pCoreAni = pCoreModel->getCoreAnimation(aniID);
			const walker::AnimationData* pAniData = walker::Model::instance()->getAnimationData(pCoreModel, actionName);
			if (pAniData) {
				// action set
				ScenarioInfo scenario(actionName);
				scenario.m_blendIn = pAniData->m_blend;
				scenario.m_blendOut = pAniData->m_blend;
				scenario.m_duration = pCoreAni->getDuration();

				// set scenario event
				if (!walker::Model::instance()->setScenarioInfo(pCoreModel->getName(), scenario))
					return false;
			}
		}
		else {
			// clear action
			pActionCmb->setEnabled(false);
			pActionCmb->clearAllSelections();
			pActionCmb->setText("");

			walker::Model::instance()->clearAction(pCalModel);
		}
	}

	return true;
}

bool walker::Interface::onActionCmb(const CEGUI::EventArgs& e) {
	// action combobox
	CEGUI::RadioButton* pBasicBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckSelect->getChild("basicBtn"));
	CEGUI::Combobox* pActionCmb = static_cast<CEGUI::Combobox*>(m_pGuiBckScenario->getChild("actionCmb"));
	if (pBasicBtn && pActionCmb) {
		// animation data
		const std::string actionName(pActionCmb->getText().c_str());
		const CalModel* pCalModel = m_pAgent->getBaseModel();
		const CalCoreModel* pCoreModel = pCalModel->getCoreModel();
		const walker::AnimationData* pAniData = walker::Model::instance()->getAnimationData(pCoreModel, actionName);
		if (pAniData) {
			// action set
			ScenarioInfo scenario(actionName);
			scenario.m_blendIn = pAniData->m_blend;
			scenario.m_blendOut = pAniData->m_blend;

			// set scenario event
			if (!walker::Model::instance()->setScenarioInfo(pCoreModel->getName(), scenario))
				return false;
		}
	}

	return true;
}

bool walker::Interface::onMeshesBtn(const CEGUI::EventArgs& e) {
	// meshes button
	CEGUI::FrameWindow* pBckList = static_cast<CEGUI::FrameWindow*>(m_pGuiBckScenario->getChild("bckListWnd"));
	CEGUI::RadioButton* pMeshesBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckScenario->getChild("meshesBtn"));
	if (pBckList && pMeshesBtn) {
		// list background size
		pBckList->setHeight(CEGUI::UDim(0.0f, m_pMeshes.size() * 20.0f + 10.0f));

		for (size_t i = 0; i < m_pMeshes.size(); ++i) {
			CEGUI::ToggleButton* pMeshBtn = m_pMeshes.at(i);
			pMeshBtn->setEnabled(pMeshesBtn->isSelected());
			pMeshBtn->setVisible(pMeshesBtn->isSelected());
		}
	}

	return true;
}

bool walker::Interface::onItemsBtn(const CEGUI::EventArgs& e) {
	// items button
	CEGUI::FrameWindow* pBckList = static_cast<CEGUI::FrameWindow*>(m_pGuiBckScenario->getChild("bckListWnd"));
	CEGUI::RadioButton* pItemsBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckScenario->getChild("itemsBtn"));
	if (pBckList && pItemsBtn) {
		// list background size
		pBckList->setHeight(CEGUI::UDim(0.0f, m_pItems.size() * 20.0f + 10.0f));

		for (size_t i = 0; i < m_pItems.size(); ++i) {
			CEGUI::ToggleButton* pItemBtn = m_pItems.at(i);
			pItemBtn->setEnabled(pItemsBtn->isSelected());
			pItemBtn->setVisible(pItemsBtn->isSelected());
		}
	}

	return true;
}

bool walker::Interface::onCyclesBtn(const CEGUI::EventArgs& e) {
	// cycles button
	CEGUI::FrameWindow* pBckList = static_cast<CEGUI::FrameWindow*>(m_pGuiBckScenario->getChild("bckListWnd"));
	CEGUI::RadioButton* pCyclesBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckScenario->getChild("cyclesBtn"));
	if (pBckList && pCyclesBtn) {
		// list background size
		pBckList->setHeight(CEGUI::UDim(0.0f, m_pCycles.size() * 20.0f + 10.0f));

		for (size_t i = 0; i < m_pCycles.size(); ++i) {
			CEGUI::ToggleButton* pCycleBtn = m_pCycles.at(i);
			pCycleBtn->setEnabled(pCyclesBtn->isSelected());
			pCycleBtn->setVisible(pCyclesBtn->isSelected());
		}
	}

	return true;
}

bool walker::Interface::onActionsBtn(const CEGUI::EventArgs& e) {
	// actions button
	CEGUI::FrameWindow* pBckList = static_cast<CEGUI::FrameWindow*>(m_pGuiBckScenario->getChild("bckListWnd"));
	CEGUI::RadioButton* pActionsBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckScenario->getChild("actionsBtn"));
	if (pBckList && pActionsBtn) {
		// list background size
		pBckList->setHeight(CEGUI::UDim(0.0f, m_pActions.size() * 20.0f + 5.0f));

		for (size_t i = 0; i < m_pActions.size(); ++i) {
			CEGUI::ToggleButton* pActionBtn = m_pActions.at(i);
			pActionBtn->setEnabled(pActionsBtn->isSelected());
			pActionBtn->setVisible(pActionsBtn->isSelected());
		}
	}

	return true;
}

bool walker::Interface::onApplyAdvBtn(const CEGUI::EventArgs& e) {
	// clear gui scenario elements
	for (size_t i = 0; i < m_scenarioGui.size(); ++i) {
		ScenarioGui* pScGui = m_scenarioGui.at(i).get();
		if (pScGui->m_pActivateBtn) pScGui->m_pActivateBtn->destroy();
		if (pScGui->m_pDelaySpn) pScGui->m_pDelaySpn->destroy();
		if (pScGui->m_pWeightSpn) pScGui->m_pWeightSpn->destroy();
		if (pScGui->m_pScaleSpn) pScGui->m_pScaleSpn->destroy();
		if (pScGui->m_pBlendInSpn) pScGui->m_pBlendInSpn->destroy();
		if (pScGui->m_pBlendOutSpn) pScGui->m_pBlendOutSpn->destroy();
		if (pScGui->m_pDurationSpn) pScGui->m_pDurationSpn->destroy();
	}

	// clear gui scenario
	m_scenarioGui.clear();

	// mixer
	CEGUI::FrameWindow* pBckMixer = static_cast<CEGUI::FrameWindow*>(m_pGuiBckScenario->getChild("bckMixerWnd"));
	if (pBckMixer) {
		// skeleton, faces, meshes
		CEGUI::RadioButton* pSkelViewBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckScenario->getChild("skelViewBtn"));
		CEGUI::RadioButton* pBndbViewBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckScenario->getChild("bndbViewBtn"));
		CEGUI::RadioButton* pFaceViewBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckScenario->getChild("faceViewBtn"));
		CEGUI::RadioButton* pMeshViewBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckScenario->getChild("meshViewBtn"));
		if (pSkelViewBtn && pBndbViewBtn && pFaceViewBtn && pMeshViewBtn) {
			pSkelViewBtn->setEnabled(true);
			pSkelViewBtn->setVisible(true);

			pBndbViewBtn->setEnabled(true);
			pBndbViewBtn->setVisible(true);

			pFaceViewBtn->setEnabled(true);
			pFaceViewBtn->setVisible(true);

			pMeshViewBtn->setEnabled(true);
			pMeshViewBtn->setVisible(true);
		}

		CEGUI::PushButton* pPlayAdvBtn = static_cast<CEGUI::PushButton*>(m_pGuiBckScenario->getChild("playAdvBtn"));
		if (pPlayAdvBtn) {
			pPlayAdvBtn->setEnabled(true);
			pPlayAdvBtn->setVisible(true);
		}

		// list background size
		pBckMixer->setEnabled(true);
		pBckMixer->setVisible(true);
		pBckMixer->setHeight(CEGUI::UDim(0.0f, 555.0f));

		// cycles added
		GLuint cyCn = 0;
		for (size_t i = 0; i < m_pCycles.size(); ++i) {
			CEGUI::ToggleButton* pCycleBtn = m_pCycles.at(i);
			if (pCycleBtn->isSelected()) {
				// animation toggle
				CEGUI::ToggleButton* pCycleScBtn = static_cast<CEGUI::ToggleButton*>(walker::Gui::instance()->createWidget(pBckMixer,
					"WindowsLook/Checkbox", glm::vec4(0.0f), glm::vec4(10.0f, 25.0f + cyCn * 30.0f, 115.0f, 30.0f), "cycleScBtn" + std::to_string(cyCn)));

				if (pCycleScBtn) {
					pCycleScBtn->setInheritsAlpha(false);
					pCycleScBtn->setText(pCycleBtn->getText());
					pCycleScBtn->setSelected(true);
				}

				// delay spinner
				CEGUI::Spinner* pDelayScSpn = static_cast<CEGUI::Spinner*>(walker::Gui::instance()->createWidget(pBckMixer,
					"WindowsLook/Spinner", glm::vec4(0.0f), glm::vec4(130.0f, 25.0f + cyCn * 30.0f, 50.0f, 30.0f), "delayScSpn" + std::to_string(cyCn)));

				if (pDelayScSpn) {
					pDelayScSpn->setInheritsAlpha(false);
					pDelayScSpn->setTextInputMode(CEGUI::Spinner::FloatingPoint);
					pDelayScSpn->setMinimumValue(0.0);
					pDelayScSpn->setMaximumValue(30.0);
					pDelayScSpn->setCurrentValue(0.0);
					pDelayScSpn->setStepSize(0.1);
				}

				// weight spinner
				CEGUI::Spinner* pWeightScSpn = static_cast<CEGUI::Spinner*>(walker::Gui::instance()->createWidget(pBckMixer,
					"WindowsLook/Spinner", glm::vec4(0.0f), glm::vec4(190.0f, 25.0f + cyCn * 30.0f, 50.0f, 30.0f), "weightScSpn" + std::to_string(cyCn)));

				if (pWeightScSpn) {
					pWeightScSpn->setInheritsAlpha(false);
					pWeightScSpn->setTextInputMode(CEGUI::Spinner::FloatingPoint);
					pWeightScSpn->setMinimumValue(0.0);
					pWeightScSpn->setMaximumValue(1.0);
					pWeightScSpn->setCurrentValue(1.0);
					pWeightScSpn->setStepSize(0.1);
				}

				// blend in spinner
				CEGUI::Spinner* pBlendInScSpn = static_cast<CEGUI::Spinner*>(walker::Gui::instance()->createWidget(pBckMixer,
					"WindowsLook/Spinner", glm::vec4(0.0f), glm::vec4(250.0f, 25.0f + cyCn * 30.0f, 50.0f, 30.0f), "blendInScSpn" + std::to_string(cyCn)));

				const std::string cycleName(pCycleBtn->getText().c_str());
				const CalModel* pCalModel = m_pAgent->getBaseModel();
				const walker::AnimationData* pAniData = walker::Model::instance()->getAnimationData(pCalModel->getCoreModel(), cycleName);
				if (pBlendInScSpn && pAniData) {
					pBlendInScSpn->setInheritsAlpha(false);
					pBlendInScSpn->setTextInputMode(CEGUI::Spinner::FloatingPoint);
					pBlendInScSpn->setMinimumValue(0.0);
					pBlendInScSpn->setMaximumValue(5.0);
					pBlendInScSpn->setCurrentValue(pAniData->m_blend);
					pBlendInScSpn->setStepSize(0.1);
				}

				// scale spinner
				CEGUI::Spinner* pScaleScSpn = static_cast<CEGUI::Spinner*>(walker::Gui::instance()->createWidget(pBckMixer,
					"WindowsLook/Spinner", glm::vec4(0.0f), glm::vec4(310.0f, 25.0f + cyCn * 30.0f, 60.0f, 30.0f), "scaleScSpn" + std::to_string(cyCn)));

				if (pScaleScSpn) {
					pScaleScSpn->setInheritsAlpha(false);
					pScaleScSpn->setTextInputMode(CEGUI::Spinner::FloatingPoint);
					pScaleScSpn->setMinimumValue(0.01);
					pScaleScSpn->setMaximumValue(5.0);
					pScaleScSpn->setCurrentValue(1.0);
					pScaleScSpn->setStepSize(0.01);
				}

				// scenario
				ScenarioGui pScGui;
				pScGui.m_pActivateBtn = pCycleScBtn;
				pScGui.m_pDelaySpn = pDelayScSpn;
				pScGui.m_pWeightSpn = pWeightScSpn;
				pScGui.m_pScaleSpn = pScaleScSpn;
				pScGui.m_pBlendInSpn = pBlendInScSpn;

				// create gui scenario
				m_scenarioGui.push_back(std::make_shared<ScenarioGui>(pScGui));

				// count
				if (++cyCn > 15) return true;
			}
		}

		// action title
		CEGUI::DefaultWindow* pActionLbl = static_cast<CEGUI::DefaultWindow*>(pBckMixer->getChild("actionLbl"));
		if (pActionLbl) pActionLbl->setPosition(CEGUI::UVector2(CEGUI::UDim(0.0f, 0.0f), CEGUI::UDim(0.0f, 40.0f + cyCn * 30.0f)));

		// actions added
		for (size_t i = 0; i < m_pActions.size(); ++i) {
			CEGUI::ToggleButton* pActionBtn = m_pActions.at(i);
			if (pActionBtn->isSelected()) {
				// animation toggle
				CEGUI::ToggleButton* pActionScBtn = static_cast<CEGUI::ToggleButton*>(walker::Gui::instance()->createWidget(pBckMixer,
					"WindowsLook/Checkbox", glm::vec4(0.0f), glm::vec4(10.0f, 60.0f + cyCn * 30.0f, 115.0f, 30.0f), "actionScBtn" + std::to_string(cyCn)));

				if (pActionScBtn) {
					pActionScBtn->setInheritsAlpha(false);
					pActionScBtn->setText(pActionBtn->getText());
					pActionScBtn->setSelected(true);
				}

				// delay spinner
				CEGUI::Spinner* pDelayScSpn = static_cast<CEGUI::Spinner*>(walker::Gui::instance()->createWidget(pBckMixer,
					"WindowsLook/Spinner", glm::vec4(0.0f), glm::vec4(130.0f, 60.0f + cyCn * 30.0f, 50.0f, 30.0f), "delayScSpn" + std::to_string(cyCn)));

				if (pDelayScSpn) {
					pDelayScSpn->setInheritsAlpha(false);
					pDelayScSpn->setTextInputMode(CEGUI::Spinner::FloatingPoint);
					pDelayScSpn->setMinimumValue(0.0);
					pDelayScSpn->setMaximumValue(30.0);
					pDelayScSpn->setCurrentValue(0.0);
					pDelayScSpn->setStepSize(0.1);
				}

				// blend in spinner
				CEGUI::Spinner* pBlendInScSpn = static_cast<CEGUI::Spinner*>(walker::Gui::instance()->createWidget(pBckMixer,
					"WindowsLook/Spinner", glm::vec4(0.0f), glm::vec4(190.0f, 60.0f + cyCn * 30.0f, 50.0f, 30.0f), "blendInScSpn" + std::to_string(cyCn)));

				const std::string actionName(pActionBtn->getText().c_str());
				const CalModel* pCalModel = m_pAgent->getBaseModel();
				const walker::AnimationData* pAniData = walker::Model::instance()->getAnimationData(pCalModel->getCoreModel(), actionName);
				if (pBlendInScSpn && pAniData) {
					pBlendInScSpn->setInheritsAlpha(false);
					pBlendInScSpn->setTextInputMode(CEGUI::Spinner::FloatingPoint);
					pBlendInScSpn->setMinimumValue(0.0);
					pBlendInScSpn->setMaximumValue(5.0);
					pBlendInScSpn->setCurrentValue(pAniData->m_blend);
					pBlendInScSpn->setStepSize(0.1);
				}

				// blend out spinner
				CEGUI::Spinner* pBlendOutScSpn = static_cast<CEGUI::Spinner*>(walker::Gui::instance()->createWidget(pBckMixer,
					"WindowsLook/Spinner", glm::vec4(0.0f), glm::vec4(250.0f, 60.0f + cyCn * 30.0f, 50.0f, 30.0f), "blendOutScSpn" + std::to_string(cyCn)));

				if (pBlendOutScSpn && pAniData) {
					pBlendOutScSpn->setInheritsAlpha(false);
					pBlendOutScSpn->setTextInputMode(CEGUI::Spinner::FloatingPoint);
					pBlendOutScSpn->setMinimumValue(0.0);
					pBlendOutScSpn->setMaximumValue(5.0);
					pBlendOutScSpn->setCurrentValue(pAniData->m_blend);
					pBlendOutScSpn->setStepSize(0.1);
				}

				// duration spinner
				CEGUI::Spinner* pDurationScSpn = static_cast<CEGUI::Spinner*>(walker::Gui::instance()->createWidget(pBckMixer,
					"WindowsLook/Spinner", glm::vec4(0.0f), glm::vec4(310.0f, 60.0f + cyCn * 30.0f, 60.0f, 30.0f), "durationScSpn" + std::to_string(cyCn)));

				const CalCoreModel* pCoreModel = pCalModel->getCoreModel();
				const GLint aniID = pCoreModel->getCoreAnimationId(actionName);
				const CalCoreAnimation* pCoreAni = pCoreModel->getCoreAnimation(aniID);

				if (pDurationScSpn && pCoreAni) {
					pDurationScSpn->setInheritsAlpha(false);
					pDurationScSpn->setTextInputMode(CEGUI::Spinner::FloatingPoint);
					pDurationScSpn->setMinimumValue(0.0);
					pDurationScSpn->setMaximumValue(99.0);
					pDurationScSpn->setCurrentValue(pCoreAni->getDuration());
					pDurationScSpn->setStepSize(0.01);
				}

				// scenario
				ScenarioGui pScGui;
				pScGui.m_pActivateBtn = pActionScBtn;
				pScGui.m_pDelaySpn = pDelayScSpn;
				pScGui.m_pBlendInSpn = pBlendInScSpn;
				pScGui.m_pBlendOutSpn = pBlendOutScSpn;
				pScGui.m_pDurationSpn = pDurationScSpn;

				// create gui scenario
				m_scenarioGui.push_back(std::make_shared<ScenarioGui>(pScGui));

				// count
				if (++cyCn > 15) return true;
			}
		}

		// list background size
		pBckMixer->setHeight(CEGUI::UDim(0.0f, 75.0f + cyCn * 30.0f));
	}

	return true;
}

bool walker::Interface::onDrawBtn(const CEGUI::EventArgs& e) {
	// model render option
	CEGUI::RadioButton* pSkelViewBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckScenario->getChild("skelViewBtn"));
	CEGUI::RadioButton* pBndbViewBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckScenario->getChild("bndbViewBtn"));
	CEGUI::RadioButton* pFaceViewBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckScenario->getChild("faceViewBtn"));
	CEGUI::RadioButton* pMeshViewBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckScenario->getChild("meshViewBtn"));
	if (pSkelViewBtn && pBndbViewBtn && pFaceViewBtn && pMeshViewBtn) {
		//
		if (pSkelViewBtn->isSelected()) m_rendMode = RendMode::SKELETON;
		if (pBndbViewBtn->isSelected()) m_rendMode = RendMode::BOUNDBOX;
		if (pFaceViewBtn->isSelected()) m_rendMode = RendMode::FACES;
		if (pMeshViewBtn->isSelected()) m_rendMode = RendMode::MESHES;
	}

	return true;
}

bool walker::Interface::onPlayAdvBtn(const CEGUI::EventArgs& e) {
	// clear all existing animations
	CalModel* pCalModel = m_pAgent->getBaseModel();
	walker::Model::instance()->clearCycle(pCalModel);

	// animation weight & speed
	std::vector<std::pair<GLfloat, GLfloat>> vecWeightSpeed;

	// iterate through awaiting scenario events
	for (size_t i = 0; i < m_scenarioGui.size(); ++i) {
		//gui scenario element
		ScenarioGui* pScGui = m_scenarioGui.at(i).get();
		CEGUI::ToggleButton* pAniScBtn = pScGui->m_pActivateBtn;
		if (pAniScBtn->isSelected()) {
			const std::string aniName(pAniScBtn->getText().c_str());
			const CalModel* pCalModel = m_pAgent->getBaseModel();
			const CalCoreModel* pCoreModel = pCalModel->getCoreModel();
			const walker::AnimationData* pAniData = walker::Model::instance()->getAnimationData(pCoreModel, aniName);
			if (pAniData) {
				CEGUI::Spinner* pDel = pScGui->m_pDelaySpn;
				CEGUI::Spinner* pWei = pScGui->m_pWeightSpn;
				CEGUI::Spinner* pScl = pScGui->m_pScaleSpn;
				CEGUI::Spinner* pBin = pScGui->m_pBlendInSpn;
				CEGUI::Spinner* pBot = pScGui->m_pBlendOutSpn;
				CEGUI::Spinner* pDur = pScGui->m_pDurationSpn;

				// event
				ScenarioInfo scenario(aniName);
				if (pDel) scenario.m_delay = (GLfloat)pDel->getCurrentValue();
				if (pWei) scenario.m_weight = (GLfloat)pWei->getCurrentValue();
				if (pScl) scenario.m_scale = (GLfloat)pScl->getCurrentValue();
				if (pBin) scenario.m_blendIn = (GLfloat)pBin->getCurrentValue();
				if (pBot) scenario.m_blendOut = (GLfloat)pBot->getCurrentValue();
				if (pDur) scenario.m_duration = (GLfloat)pDur->getCurrentValue();

				// set scenario event
				if (!walker::Model::instance()->setScenarioInfo(pCoreModel->getName(), scenario))
					return false;

				// sum of weight and speed
				vecWeightSpeed.push_back(std::make_pair(scenario.m_weight, scenario.m_scale * pAniData->m_speed));
			}
		}
	}

	//  clear mixer
	CEGUI::RadioButton* pSkelViewBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckScenario->getChild("skelViewBtn"));
	CEGUI::RadioButton* pBndbViewBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckScenario->getChild("bndbViewBtn"));
	CEGUI::RadioButton* pFaceViewBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckScenario->getChild("faceViewBtn"));
	CEGUI::RadioButton* pMeshViewBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckScenario->getChild("meshViewBtn"));
	CEGUI::PushButton* pPlayAdvBtn = static_cast<CEGUI::PushButton*>(m_pGuiBckScenario->getChild("playAdvBtn"));
	if (pSkelViewBtn && pBndbViewBtn && pFaceViewBtn && pMeshViewBtn && pPlayAdvBtn) {
		pSkelViewBtn->setEnabled(false);
		pSkelViewBtn->setVisible(false);

		pBndbViewBtn->setEnabled(false);
		pBndbViewBtn->setVisible(false);

		pFaceViewBtn->setEnabled(false);
		pFaceViewBtn->setVisible(false);

		pMeshViewBtn->setEnabled(false);
		pMeshViewBtn->setVisible(false);

		pPlayAdvBtn->setEnabled(false);
		pPlayAdvBtn->setVisible(false);
	}

	// view button
	CEGUI::RadioButton* pViewBtn = static_cast<CEGUI::RadioButton*>(m_pGuiBckSelect->getChild("viewBtn"));
	if (pViewBtn)
		pViewBtn->setSelected(true);

	// blend control slider // TODO: normalize weights
	CEGUI::Slider* pAniControlSld = static_cast<CEGUI::Slider*>(m_pRoot->getChild("aniControlSld"));
	if (pAniControlSld && vecWeightSpeed.size()) {
		// iterare throught data for control slider
		GLfloat min = 999.0f, max = 0.0f, spd = 0.0f;
		for (size_t i = 0; i < vecWeightSpeed.size(); ++i) {
			std::pair<GLfloat, GLfloat>& pair = vecWeightSpeed[i];

			// set min and max speed value
			if (pair.second < min) min = pair.second;
			if (pair.second > max) max = pair.second;

			// currently assigned speed
			spd += pair.first * pair.second;
		}

		// set control slider
		pAniControlSld->setMaxValue(max - min);
		pAniControlSld->setCurrentValue(spd - min);
	}

	return true;
}

bool walker::Interface::onAniControlSld(const CEGUI::EventArgs& e) {
	// animation control slider
	CEGUI::Slider* pAniControlSld = static_cast<CEGUI::Slider*>(m_pRoot->getChild("aniControlSld"));
	if (pAniControlSld && m_pAgent) {
		// base calmodel
		CalModel* pCalModel = m_pAgent->getBaseModel();
		CalMixer* pCalMix = pCalModel->getMixer();
		const CalCoreModel* pCoreModel = pCalModel->getCoreModel();
		const std::list<CalAnimationCycle*>& calCycLst = pCalMix->getAnimationCycle();

		// current support number
		if (calCycLst.size() != 2)
			return false;

		// locals
		GLuint counter = 0;
		GLfloat speed[2] = { 0.0f, 0.0f };
		const walker::AnimationData* pAniData[2] = { nullptr, nullptr };
		ScenarioInfo* info[2] = { new ScenarioInfo(""), new ScenarioInfo("") };

		// cycle animation list
		std::list<CalAnimationCycle*>::const_iterator itCalCycLst = calCycLst.cbegin();
		while (itCalCycLst != calCycLst.cend()) {
			const CalAnimationCycle* pCalCycle = *itCalCycLst;
			const CalCoreAnimation* pCoreAnimation = pCalCycle->getCoreAnimation();

			// current animation aditional data
			const walker::AnimationData* pAniDataTmp = walker::Model::instance()->getAnimationData(pCoreModel, pCoreAnimation->getName());
			if (pAniDataTmp) {
				// set data
				speed[counter] = pAniDataTmp->m_speed;
				pAniData[counter] = pAniDataTmp;

				ScenarioInfo* infoTmp = info[counter];
				infoTmp->m_animation = pCoreAnimation->getName();
				infoTmp->m_blendIn = 0.5f;
				infoTmp->m_blendOut = 0.5f;
				infoTmp->m_delay = 0.0f;
				infoTmp->m_duration = pCoreAnimation->getDuration();
				infoTmp->m_scale = 1.0f;
				infoTmp->m_weight = 0.0f;
			}

			// Iterator
			++counter;
			++itCalCycLst;
		}

		// adjust speed order
		if (speed[1] < speed[0]) {
			GLfloat tmpSp = speed[0];
			speed[0] = speed[1];
			speed[1] = tmpSp;

			ScenarioInfo* tmpSc = info[0];
			info[0] = info[1];
			info[1] = tmpSc;
		}

		// current speed difference
		const GLfloat difSpeed = pAniControlSld->getCurrentValue();

		// set factors
		GLfloat fac[2];
		fac[1] = difSpeed / (speed[1] - speed[0]);
		fac[0] = 1.0f - fac[1];

		// apply changes to the cycle list
		walker::Model::instance()->clearCycle(pCalModel);
		for (size_t i = 0; i < sizeof(info) / sizeof(*info); ++i) {
			info[i]->m_weight = fac[i];
			walker::Model::instance()->assignCycle(pCalModel, info[i]);
			delete info[i];
		}

		// set modidied agent speed
		m_pAgent->setSpeed(speed[0] + difSpeed);
	}

	return true;
}

walker::Interface::ScenarioGui::ScenarioGui() :
	m_pActivateBtn(nullptr),
	m_pDelaySpn(nullptr),
	m_pWeightSpn(nullptr),
	m_pScaleSpn(nullptr),
	m_pBlendInSpn(nullptr),
	m_pBlendOutSpn(nullptr),
	m_pDurationSpn(nullptr) {}