// Agent
#include <glm\gtx\transform.hpp>
#include <glm\gtc\quaternion.hpp>
#include <cal3d\cal3d.h>
#include <math.h>

#include "Model.h"
#include "Agent.h"
#include "Interface.h"

#define _USE_MATH_DEFINES

walker::Agent::Agent() :
	m_isPath(false),
	m_position(0.0f, 0.0f, 0.0f),
	m_direction(0.0f, -1.0f, 0.0f),
	m_speed(0.0f),
	m_isAbsLook(false),
	m_isRelLook(false) {}

walker::Agent::~Agent() {
	// Detach all meshes from the models
	for (auto& model : m_models)
		walker::Model::instance()->detachMesh(model.get(), false);
}

GLvoid walker::Agent::setPosition(glm::vec3 position) {
	m_position = position;
}

GLvoid walker::Agent::setDirection(glm::vec3 direction) {
	m_direction = glm::normalize(direction);
}

GLvoid walker::Agent::setSpeed(GLfloat speed) {
	m_speed = speed;
}

GLvoid walker::Agent::setIsPath(GLboolean isPath) {
	m_isPath = isPath;
}

GLvoid walker::Agent::setIsAbsLook(GLboolean isLook) {
	m_isAbsLook = isLook;
}

GLvoid walker::Agent::setIsRelLook(GLboolean isLook) {
	m_isRelLook = isLook;
}

CalModel* walker::Agent::getBaseModel() {
	// Get the base type model
	return getModel(m_type);
}

glm::vec3 walker::Agent::getPosition() const {
	return m_position;
}

glm::vec3 walker::Agent::getDirection() const {
	return m_direction;
}

GLfloat walker::Agent::getRotation() {
	// Base model
	CalModel* pCalModel = getModel(m_type);

	// Local orientation
	return walker::Model::instance()->getDirectionAngle(pCalModel, m_direction);
}

GLfloat walker::Agent::getSpeed() const {
	return m_speed;
}

GLboolean walker::Agent::getIsPath() const {
	return m_isPath;
}

GLboolean walker::Agent::init(const std::string& coreModelName) {
	// Verify base model
	CalModel* pCalModel = getModel(coreModelName);
	if (!pCalModel)
		return false;

	// Mesh attachment
	if (!walker::Model::instance()->attachMesh(pCalModel))
		return false;

	// Remember model type
	m_type = coreModelName;

	// debug
	/*if (coreModelName == "wheelchair_adult") {
		const CalSkeleton* pCalSkeleton = pCalModel->getSkeleton();
		const CalCoreSkeleton* pCoreSkeleton = pCalSkeleton->getCoreSkeleton();
		const std::vector<CalCoreBone*>& corebones = pCoreSkeleton->getVectorCoreBone();
		for (size_t i = 0; i < corebones.size(); ++i) {
			const CalCoreBone* pCoreBone = corebones.at(i);
			std::cout << "bone[" << i << "]: " << pCoreBone->getName() << std::endl;
		}
	}*/

	// Init over
	return true;
}

GLboolean walker::Agent::play(const ScenarioInfo* pScInfo) {
	// Base CalModel
	CalModel* pCalModel = getModel(m_type);
	const CalCoreModel* pCoreModel = pCalModel->getCoreModel();

	// Assets
	const AssetsInfo* pAssetsInfo = walker::Model::instance()->getAssetsInfo(pCoreModel);
	if (pAssetsInfo) {
		// Find required cycle
		std::vector<std::string>::const_iterator itInfo;
		itInfo = std::find(pAssetsInfo->m_cycles.cbegin(), pAssetsInfo->m_cycles.cend(), pScInfo->m_animation);
		if (itInfo != pAssetsInfo->m_cycles.cend())
			// Assign cycle
			if (!walker::Model::instance()->assignCycle(pCalModel, pScInfo))
				return false;

		// Find required action
		itInfo = std::find(pAssetsInfo->m_actions.cbegin(), pAssetsInfo->m_actions.cend(), pScInfo->m_animation);
		if (itInfo != pAssetsInfo->m_actions.cend())
			// Assign action
			if (!walker::Model::instance()->assignAction(pCalModel, pScInfo))
				return false;

		// Additional model
		if (!addModel(pCalModel, pScInfo))
			return false;
	}

	// Play
	return true;
}

GLvoid walker::Agent::update(const glm::vec3 look, const GLfloat deltaTime) {
	// Base model
	CalModel* pCalModel = getModel(m_type);

	// Update all models
	std::list<std::shared_ptr<CalModel>>::iterator itCalModel = m_models.begin();
	while (itCalModel != m_models.end()) {
		// Additional model
		CalModel* pAddCalModel = itCalModel->get();

		// Confirm model status
		if (walker::Model::instance()->hasModelExpired(pCalModel, pAddCalModel)) {
			// Remove expired model
			m_models.erase(itCalModel++);
		}
		else {
			// Update meshes
			walker::Model::instance()->detachMesh(pAddCalModel, true);

			// Update actions
			walker::Model::instance()->loopAction(pAddCalModel, deltaTime);

			if (m_isAbsLook) {
				// Update character eyes orientation // TODO: configure model eyepoint
				const glm::vec3 eyePos = m_position + glm::vec3(0.0f, 0.0f, 1.55f);
				const glm::vec3 aim = glm::normalize(look - eyePos);

				// Update absolute skeleton adjustment
				walker::Model::instance()->absBoneAdjustment(pAddCalModel, m_direction, aim);
			}

			if (m_isRelLook)
				// Update relative skeleton adjustment
				walker::Model::instance()->relBoneAdjustment(pAddCalModel, m_speed, glm::atan(look.x, -look.y), deltaTime);

			// Update CalModel
			pAddCalModel->update(deltaTime);

			if (m_isAbsLook || m_isRelLook)
				// Reset skeleton adjustment
				pAddCalModel->getMixer()->removeAllBoneAdjustments();

			// Iterator
			++itCalModel;
		}
	}
}

GLvoid walker::Agent::draw(const RendMode mode, const GLuint programID) {
	// All models
	for (auto& model : m_models) {
		// CalModel
		CalModel* pCalModel = model.get();

		// Render
		switch (mode) {
		case RendMode::MESHES:
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			walker::Model::instance()->drawMesh(programID, pCalModel);
			break;
		case RendMode::FACES:
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			walker::Model::instance()->drawMesh(programID, pCalModel);
			break;
		case RendMode::BOUNDBOX:
			walker::Model::instance()->drawBoundingBox(pCalModel->getSkeleton());
			break;
		case RendMode::SKELETON:
			walker::Model::instance()->drawSkeleton(pCalModel->getSkeleton());
			break;
		case RendMode::NONE:
			break;
		default:
			break;
		}
	}
}

GLboolean walker::Agent::addModel(CalModel* pCalModel, const ScenarioInfo* pScInfo) {
	// Add additional model if required by animation
	const CalCoreModel* pCoreModel = pCalModel->getCoreModel();
	const walker::AnimationData* pAniData = walker::Model::instance()->getAnimationData(pCoreModel, pScInfo->m_animation);
	if (pAniData) {
		const walker::CompoundData* pComData = walker::Model::instance()->getCompoundData(pCoreModel, pAniData->m_compound);
		if (pComData) {
			// Additional model
			CalModel* pAddCalModel = getModel(pComData->m_skeleton);
			if (pAddCalModel) {
				// Cycle component
				if (!pComData->m_cycle.empty()) {
					const ScenarioInfo addInfo(pComData->m_cycle);
					// Assign cycle
					if (walker::Model::instance()->hasModelStarted(pCalModel, pAddCalModel, &addInfo))
						// Recursive call
						addModel(pAddCalModel, &addInfo);
				}

				// Action component
				if (!pComData->m_action.empty()) {
					ScenarioInfo addInfo(pComData->m_action);
					// Find animation duration
					const CalCoreModel* pAddCalCoreModel = pAddCalModel->getCoreModel();
					const GLint animationID = pAddCalCoreModel->getCoreAnimationId(pComData->m_action);
					const CalCoreAnimation* pCalCoreAnimation = pAddCalCoreModel->getCoreAnimation(animationID);
					if (pCalCoreAnimation)
						addInfo.m_duration = pCalCoreAnimation->getDuration();

					// Assign action
					if (walker::Model::instance()->hasModelStarted(pCalModel, pAddCalModel, &addInfo))
						// Recursive call
						addModel(pAddCalModel, &addInfo);
				}
			}
		}
	}

	// Add model
	return true;
}

CalModel* walker::Agent::getModel(const std::string& coreModelName) {
	// Required coremodel
	CalCoreModel* pAddCoreModel = walker::Model::instance()->getCoreModel(coreModelName);
	if (pAddCoreModel) {
		// Find existing model
		for (auto& model : m_models) {
			// iterate throughout existing models
			const CalCoreModel* pCalCoreModel = model->getCoreModel();
			if (pCalCoreModel->getName() == pAddCoreModel->getName())
				// Model already added
				return model.get();
		}

		// Add additional model to the list
		m_models.emplace_back(std::make_shared<CalModel>(pAddCoreModel));

		// New model created
		return m_models.back().get();
	}

	// No model found
	return nullptr;
}
