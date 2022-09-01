// Model
#include <iostream>

#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>
#include <glm\gtc\quaternion.hpp>
#include <cal3d\cal3d.h>

#include "Model.h"
#include "Loader.h"
#include "Gui.h"

walker::GeometryInfo::GeometryInfo(GLfloat scale, glm::ivec2 direction)
	: m_scale(scale), m_direction(direction) {}

walker::ScenarioInfo::ScenarioInfo(const std::string& animation)
	: m_animation(animation), m_delay(0.0f), m_weight(1.0f), m_scale(1.0f), m_blendIn(0.0f), m_blendOut(0.0f), m_duration(0.0f) {}

walker::MeshData::MeshData(GLboolean append, GLuint texID, const std::string& texName)
	: m_append(append), m_texID(texID), m_texName(texName) {}

walker::MaterialData::MaterialData(GLboolean append, GLuint texID, const std::string& texName)
	: m_append(append), m_texID(texID), m_texName(texName) {}

walker::AnimationData::AnimationData(GLfloat weight, GLfloat speed, GLfloat blend, const std::string& mesh, const std::string& compound)
	: m_weight(weight), m_speed(speed), m_blend(blend), m_mesh(mesh), m_compound(compound) {}

walker::CompoundData::CompoundData(const std::string &skeleton, const std::string& cycle, const std::string& action)
	: m_skeleton(skeleton), m_cycle(cycle), m_action(action) {}

// Cal3DMgr singleton class instance pointer ----------
std::unique_ptr<walker::Model> walker::Model::m_pModel;
// ----------------------------------------------------

// Destructor
walker::Model::~Model() {
	glDeleteBuffers(MOD_NUM_BUFFERS, m_vbo);
	glDeleteVertexArrays(1, &m_vao);
}

// Returns a pointer to the Cal3DMgr singleton
walker::Model* walker::Model::instance() {
	if (!m_pModel) {
		// Allocate memory for Cal3DMgr
		m_pModel = std::unique_ptr<walker::Model>(new walker::Model());
	}

	return m_pModel.get();
}

// Creates the core model instance
CalCoreModel* walker::Model::createCoreModel(const std::string& coreModelName) {
	if (m_calCoreModelMap.find(coreModelName) != m_calCoreModelMap.end()) {
		// Core model already exists under that name
		return nullptr;
	}

	// Core model data
	CalCoreModelData& rCoreModelData = m_calCoreModelMap[coreModelName];

	// Allocate memory for the new core model
	rCoreModelData.m_calCoreModel = std::make_shared<CalCoreModel>(coreModelName);
	return rCoreModelData.m_calCoreModel.get();
}

// Removes the core model instance
GLvoid walker::Model::removeCoreModel(const std::string& coreModelName) {
	m_calCoreModelMap.erase(coreModelName);
}

// Store graphics data
GLboolean walker::Model::setGeometryInfo(const std::string& coreModelName, const GeometryInfo& geometryInfo) {
	// Looking for requested core model
	if (m_calCoreModelMap.find(coreModelName) == m_calCoreModelMap.end()) {
		// Core model not found
		return false;
	}

	// Create graphics data
	m_calCoreModelMap[coreModelName].m_geometryInfo = std::make_shared<GeometryInfo>(geometryInfo.m_scale, geometryInfo.m_direction);

	// Graphics
	return true;
}

// Store assets info
GLboolean walker::Model::setAssetsInfo(const std::string& coreModelName, const AssetType type, const std::string& asset) {
	// Looking for requested core model
	if (m_calCoreModelMap.find(coreModelName) == m_calCoreModelMap.end()) {
		// Core model not found
		return false;
	}

	// Core model data
	CalCoreModelData& rCoreModelData = m_calCoreModelMap[coreModelName];

	// Assets memory allocation
	if (!rCoreModelData.m_assetsInfo)
		rCoreModelData.m_assetsInfo = std::make_shared<AssetsInfo>();

	// Local pointers
	AssetsInfo* pAssetInfo = rCoreModelData.m_assetsInfo.get();
	std::vector<std::string>* pAssets = nullptr;

	// Asset type
	switch (type) {
	case AssetType::MESH:
		pAssets = &pAssetInfo->m_meshes;
		break;
	case AssetType::ITEM:
		pAssets = &pAssetInfo->m_items;
		break;
	case AssetType::MATERIAL:
		pAssets = &pAssetInfo->m_materials;
		break;
	case AssetType::TEXTURE:
		pAssets = &pAssetInfo->m_textures;
		break;
	case AssetType::CYCLE:
		pAssets = &pAssetInfo->m_cycles;
		break;
	case AssetType::ACTION:
		pAssets = &pAssetInfo->m_actions;
		break;
	default:
		return false;
	}

	// Storing asset
	pAssets->push_back(asset);

	// Asset in
	return true;
}

// Store scenario info
GLboolean walker::Model::setScenarioInfo(const std::string& coreModelName, const ScenarioInfo& scenarioInfo) {
	// Looking for requested core model
	if (m_calCoreModelMap.find(coreModelName) == m_calCoreModelMap.end()) {
		// Core model not found
		return false;
	}

	// Reference to scenario event list
	std::list<std::shared_ptr<ScenarioInfo>>& rScInfoLst = m_calCoreModelMap[coreModelName].m_scenarioInfoLst;

	// Set new scenario event
	rScInfoLst.push_back(std::make_shared<ScenarioInfo>(scenarioInfo.m_animation));

	// Scenario info
	ScenarioInfo* pScenInfo = rScInfoLst.back().get();
	pScenInfo->m_delay = scenarioInfo.m_delay;
	pScenInfo->m_weight = scenarioInfo.m_weight;
	pScenInfo->m_scale = scenarioInfo.m_scale;
	pScenInfo->m_blendIn = scenarioInfo.m_blendIn;
	pScenInfo->m_blendOut = scenarioInfo.m_blendOut;
	pScenInfo->m_duration = scenarioInfo.m_duration;

	// Scenario
	return true;
}

// Store mesh data struct
GLboolean walker::Model::setMeshData(const std::string& coreModelName, const std::string& meshName, const MeshData& meshData) {
	// Looking for requested core model
	if (m_calCoreModelMap.find(coreModelName) == m_calCoreModelMap.end()) {
		// Core model not found
		return false;
	}

	// Core model data
	CalCoreModelData& rCoreModelData = m_calCoreModelMap[coreModelName];

	// Looking for requested mesh
	if (rCoreModelData.m_meshMap.find(meshName) != rCoreModelData.m_meshMap.end()) {
		// Mesh already exist under specified name
		return false;
	}

	// Create mesh data
	rCoreModelData.m_meshMap[meshName] = std::make_shared<walker::MeshData>(meshData.m_append, meshData.m_texID, meshData.m_texName);

	// Mesh
	return true;
}

// Set mesh appendance
GLboolean walker::Model::setMeshAppend(const std::string& coreModelName, const std::string& meshName, GLboolean append) {
	// Looking for requested core model
	if (m_calCoreModelMap.find(coreModelName) == m_calCoreModelMap.end()) {
		// Core model not found
		return false;
	}

	// Core model data
	CalCoreModelData& rCoreModelData = m_calCoreModelMap[coreModelName];

	// Looking for requested mesh
	if (rCoreModelData.m_meshMap.find(meshName) == rCoreModelData.m_meshMap.end()) {
		// Mesh doesn't exist under specified name
		return false;
	}

	// Set mesh appendance
	rCoreModelData.m_meshMap[meshName]->m_append = append;

	// Append
	return true;
}

// Store material data struct
GLboolean walker::Model::setMaterialData(const std::string& coreModelName, const std::string& materialName, const MaterialData& materialData) {
	// Looking for requested core model
	if (m_calCoreModelMap.find(coreModelName) == m_calCoreModelMap.end()) {
		// Core model not found
		return false;
	}

	// Core model data
	CalCoreModelData& rCoreModelData = m_calCoreModelMap[coreModelName];

	// Looking for requested material
	if (rCoreModelData.m_materialMap.find(materialName) != rCoreModelData.m_materialMap.end()) {
		// Material already exist under specified name
		return false;
	}

	// Create material data
	rCoreModelData.m_materialMap[materialName]
		= std::make_shared<walker::MaterialData>(materialData.m_append, materialData.m_texID, materialData.m_texName);

	// Material
	return true;
}

// Store texture data
GLboolean walker::Model::setTextureID(const CalCoreModel* calCoreModel, const std::string& texName, GLuint texID) {
	// Looking for requested core model
	if (m_calCoreModelMap.find(calCoreModel->getName()) == m_calCoreModelMap.end()) {
		// Core model not found
		return false;
	}

	// Core model data
	CalCoreModelData& rCoreModelData = m_calCoreModelMap[calCoreModel->getName()];

	// Type of texture apply
	if (m_useMaterial) {
		// Apply texture to selected materials
		for (GLint i = 0; i < calCoreModel->getCoreMaterialCount(); ++i) {
			// Core material
			const CalCoreMaterial* coreMaterial = calCoreModel->getCoreMaterial(i);
			const std::string& materialName = coreMaterial->getName();
			const walker::MaterialData* pMaterialData = getMaterialData(calCoreModel, materialName);
			if (pMaterialData && pMaterialData->m_texName == texName) {
				// Set mesh data texture ID
				rCoreModelData.m_materialMap[materialName]->m_texID = texID;
			}

			// Apply material
			/*if (coreMaterial && pMaterialData->m_append) {
			// loop through all maps of the core material
			for (GLint mapId = 0; mapId < coreMaterial->getMapCount(); ++mapId)
			// store the opengl texture id in the user data of the map
			coreMaterial->setMapUserData(mapId, (Cal::UserData)texID);

			// create the a material thread
			coreMaterial->createCoreMaterialThread(i);
			// initialize the material thread
			coreMaterial->setCoreMaterialId(i, 0, i);
			}*/
		}
	}
	else {
		// Apply texture to selected meshes
		for (GLint i = 0; i < calCoreModel->getCoreMeshCount(); ++i) {
			// Core mesh
			const CalCoreMesh* coreMesh = calCoreModel->getCoreMesh(i);
			const std::string& meshName = coreMesh->getName();
			const walker::MeshData* pMeshData = getMeshData(calCoreModel, meshName);
			if (pMeshData && pMeshData->m_texName == texName) {
				// Set mesh data texture ID
				rCoreModelData.m_meshMap[meshName]->m_texID = texID;
			}
		}
	}

	// Texture
	return true;
}

// Store animation data struct
GLboolean walker::Model::setAnimationData(const std::string& coreModelName, const std::string& animationName, const walker::AnimationData& animationData) {
	// Looking for requested core model
	if (m_calCoreModelMap.find(coreModelName) == m_calCoreModelMap.end()) {
		// Core model doesn't exist under specified name
		return false;
	}

	// Core model data
	CalCoreModelData& rCoreModelData = m_calCoreModelMap[coreModelName];

	// Looking for requested animation
	if (rCoreModelData.m_animationMap.find(animationName) != rCoreModelData.m_animationMap.end()) {
		// Animation already exist under specified name
		return false;
	}

	// Create animation data
	rCoreModelData.m_animationMap[animationName] = std::make_shared<walker::AnimationData>(
			animationData.m_weight,
			animationData.m_speed,
			animationData.m_blend,
			animationData.m_mesh,
			animationData.m_compound);

	// Animation data stored
	return true;
}

// Store compound data struct
GLboolean walker::Model::setCompoundData(const std::string& coreModelName, const std::string& compoundName, const CompoundData& compoundData) {
	// Looking for requested core model
	if (m_calCoreModelMap.find(coreModelName) == m_calCoreModelMap.end()) {
		// Core model doesn't exist under specified name
		return false;
	}

	// Core model data
	CalCoreModelData& rCoreModelData = m_calCoreModelMap[coreModelName];

	// Looking for requested compound
	if (rCoreModelData.m_compoundMap.find(compoundName) != rCoreModelData.m_compoundMap.end()) {
		// Compound already exist under specified name
		return false;
	}

	// Create compound data
	rCoreModelData.m_compoundMap[compoundName]
		= std::make_shared<walker::CompoundData>(compoundData.m_skeleton, compoundData.m_cycle, compoundData.m_action);

	// Compound data stored
	return true;
}

// Set action for repeat
GLboolean walker::Model::setRepeatAction(const std::string& coreModelName, const std::string& animationName, const GLfloat duration) {
	// Looking for requested core model
	if (m_calCoreModelMap.find(coreModelName) == m_calCoreModelMap.end()) {
		// Core model doesn't exist under specified name
		return false;
	}

	// Core model data
	CalCoreModelData& rCoreModelData = m_calCoreModelMap[coreModelName];

	// Looking for requested compound
	if (rCoreModelData.m_repActMap.find(animationName) != rCoreModelData.m_repActMap.end()) {
		// Action already exist under specified name
		return false;
	}

	// Create action repeat data
	rCoreModelData.m_repActMap[animationName] = duration;

	// Action set
	return true;
}

// Returns core model list
GLboolean walker::Model::getCoreModelList(std::vector<std::string>& coreModels) {
	// No core models
	if (m_calCoreModelMap.empty())
		return false;

	// Iterate through core models
	for (auto& itCoreMap : m_calCoreModelMap)
		// Add core model name to the list
		coreModels.push_back(itCoreMap.first);

	// model list
	return true;
}

// Returns a pointer to the core model instance
CalCoreModel* walker::Model::getCoreModel(const std::string& coreModelName) {
	if (m_calCoreModelMap.find(coreModelName) == m_calCoreModelMap.end()) {
		// Core model doesn't exist under specified name
		return nullptr;
	}

	return m_calCoreModelMap[coreModelName].m_calCoreModel.get();
}

// Returns a const pointer to the graphics info struct
const walker::GeometryInfo* walker::Model::getGeometryInfo(const CalCoreModel* calCoreModel) {
	// Looking for requested core model
	if (m_calCoreModelMap.find(calCoreModel->getName()) == m_calCoreModelMap.end()) {
		// Core model not found
		return nullptr;
	}

	// return selected animation data
	return m_calCoreModelMap[calCoreModel->getName()].m_geometryInfo.get();
}

// Returns a const pointer to the assets info struct
const walker::AssetsInfo* walker::Model::getAssetsInfo(const CalCoreModel* calCoreModel) {
	// Looking for requested core model
	if (m_calCoreModelMap.find(calCoreModel->getName()) == m_calCoreModelMap.end()) {
		// Core model not found
		return nullptr;
	}

	// return selected model data
	return m_calCoreModelMap[calCoreModel->getName()].m_assetsInfo.get();
}

// Returns a const pointer to the (next) scenario info struct
const walker::ScenarioInfo* walker::Model::getScenarioInfo(const CalCoreModel* calCoreModel, const GLfloat deltaTime) {
	// Looking for requested core model
	if (m_calCoreModelMap.find(calCoreModel->getName()) == m_calCoreModelMap.end()) {
		// Core model not found
		return nullptr;
	}

	// Reference to scenario event list
	std::list<std::shared_ptr<ScenarioInfo>>& rScInfoLst = m_calCoreModelMap[calCoreModel->getName()].m_scenarioInfoLst;

	// Time tracking
	if (rScInfoLst.empty())
		m_scTime = 0.0f;
	else
		m_scTime += deltaTime;

	// Scenario events
	std::list<std::shared_ptr<ScenarioInfo>>::const_iterator itScenario = rScInfoLst.cbegin();
	while (itScenario != rScInfoLst.cend()) {
		const ScenarioInfo* pScenInfo = itScenario->get();
		// find next event
		if (pScenInfo->m_delay < m_scTime)
			// Use scenario event
			return pScenInfo;
		else
			// iterator
			++itScenario;
	}

	// No event
	return nullptr;
}

// Returns a const pointer to the mesh data struct
const walker::MeshData* walker::Model::getMeshData(const CalCoreModel* calCoreModel, const std::string& meshName) {
	// Looking for requested core model
	if (m_calCoreModelMap.find(calCoreModel->getName()) == m_calCoreModelMap.end()) {
		// Core model not found
		return nullptr;
	}

	// Core model data
	CalCoreModelData& rCoreModelData = m_calCoreModelMap[calCoreModel->getName()];

	// Looking for requested mesh
	if (rCoreModelData.m_meshMap.find(meshName) == rCoreModelData.m_meshMap.end()) {
		// Mesh not found
		return nullptr;
	}

	// return selected mesh data
	return rCoreModelData.m_meshMap[meshName].get();
}

// Returns a const pointer to the material data struct
const walker::MaterialData* walker::Model::getMaterialData(const CalCoreModel* calCoreModel, const std::string& materialName) {
	// Looking for requested core model
	if (m_calCoreModelMap.find(calCoreModel->getName()) == m_calCoreModelMap.end()) {
		// Core model not found
		return nullptr;
	}

	// Core model data
	CalCoreModelData& rCoreModelData = m_calCoreModelMap[calCoreModel->getName()];

	// Looking for requested material
	if (rCoreModelData.m_materialMap.find(materialName) == rCoreModelData.m_materialMap.end()) {
		// Material not found
		return nullptr;
	}

	// return selected material data
	return rCoreModelData.m_materialMap[materialName].get();
}

// Returns a const pointer to the animation data struct
const walker::AnimationData* walker::Model::getAnimationData(const CalCoreModel* calCoreModel, const std::string& animationName) {
	// Looking for requested core model
	if (m_calCoreModelMap.find(calCoreModel->getName()) == m_calCoreModelMap.end()) {
		// Core model not found
		return nullptr;
	}

	// Core model data
	CalCoreModelData& rCoreModelData = m_calCoreModelMap[calCoreModel->getName()];

	// Looking for requested animation
	if (rCoreModelData.m_animationMap.find(animationName) == rCoreModelData.m_animationMap.end()) {
		// Animation not found
		return nullptr;
	}

	// return selected animation data
	return rCoreModelData.m_animationMap[animationName].get();
}

// Returns a const pointer to the compound data struct
const walker::CompoundData* walker::Model::getCompoundData(const CalCoreModel* calCoreModel, const std::string& compoundName) {
	// Looking for requested core model
	if (m_calCoreModelMap.find(calCoreModel->getName()) == m_calCoreModelMap.end()) {
		// Core model not found
		return nullptr;
	}

	// Core model data
	CalCoreModelData& rCoreModelData = m_calCoreModelMap[calCoreModel->getName()];

	// Looking for requested compound
	if (rCoreModelData.m_compoundMap.find(compoundName) == rCoreModelData.m_compoundMap.end()) {
		// Compound not found
		return nullptr;
	}

	// return selected compound data
	return rCoreModelData.m_compoundMap[compoundName].get();
}

// Returns action removal delay
GLfloat* walker::Model::getRepeatAction(const CalCoreModel* calCoreModel, const std::string& animationName) {
	// Looking for requested core model
	if (m_calCoreModelMap.find(calCoreModel->getName()) == m_calCoreModelMap.end()) {
		// Core model not found
		return nullptr;
	}

	// Core model data
	CalCoreModelData& rCoreModelData = m_calCoreModelMap[calCoreModel->getName()];

	// Looking for requested action repeat data
	if (rCoreModelData.m_repActMap.find(animationName)
		== rCoreModelData.m_repActMap.end()) {
		// Action not found
		return nullptr;
	}

	// Action delay
	return &rCoreModelData.m_repActMap[animationName];
}

// Local CalModel orientation
GLfloat walker::Model::getDirectionAngle(const CalModel* pModel, const glm::vec3 direction) const {
	// Get direction
	const CalCoreModel* pCoreModel = pModel->getCoreModel();
	const walker::GeometryInfo* pGeometryData = walker::Model::instance()->getGeometryInfo(pCoreModel);
	if (pGeometryData) {
		// Get direction
		glm::ivec2 dir = pGeometryData->m_direction;

		if (dir.x == 0 && dir.y == -1)
			return atan2(direction.x, -direction.y);
		else if (dir.x == -1 && dir.y == 0)
			return atan2(-direction.y, -direction.x);
		else if (dir.x == 0 && dir.y == 1)
			return atan2(-direction.x, direction.y);
		else if (dir.x == 1 && dir.y == 0)
			return atan2(direction.y, direction.x);
	}

	// Default direction (x=0, y=-1)
	return atan2(direction.x, -direction.y);
}

// Remove scenario event from the list
GLboolean walker::Model::removeScenarioInfo(const CalCoreModel* calCoreModel, const ScenarioInfo* info) {
	// Looking for requested core model
	if (m_calCoreModelMap.find(calCoreModel->getName()) == m_calCoreModelMap.end()) {
		// Core model not found
		return false;
	}

	// Reference to scenario event list
	std::list<std::shared_ptr<ScenarioInfo>>& rScInfoLst = m_calCoreModelMap[calCoreModel->getName()].m_scenarioInfoLst;

	// Scenario events
	std::list<std::shared_ptr<ScenarioInfo>>::const_iterator itScenario = rScInfoLst.cbegin();
	while (itScenario != rScInfoLst.cend()) {
		const ScenarioInfo* pScenInfo = itScenario->get();
		// Find next event
		if (pScenInfo->m_animation == info->m_animation) {
			// Remove scenario event
			rScInfoLst.erase(itScenario);
			return true;
		}

		// Iterator
		++itScenario;
	}

	// No event
	return false;
}

// Update on model deactivation status
GLboolean walker::Model::hasModelExpired(CalModel* pModel, CalModel* pAddModel) {
	// Confirm additional model
	const CalCoreModel* pCoreModel = pModel->getCoreModel();
	const CalCoreModel* pAddCoreModel = pAddModel->getCoreModel();
	const std::string& modelName = pCoreModel->getName();
	const std::string& addModelName = pAddCoreModel->getName();
	if (modelName == addModelName)
		// base model
		return false;

	// CalModel mixer
	CalMixer* pCalMix = pModel->getMixer();

	// Cycle animation list
	const std::list<CalAnimationCycle*>& cycleList = pCalMix->getAnimationCycle();
	std::list<CalAnimationCycle*>::const_iterator itCalCycle = cycleList.cbegin();
	while (itCalCycle != cycleList.cend()) {
		const CalAnimationCycle* pCalCycle = *itCalCycle;
		const CalCoreAnimation* pCoreAnimation = pCalCycle->getCoreAnimation();
		// Pointer to the current animation additional data
		const walker::AnimationData* pAniData = getAnimationData(pCoreModel, pCoreAnimation->getName());
		if (pAniData) {
			const walker::CompoundData* pComData = getCompoundData(pCoreModel, pAniData->m_compound);
			if (pComData && pComData->m_skeleton == addModelName)
				// Model needed
				return false;
		}

		// Iterator
		++itCalCycle;
	}

	// Action animation list
	const std::list<CalAnimationAction*>& actionList = pCalMix->getAnimationActionList();
	std::list<CalAnimationAction*>::const_iterator itCalAction = actionList.cbegin();
	while (itCalAction != actionList.cend()) {
		const CalAnimationAction* pCalAction = *itCalAction;
		const CalCoreAnimation* pCoreAnimation = pCalAction->getCoreAnimation();
		// Pointer to the current animation additional data
		const walker::AnimationData* pAniData = getAnimationData(pCoreModel, pCoreAnimation->getName());
		if (pAniData) {
			const walker::CompoundData* pComData = getCompoundData(pCoreModel, pAniData->m_compound);
			if (pComData && pComData->m_skeleton == addModelName)
				// Model needed
				return false;
		}

		// Iterator
		++itCalAction;
	}

	// Remove model
	return true;
}

//Update on model activation status
GLboolean walker::Model::hasModelStarted(CalModel* pModel, CalModel* pAddModel, const ScenarioInfo* info) {
	// Additional model meshes
	if (!attachMesh(pAddModel))
		return false;

	const CalCoreModel* pAddCoreModel = pAddModel->getCoreModel();
	const walker::AnimationData* pAddAniData = getAnimationData(pAddCoreModel, info->m_animation);
	if (pAddAniData) {
		// Animation assignment
		if (info->m_duration == 0.0f) {
			// Remove cycle
			if (!clearCycle(pAddModel))
				return false;

			// Assign cycle
			if (!assignCycle(pAddModel, info))
				return false;

			// Sync additional animation
			CalMixer* pCalMix = pModel->getMixer();
			CalMixer* pAddCalMix = pAddModel->getMixer();
			std::list<CalAnimationCycle*>& addCalCycleLst = pAddCalMix->getAnimationCycle();
			std::list<CalAnimationCycle*>::iterator itAddCalCycleLst = addCalCycleLst.begin();
			while (itAddCalCycleLst != addCalCycleLst.end()) {
				CalAnimationCycle* pAddCalCycle = *itAddCalCycleLst;
				const CalCoreAnimation* pAddCoreAnimation = pAddCalCycle->getCoreAnimation();
				if (info->m_animation == pAddCoreAnimation->getName()) {
					pAddCalCycle->setAsync(pAddCalCycle->getTime(), pAddCoreAnimation->getDuration());
					pAddCalCycle->setTime(pCalMix->getAnimationTime());
				}

				// iterator
				++itAddCalCycleLst;
			}
		}
		else {
			// Assign action
			if (!assignAction(pAddModel, info))
				return false;
		}
	}

	// Model set
	return true;
}

// Remove additional meshes (that are no longer needed)
GLboolean walker::Model::detachMesh(CalModel* pModel, GLboolean isAppend) {
	// CalModel database
	CalMixer* pCalMix = pModel->getMixer();
	const CalCoreModel* pCoreModel = pModel->getCoreModel();
	const std::vector<CalMesh*>& meshVec = pModel->getVectorMesh();
	for (size_t i = 0; i < meshVec.size(); ++i) {
		// Attached mesh data
		const CalMesh* pMesh = meshVec[i];
		const CalCoreMesh* pCoreMesh = pMesh->getCoreMesh();
		const std::string& meshName = pCoreMesh->getName();
		const walker::MeshData* pMeshData = walker::Model::instance()->getMeshData(pCoreModel, meshName);

		// Skip appended meshes
		if (isAppend && pMeshData && pMeshData->m_append)
			continue;

		// Control boolean
		GLboolean removeMesh = true;

		// Cycle animation list
		const std::list<CalAnimationCycle*>& calCycLst = pCalMix->getAnimationCycle();
		std::list<CalAnimationCycle*>::const_iterator itCalCycLst = calCycLst.cbegin();
		while (itCalCycLst != calCycLst.cend()) {
			// Clear a single cycle
			const CalAnimationCycle* pCalCycle = *itCalCycLst;
			if (pCalCycle) {
				const CalCoreAnimation* pCoreAnimation = pCalCycle->getCoreAnimation();
				const std::string& cycName = pCoreAnimation->getName();
				// Pointer to the current animation additional data
				const walker::AnimationData* pAnimData = walker::Model::instance()
					->getAnimationData(pCoreModel, pCoreAnimation->getName());

				// Animation still in the loop
				if (pAnimData && meshName == pAnimData->m_mesh) {
					// mesh still needed
					removeMesh = false;
					break;
				}
			}

			// Iterator
			++itCalCycLst;
		}

		// Further check
		if (removeMesh) {
			// Action animation list
			const std::list<CalAnimationAction*>& actionList = pCalMix->getAnimationActionList();
			std::list<CalAnimationAction*>::const_iterator itCalAction = actionList.cbegin();
			while (itCalAction != actionList.cend()) {
				const CalAnimationAction* pCalAction = *itCalAction;
				const CalCoreAnimation* pCoreAnimation = pCalAction->getCoreAnimation();
				// Pointer to the current animation additional data
				const walker::AnimationData* pAnimData = walker::Model::instance()
					->getAnimationData(pCoreModel, pCoreAnimation->getName());

				// Animation still in the loop
				if (pAnimData && meshName == pAnimData->m_mesh) {
					// mesh still needed
					removeMesh = false;
					break;
				}

				// Iterator
				++itCalAction;
			}
		}

		// Remove mesh for finished gesture
		if (removeMesh) {
			if (!pModel->detachMesh(pCoreModel->getCoreMeshId(meshName))) {
				CalError::printLastError();
				return false;
			}
		}
	}

	// Detached
	return true;
}

// Add appended meshes
GLboolean walker::Model::attachMesh(CalModel* pModel) {
	// Attach appended meshes
	const CalCoreModel* pCoreModel = pModel->getCoreModel();
	for (GLint i = 0; i < pCoreModel->getCoreMeshCount(); ++i) {
		const CalCoreMesh* coreMesh = pCoreModel->getCoreMesh(i);
		const std::string& meshName = coreMesh->getName();
		const walker::MeshData* pMeshData = walker::Model::instance()->getMeshData(pCoreModel, meshName);
		if (pMeshData && pMeshData->m_append) {
			GLint coreMeshID = pCoreModel->getCoreMeshId(meshName);
			if (!pModel->attachMesh(coreMeshID)) {
				CalError::printLastError();
				return false;
			}
		}
	}

	// Attached
	return true;
}

// Remove cycles
GLboolean walker::Model::clearCycle(CalModel* pModel, const std::string& animationName) {
	// Clear all existing cycles
	CalMixer* pCalMix = pModel->getMixer();
	const CalCoreModel* pCoreModel = pModel->getCoreModel();
	const std::list<CalAnimationCycle*>& calCycLst = pCalMix->getAnimationCycle();
	std::list<CalAnimationCycle*>::const_iterator itCalCycLst = calCycLst.cbegin();
	while (itCalCycLst != calCycLst.cend()) {
		// Clear a single cycle
		const CalAnimationCycle* pCalCycle = *itCalCycLst;
		if (pCalCycle) {
			const CalCoreAnimation* pCoreAnimation = pCalCycle->getCoreAnimation();
			const std::string& cycName = pCoreAnimation->getName();
			if (animationName.empty() || cycName == animationName) {
				const GLint aniID = pCoreModel->getCoreAnimationId(cycName);
				const walker::AnimationData* pAniData = walker::Model::instance()->getAnimationData(pCoreModel, cycName);
				if (!pAniData)
					return false;

				if (!pCalMix->clearCycle(aniID, pAniData->m_blend)) {
					CalError::printLastError();
					return false;
				}
			}
		}

		// Iterator
		++itCalCycLst;
	}

	// Cycle out
	return true;
}

// Add cycle
GLboolean walker::Model::assignCycle(CalModel* pModel, const ScenarioInfo* info) {
	// Assign cycle to the animation list
	GLboolean isAssigned = false;
	const CalCoreModel* pCoreModel = pModel->getCoreModel();
	const walker::AnimationData* pAniData = walker::Model::instance()->getAnimationData(pCoreModel, info->m_animation);
	if (pAniData) {
		// Locals
		CalMixer* pCalMix = pModel->getMixer();
		const GLint aniID = pCoreModel->getCoreAnimationId(info->m_animation);

		// Assign cycle to list
		isAssigned = pCalMix->blendCycle(aniID, info->m_weight, info->m_blendIn);

		if (isAssigned) {
			// Assign modified speed
			if (info->m_scale != 1.0f) {
				std::list<CalAnimationCycle*>& calCycleLst = pCalMix->getAnimationCycle();
				std::list<CalAnimationCycle*>::iterator itCalCycleLst = calCycleLst.begin();
				while (itCalCycleLst != calCycleLst.end()) {
					CalAnimationCycle* pCalCycle = *itCalCycleLst;
					if (pCalCycle) {
						const CalCoreAnimation* pCoreAnimation = pCalCycle->getCoreAnimation();
						if (info->m_animation == pCoreAnimation->getName()) {
							pCalCycle->setAsync(pCalCycle->getTime(), pCoreAnimation->getDuration());
							pCalCycle->setTimeFactor(info->m_scale);
						}
					}

					// Iterator
					++itCalCycleLst;
				}
			}
		}
		else // Check for error
			CalError::printLastError();

		// Add additional mesh
		GLint coreMeshID = pCoreModel->getCoreMeshId(pAniData->m_mesh);
		if (coreMeshID != -1)
			if (!pModel->attachMesh(coreMeshID))
				CalError::printLastError();
	}

	// Cycle in
	return isAssigned;
}

// Remove actions
GLboolean walker::Model::clearAction(CalModel* pModel, const std::string& animationName) {
	// Clear all existing actions
	CalMixer* pCalMix = pModel->getMixer();
	const CalCoreModel* pCoreModel = pModel->getCoreModel();
	const std::list<CalAnimationAction*>& calActLst = pCalMix->getAnimationActionList();
	std::list<CalAnimationAction*>::const_iterator itCalActLst = calActLst.cbegin();
	while (itCalActLst != calActLst.cend()) {
		// Clear a single action
		const CalAnimationAction* pCalAction = *itCalActLst;
		if (pCalAction) {
			const CalCoreAnimation* pCoreAnimation = pCalAction->getCoreAnimation();
			const std::string& actName = pCoreAnimation->getName();
			if (animationName.empty() || actName == animationName) {
				// Iterator
				++itCalActLst;

				// Remove action
				const GLint aniID = pCoreModel->getCoreAnimationId(actName);
				if (!pCalMix->removeAction(aniID)) {
					CalError::printLastError();
					return false;
				}

				// Skip
				continue;
			}
		}

		// Iterator
		++itCalActLst;
	}

	// Action in
	return true;
}

// Add action
GLboolean walker::Model::assignAction(CalModel* pModel, const ScenarioInfo* info) {
	// Pointer to the animation additional data
	const std::string& aniName = info->m_animation;
	const CalCoreModel* pCoreModel = pModel->getCoreModel();
	const walker::AnimationData* pAniData = walker::Model::instance()->getAnimationData(pCoreModel, aniName);
	if (pAniData) {
		// Local
		CalMixer* pCalMix = pModel->getMixer();
		const GLint aniID = pCoreModel->getCoreAnimationId(aniName);
		const CalCoreAnimation* pCoreAnimation = pCoreModel->getCoreAnimation(aniID);

		// Assign action more than once
		if (info->m_duration > pCoreAnimation->getDuration()) {
			// Assign action intro
			if (!pCalMix->executeAction(aniID, info->m_blendIn, 0.0f)) {
				CalError::printLastError();
				return false;
			}

			// Update action repeat map
			if (setRepeatAction(pCoreModel->getName(), aniName, info->m_duration))
				return false;
		}
		else
			// Assign action once only
			if (!pCalMix->executeAction(aniID, info->m_blendIn, info->m_blendOut)) {
				CalError::printLastError();
				return false;
			}

		// Add additional mesh
		if (!pAniData->m_mesh.empty()) {
			GLint coreMeshID = pCoreModel->getCoreMeshId(pAniData->m_mesh);
			if (coreMeshID != -1)
				if (!pModel->attachMesh(coreMeshID))
					CalError::printLastError();
		}
	}

	// Action out
	return true;
}

// Add action as cycle
GLvoid walker::Model::loopAction(CalModel* pModel, const GLfloat deltaTime) {
	// Check action animation list
	CalMixer* pCalMixer = pModel->getMixer();
	const CalCoreModel* pCoreModel = pModel->getCoreModel();
	const std::list<CalAnimationAction*>& calActLst = pCalMixer->getAnimationActionList();
	std::list<CalAnimationAction*>::const_iterator itCalAction = calActLst.cbegin();
	while (itCalAction != calActLst.cend()) {
		// Locate action within the map
		CalAnimationAction* pCalAction = *itCalAction;
		if (pCalAction) {
			const CalCoreAnimation* pCoreAnimation = pCalAction->getCoreAnimation();

			// Reference to action duration
			GLfloat* pActDuration = getRepeatAction(pCoreModel, pCoreAnimation->getName());
			if (pActDuration) {
				// Update remaining time
				*pActDuration -= deltaTime;

				// Action finished cycle
				if (pCoreAnimation->getDuration() - pCalAction->getTime() < deltaTime) {
					// Loop the animation
					if (*pActDuration > 0) {
						pCalAction->setTime(pCalAction->getTime() - pCoreAnimation->getDuration());
					}
					else {
						// Pointer to animation additional data
						const std::string& aniName = pCoreAnimation->getName();
						const walker::AnimationData* pAniData = walker::Model::instance()->getAnimationData(pCoreModel, aniName);
						if (pAniData) {
							// Animation vector ID
							const GLint aniID = pCoreModel->getCoreAnimationId(aniName);
							// Perform action last time
							if (!pCalMixer->executeAction(aniID, 0.0f, pAniData->m_blend))
								CalError::printLastError();
						}

						// Remove action from the map
						removeAction(pCoreModel, pCoreAnimation->getName());
					}
				}
			}
		}

		// Iterator
		++itCalAction;
	}
}

// Remove action repeat
GLvoid walker::Model::removeAction(const CalCoreModel* calCoreModel, const std::string& animationName) {
	// Looking for requested core model
	if (m_calCoreModelMap.find(calCoreModel->getName()) == m_calCoreModelMap.end()) {
		// Core model not found
		return;
	}

	// Core model data
	CalCoreModelData& rCoreModelData = m_calCoreModelMap[calCoreModel->getName()];

	// Looking for requested action repeat data
	if (rCoreModelData.m_repActMap.find(animationName)
		== rCoreModelData.m_repActMap.end()) {
		// Action not found
		return;
	}

	// Find action repeat data
	std::map<std::string, GLfloat>::iterator itRepActMap = rCoreModelData.m_repActMap.find(animationName);

	// Remove action repeat data
	rCoreModelData.m_repActMap.erase(itRepActMap);
}

// Add adjustments to skeleton
GLboolean walker::Model::absBoneAdjustment(CalModel* pModel, const glm::vec3 dir, const glm::vec3 aim) {
	// Local boolean
	GLboolean useAbsoluteAdjust = false;

	// Model additional data
	const CalCoreModel* pCalCoreModel = pModel->getCoreModel();

	// Horizontal view projection
	glm::vec3 hAim = glm::normalize(glm::vec3(aim.x, aim.y, 0.0f));

	// Angle between two vectors (horizontal)
	glm::vec3 crossYaw = glm::cross(dir, hAim);
	GLfloat cosYaw = glm::dot(dir, hAim) / glm::length(dir) / glm::length(hAim);

	// Horizontal bone rotation (considering the sign of an angle)
	const GLfloat YAW = crossYaw.z < 0.0f ? -glm::acos(cosYaw) : glm::acos(cosYaw);

	// Vertical view projection
	glm::vec3 vAim = glm::vec3(dir.x, dir.y, aim.z);

	// Angle between two vectors (vertical)
	GLfloat cosPitch = glm::dot(dir, vAim) / glm::length(dir) / glm::length(vAim);

	// Vertical bone rotation (considering the sign of an angle)
	const GLfloat PITCH = aim.z > 0.0f ? -glm::acos(cosPitch) : glm::acos(cosPitch);

	// blend factor for all bones will be decreased with higher angle
	GLfloat globalFac = 1.0f - glm::max<GLfloat>(glm::abs(YAW), glm::abs(PITCH)) / 2.0f / glm::pi<GLfloat>();
	if (globalFac > BONE_ROT_MAX) globalFac = BONE_ROT_MAX;

	// Here we assign any bone rotation we want to add to model
	CalQuaternion& rotationAdjustment = m_boneAdjustMix->localOri_;

	// Base model mixer
	CalMixer* pCalMixer = pModel->getMixer();
	const CalSkeleton* pCalSkeleton = pModel->getSkeleton();
	const CalCoreSkeleton* pCoreSkeleton = pCalSkeleton->getCoreSkeleton();
	const std::vector<CalCoreBone*>& coreBones = pCoreSkeleton->getVectorCoreBone();

	// Iterate through all skeleton bones
	for (size_t i = 0; i < pCoreSkeleton->getNumCoreBones(); ++i) {
		// Local default rotation
		rotationAdjustment = CalQuaternion();

		// Local bone
		const CalCoreBone* pCoreBone = coreBones.at(i);
		const std::string& boneName = pCoreBone->getName();

		// In this section we apply different type
		// of adjustment, based on the bone name
		switch (getAdjustmentGroup(boneName)) {

			// headtracking
		case BoneGroup::SPINE: {
			// Local factor
			GLfloat localFac = 0.0f;

			// Type of current bone adjustment
			switch (getAdjustmentType(boneName)) {
			case BoneType::SPINE1:
				// Lower spine rotation factor
				localFac = m_boneAdjustMap.at(BoneType::SPINE1);
				break;
			case BoneType::SPINE2:
				// Upper spine rotation factor
				localFac = m_boneAdjustMap.at(BoneType::SPINE2);
				break;
			case BoneType::NECK:
				// Neck rotation factor
				localFac = m_boneAdjustMap.at(BoneType::NECK);
				break;
			case BoneType::HEAD:
				// Head rotation factor
				localFac = m_boneAdjustMap.at(BoneType::HEAD);
				break;
			case BoneType::NONE:
			case BoneType::LEFT_UPPER_ARM:
			case BoneType::LEFT_FORE_ARM:
			case BoneType::RIGHT_UPPER_ARM:
			case BoneType::RIGHT_FORE_ARM:
				break;
			}

			// Individual bone total rotation factor
			const GLfloat ROT_FAC = localFac / (1.0f - globalFac);
			if (ROT_FAC != 0.0f) {
				// Rotation matrices for rotation around XYZ axis
				const glm::mat4 MAT_X = glm::rotate(YAW * ROT_FAC, glm::vec3(-1.0f, 0.0f, 0.0f));
				const glm::mat4 MAT_Y = glm::rotate(PITCH * ROT_FAC * glm::sin(YAW), glm::vec3(0.0f, -1.0f, 0.0f));
				const glm::mat4 MAT_Z = glm::rotate(PITCH * ROT_FAC * glm::cos(YAW), glm::vec3(0.0f, 0.0f, 1.0f));

				// Rotation quaternion
				const glm::highp_quat QUAT = glm::quat_cast(MAT_X * MAT_Y * MAT_Z);
				rotationAdjustment = CalQuaternion(QUAT.x, QUAT.y, QUAT.z, QUAT.w);

				// Blend rotation with core model state
				rotationAdjustment.blend(globalFac, pCoreBone->getRotation());
			}

			break;
		}

		case BoneGroup::NONE:
		case BoneGroup::WHEEL:
		case BoneGroup::STEER:
		case BoneGroup::ARM:
			break;
		}

		// Check if there are rotations in waiting
		if (rotationAdjustment != CalQuaternion())
			// Apply rotation adjustment
			useAbsoluteAdjust = pCalMixer->addBoneAdjustment(i, *m_boneAdjustMix);
	}

	// Absolute bone adjustment
	return useAbsoluteAdjust;
}

// Relative skeleton adjustments
GLboolean walker::Model::relBoneAdjustment(CalModel* pModel, GLfloat speed, GLfloat steer, const GLfloat deltaTime) {
	// Local boolean
	GLboolean useRelativeAdjust = false;

	// Here we assign any bone rotation we want to add to model
	CalQuaternion& rotationAdjustment = m_boneAdjustMix->localOri_;

	// Local model bone data
	CalMixer* pCalMixer = pModel->getMixer();
	const CalCoreModel* pCalCoreModel = pModel->getCoreModel();
	const CalCoreSkeleton* pCoreSkeleton = pCalCoreModel->getCoreSkeleton();
	const std::vector<CalCoreBone*>& coreBones = pCoreSkeleton->getVectorCoreBone();

	// Iterate through all skeleton bones
	for (size_t i = 0; i < pCoreSkeleton->getNumCoreBones(); ++i) {
		// Local default rotation
		rotationAdjustment = CalQuaternion();

		// Local bone
		const CalCoreBone* pCoreBone = coreBones.at(i);
		const std::string& boneName = pCoreBone->getName();

		// In this section we apply different type
		// of adjustment, based on the bone name
		switch (getAdjustmentGroup(boneName)) {
		// wheel rotation
		case BoneGroup::WHEEL: {
			// incremental increase of wheel rotation
			static GLfloat angle = 0.0f;

			// incremental increase of wheel rotation => v / r * dt
			angle += speed / 0.336f * deltaTime;
			if (angle > 2.0f * glm::pi<GLfloat>())
				angle -= 2.0f * glm::pi<GLfloat>();

			// Rotation matrix and quaternion for rotation around X axis
			const glm::mat4 MAT = glm::rotate(angle, glm::vec3(1.0f, 0.0f, 0.0f));
			const glm::highp_quat QUAT = glm::quat_cast(MAT);

			// Applying rotation to skeleton
			rotationAdjustment = pCoreBone->getRotation() * CalQuaternion(QUAT.x, QUAT.y, QUAT.z, QUAT.w);
			break;
		}

		// steering angle
		case BoneGroup::STEER: {
			// Rotation matrix and quaternion for rotation around X axis
			const glm::mat4 MAT = glm::rotate(steer, glm::vec3(1.0f, 0.0f, 0.0f));
			const glm::highp_quat QUAT = glm::quat_cast(MAT);

			// Applying rotation to skeleton
			rotationAdjustment = pCoreBone->getRotation() * CalQuaternion(QUAT.x, QUAT.y, QUAT.z, QUAT.w);
			break;
		}

		// head turning to steer direction
		case BoneGroup::SPINE: {
			// Local bone data
			const GLint BONE_ID = pCoreSkeleton->getCoreBoneId(boneName);
			const CalSkeleton* pSkeleton = pModel->getSkeleton();
			const CalBone* pBone = pSkeleton->getBone(BONE_ID);
			if (pBone) {
				// Local matrix
				glm::mat4 rotMat;

				// Type of current bone adjustment
				switch (getAdjustmentType(boneName)) {
				case BoneType::HEAD:
					// Rotation matrix and quaternion for rotation around X axis
					rotMat = glm::rotate(steer, glm::vec3(-1.0f, 0.0f, 0.0f));
					break;
				case BoneType::NONE:
				case BoneType::SPINE1:
				case BoneType::SPINE2:
				case BoneType::NECK:
				case BoneType::LEFT_UPPER_ARM:
				case BoneType::LEFT_FORE_ARM:
				case BoneType::RIGHT_UPPER_ARM:
				case BoneType::RIGHT_FORE_ARM:
					break;
				}

				// Skeleton update required
				pCalMixer->updateSkeleton();

				// Applying rotation to skeleton
				const glm::highp_quat QUAT = glm::quat_cast(rotMat);
				rotationAdjustment = pBone->getRotation() * CalQuaternion(QUAT.x, QUAT.y, QUAT.z, QUAT.w);
			}
			break;
		}

		// arms on steer
		case BoneGroup::ARM: {
			// Local bone data
			const GLint BONE_ID = pCoreSkeleton->getCoreBoneId(boneName);
			const CalSkeleton* pSkeleton = pModel->getSkeleton();
			const CalBone* pBone = pSkeleton->getBone(BONE_ID);
			if (pBone) {
				// Local matrix
				glm::mat4 rotMat;

				// Type of current bone adjustment
				switch (getAdjustmentType(boneName)) {
				case BoneType::LEFT_UPPER_ARM:
					// Rotation matrix for rotation around Z axis
					rotMat = glm::rotate(m_boneAdjustMap.at(BoneType::LEFT_UPPER_ARM) * steer, glm::vec3(0.0f, 0.0f, -1.0f));
					break;
				case BoneType::LEFT_FORE_ARM:
					// Rotation matrix for rotation around Z axis
					rotMat = glm::rotate(m_boneAdjustMap.at(BoneType::LEFT_FORE_ARM) * steer, glm::vec3(0.0f, 0.0f, -1.0f));
					break;
				case BoneType::RIGHT_UPPER_ARM:
					// Rotation matrix for rotation around Z axis
					rotMat = glm::rotate(m_boneAdjustMap.at(BoneType::RIGHT_UPPER_ARM) * steer, glm::vec3(0.0f, 0.0f, 1.0f));
					break;
				case BoneType::RIGHT_FORE_ARM:
					// Rotation matrix for rotation around Z axis
					rotMat = glm::rotate(m_boneAdjustMap.at(BoneType::RIGHT_FORE_ARM) * steer, glm::vec3(0.0f, 0.0f, 1.0f));
					break;
				case BoneType::NONE:
				case BoneType::SPINE1:
				case BoneType::SPINE2:
				case BoneType::NECK:
				case BoneType::HEAD:
					break;
				}

				// Skeleton update required
				pCalMixer->updateSkeleton();

				// Applying rotation to skeleton
				const glm::highp_quat ROT_QUAT = glm::quat_cast(rotMat);
				rotationAdjustment = pBone->getRotation() * CalQuaternion(ROT_QUAT.x, ROT_QUAT.y, ROT_QUAT.z, ROT_QUAT.w);
			}
			break;
		}

		// no rotation
		case BoneGroup::NONE:
			break;
		}

		// Check if there are rotations in waiting
		if (rotationAdjustment != CalQuaternion())
			// Apply calculated bone adjustment
			useRelativeAdjust = pCalMixer->addBoneAdjustment(i, *m_boneAdjustMix);
	}

	// Relative bone adjustment
	return useRelativeAdjust;
}

// Render model meshes
GLvoid walker::Model::drawMesh(const GLuint programID, CalModel* pModel) {
	// Rendering init
	CalRenderer* pCalRenderer = pModel->getRenderer();

	// Begin the rendering loop
	if (!pCalRenderer->beginRendering()) {
		std::cout << CalError::getLastErrorDescription() << std::endl;
		return;
	}

	// Bind vertex array
	glBindVertexArray(getVertexArrayID());

	// Get the number of meshes
	GLuint meshCount = pCalRenderer->getMeshCount();

	// Loop through all meshes of the model
	for (GLuint rMeshID = 0; rMeshID < meshCount; ++rMeshID) {
		// Get the number of submeshes
		GLuint subMeshCount = pCalRenderer->getSubmeshCount(rMeshID);
		// Loop through all submeshes of the mesh
		for (GLuint rSubMeshID = 0; rSubMeshID < subMeshCount; ++rSubMeshID) {
			// Select mesh and submesh for further data access
			if (pCalRenderer->selectMeshSubmesh(rMeshID, rSubMeshID)) {
				// Clear data buffers
				clearBuffers();

				// Get the transformed vertices of the submesh
				pCalRenderer->getVertices(getVert());
				// Get the transformed normals of the submesh
				pCalRenderer->getNormals(getNorm());
				// Get the texture coordinates of the submesh
				pCalRenderer->getTextureCoordinates(0, getCoord());
				// Get the faces of the submesh
				GLint faceCount = pCalRenderer->getFaces(getFace());

				// Bind data buffers
				glBindBuffer(GL_ARRAY_BUFFER, getVbo(0));
				glBufferSubData(GL_ARRAY_BUFFER, 0, MOD_NUM_VERTICES * MOD_NUM_FLOAT_VRT * sizeof(GLfloat), getVert());

				glBindBuffer(GL_ARRAY_BUFFER, getVbo(1));
				glBufferSubData(GL_ARRAY_BUFFER, 0, MOD_NUM_VERTICES * MOD_NUM_FLOAT_VRT * sizeof(GLfloat), getNorm());

				glBindBuffer(GL_ARRAY_BUFFER, getVbo(2));
				glBufferSubData(GL_ARRAY_BUFFER, 0, MOD_NUM_VERTICES * MOD_NUM_FLOAT_CRD * sizeof(GLfloat), getCoord());

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, getVbo(3));
				glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, MOD_NUM_FACES * MOD_NUM_FLOAT_VRT * sizeof(CalIndex), getFace());

				// Textures
				if (m_useMaterial) {
					// apply with material
					glActiveTexture(GL_TEXTURE0);
					glUniform1i(glGetUniformLocation(programID, "textureSampler"), 0);
					intptr_t textureID = (intptr_t)pCalRenderer->getMapUserData(0);
					glBindTexture(GL_TEXTURE_2D, (GLuint)textureID);
				}
				else {
					// Apply without material
					const std::vector<CalMesh*>& vecCalMesh = pModel->getVectorMesh();
					if (rMeshID < vecCalMesh.size()) {
						// CalMesh
						const CalMesh* calMesh = vecCalMesh[rMeshID];
						const CalCoreMesh* calCoreMesh = calMesh->getCoreMesh();
						const std::string& meshName = calCoreMesh->getName();

						// MeshData
						const walker::MeshData* pMeshData = getMeshData(pModel->getCoreModel(), meshName);
						if (pMeshData) {
							const GLuint texID = pMeshData->m_texID;
							glActiveTexture(GL_TEXTURE0 + texID);
							glUniform1i(glGetUniformLocation(programID, "textureSampler"), texID);
							glBindTexture(GL_TEXTURE_2D, texID);
						}
					}
				}

				if (sizeof(CalIndex) == 2)
					glDrawElements(GL_TRIANGLES, faceCount * MOD_NUM_FLOAT_VRT, GL_UNSIGNED_SHORT, 0);
				else
					glDrawElements(GL_TRIANGLES, faceCount * MOD_NUM_FLOAT_VRT, GL_UNSIGNED_INT, 0);

				// End of biding
				glBindTexture(GL_TEXTURE_2D, 0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}
		}
	}

	// End the rendering of the model
	pCalRenderer->endRendering();
}

// Render model skeleton
GLvoid walker::Model::drawSkeleton(const CalSkeleton* pSkeleton) {
	// Draw the bone lines
	GLfloat lines[1024][2][3];
	GLint nrLines = pSkeleton->getBoneLines(&lines[0][0][0]);

	glLineWidth(5.0f);
	glColor3f(1.0f, 0.5f, 0.0f);
	glBegin(GL_LINES);

	for (GLint currLine = 0; currLine < nrLines; ++currLine) {
		glVertex3f(lines[currLine][0][0], lines[currLine][0][1], lines[currLine][0][2]);
		glVertex3f(lines[currLine][1][0], lines[currLine][1][1], lines[currLine][1][2]);
	}

	glEnd();
	glLineWidth(1.0f);

	// Draw the bone points
	GLfloat points[1024][3];
	GLint nrPoints;
	nrPoints = pSkeleton->getBonePoints(&points[0][0]);

	glPointSize(8.0f);
	glBegin(GL_POINTS);
	glColor3f(0.0f, 0.0f, 1.0f);

	for (GLint currPoint = 0; currPoint < nrPoints; ++currPoint)
		glVertex3f(points[currPoint][0], points[currPoint][1], points[currPoint][2]);

	glEnd();
	glPointSize(1.0f);
}

// Render model bounding box
GLvoid walker::Model::drawBoundingBox(CalSkeleton* pSkeleton) {
	// Calculate bounding box
	pSkeleton->calculateBoundingBoxes();

	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_LINES);

	const std::vector<CalBone*>& vectorCalBone = pSkeleton->getVectorBone();
	for (size_t boneId = 0; boneId < vectorCalBone.size(); ++boneId) {
		CalBone* pCalBone = vectorCalBone[boneId];
		CalBoundingBox& calBoundingBox = pCalBone->getBoundingBox();

		CalVector p[8];
		calBoundingBox.computePoints(p);

		glVertex3f(p[0].x, p[0].y, p[0].z);
		glVertex3f(p[1].x, p[1].y, p[1].z);

		glVertex3f(p[0].x, p[0].y, p[0].z);
		glVertex3f(p[2].x, p[2].y, p[2].z);

		glVertex3f(p[1].x, p[1].y, p[1].z);
		glVertex3f(p[3].x, p[3].y, p[3].z);

		glVertex3f(p[2].x, p[2].y, p[2].z);
		glVertex3f(p[3].x, p[3].y, p[3].z);

		glVertex3f(p[4].x, p[4].y, p[4].z);
		glVertex3f(p[5].x, p[5].y, p[5].z);

		glVertex3f(p[4].x, p[4].y, p[4].z);
		glVertex3f(p[6].x, p[6].y, p[6].z);

		glVertex3f(p[5].x, p[5].y, p[5].z);
		glVertex3f(p[7].x, p[7].y, p[7].z);

		glVertex3f(p[6].x, p[6].y, p[6].z);
		glVertex3f(p[7].x, p[7].y, p[7].z);

		glVertex3f(p[0].x, p[0].y, p[0].z);
		glVertex3f(p[4].x, p[4].y, p[4].z);

		glVertex3f(p[1].x, p[1].y, p[1].z);
		glVertex3f(p[5].x, p[5].y, p[5].z);

		glVertex3f(p[2].x, p[2].y, p[2].z);
		glVertex3f(p[6].x, p[6].y, p[6].z);

		glVertex3f(p[3].x, p[3].y, p[3].z);
		glVertex3f(p[7].x, p[7].y, p[7].z);
	}

	glEnd();
}

// Returns vao buffer
const GLuint walker::Model::getVertexArrayID() const {
	return m_vao;
}

// Constructor
walker::Model::Model() : m_useMaterial(false), m_scTime(0.0f), BONE_ROT_MAX(0.775f) {
	// Bone adjustment modified state
	m_boneAdjustMix = std::make_shared<CalMixerBoneAdjustment>();
	m_boneAdjustMix->flags_ = 1;
	m_boneAdjustMix->rampValue_ = 1.0f;
	m_boneAdjustMix->localPos_ = CalVector(0.0f, 0.0f, 0.0f);
	m_boneAdjustMix->meshScaleAbsolute_ = CalVector(1.0f, 1.0f, 1.0f);

	// Rotation factors needed for tracking
	m_boneAdjustMap[BoneType::SPINE1] = 0.1f;
	m_boneAdjustMap[BoneType::SPINE2] = 0.2f;
	m_boneAdjustMap[BoneType::NECK] = 0.3f;
	m_boneAdjustMap[BoneType::HEAD] = 0.4f;

	// Rotation factors needed for steering
	m_boneAdjustMap[BoneType::LEFT_UPPER_ARM] = 0.3f;
	m_boneAdjustMap[BoneType::LEFT_FORE_ARM] = 0.1f;
	m_boneAdjustMap[BoneType::RIGHT_UPPER_ARM] = 0.3f;
	m_boneAdjustMap[BoneType::RIGHT_FORE_ARM] = 0.1f;

	// Create Vertex Array Object
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	// Create a Vertex Buffer Array
	glGenBuffers(MOD_NUM_BUFFERS, m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_vert), nullptr, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, MOD_NUM_FLOAT_VRT, GL_FLOAT, GL_FALSE, 0, 0);

	// Create a Normal Buffer Array
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_norm), nullptr, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, MOD_NUM_FLOAT_VRT, GL_FLOAT, GL_FALSE, 0, 0);

	// Create an TexCoord Buffer Array
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_txco), nullptr, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, MOD_NUM_FLOAT_CRD, GL_FLOAT, GL_FALSE, 0, 0);

	// Create an Element Buffer Array
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_face), nullptr, GL_DYNAMIC_DRAW);

	// End of biding
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

// Returns vao buffer
GLuint walker::Model::getVbo(const GLuint index) const {
	if (index >= MOD_NUM_BUFFERS)
		return 0;

	return m_vbo[index];
}

// Returns verticies buffer
GLfloat* walker::Model::getVert() const {
	return &walker::Model::instance()->m_vert[0][0];
}

// Returns normal buffer
GLfloat* walker::Model::getNorm() const {
	return &walker::Model::instance()->m_norm[0][0];
}

// Returns coords buffer
GLfloat* walker::Model::getCoord() const {
	return &walker::Model::instance()->m_txco[0][0];
}

// Returns faces buffer
CalIndex* walker::Model::getFace() const {
	return &walker::Model::instance()->m_face[0][0];
}

// OpenGL buffers
GLvoid walker::Model::clearBuffers() {
	// Clear OpenGL buffers
	memset((GLchar*)&m_vert[0][0], 0, MOD_NUM_VERTICES * MOD_NUM_FLOAT_VRT * sizeof(GLfloat));
	memset((GLchar*)&m_norm[0][0], 0, MOD_NUM_VERTICES * MOD_NUM_FLOAT_VRT * sizeof(GLfloat));
	memset((GLchar*)&m_txco[0][0], 0, MOD_NUM_VERTICES * MOD_NUM_FLOAT_CRD * sizeof(GLfloat));
	memset((GLchar*)&m_face[0][0], 0, MOD_NUM_FACES * MOD_NUM_FLOAT_VRT * sizeof(CalIndex));
}

// Specifies what group of current bones is given
walker::BoneGroup walker::Model::getAdjustmentGroup(const std::string& boneName) {
	// Name for the bone within the bone type list
	if (boneName.find("Wheel") != std::string::npos) {
		// Wheels rotation
		return BoneGroup::WHEEL;
	}
	else if (boneName.find("Steering") != std::string::npos) {
		// Front wheel steering
		return  BoneGroup::STEER;
	}
	else if (boneName.find("Spine") != std::string::npos) {
		// Spine bone
		return  BoneGroup::SPINE;
	}
	else if (boneName.find("Neck") != std::string::npos) {
		// Neck bone
		return  BoneGroup::SPINE;
	}
	else if (boneName.find("Head") != std::string::npos) {
		// Head bone
		return  BoneGroup::SPINE;
	}
	else if (boneName.find("UpperArm") != std::string::npos) {
		// Arms on the bicycle handle
		return  BoneGroup::ARM;
	}
	else if (boneName.find("Forearm") != std::string::npos) {
		// Arms on the bicycle handle
		return  BoneGroup::ARM;
	}

	// No group found
	return BoneGroup::NONE;
}

// Specifies what type of current bone is given
walker::BoneType walker::Model::getAdjustmentType(const std::string& boneName) {
	// Name for the bone within the bone type list
	if (boneName.find("Spine1") != std::string::npos) {
		// Left Up
		return BoneType::SPINE1;
	}
	else if (boneName.find("Spine2") != std::string::npos) {
		// Left Up
		return BoneType::SPINE2;
	}
	else if (boneName.find("Neck") != std::string::npos) {
		// Left Up
		return BoneType::NECK;
	}
	else if (boneName.find("Head") != std::string::npos) {
		// Left Up
		return BoneType::HEAD;
	}
	else if (boneName.find("L UpperArm") != std::string::npos) {
		// Left Up
		return BoneType::LEFT_UPPER_ARM;
	}
	else if (boneName.find("L Forearm") != std::string::npos) {
		// Left lower
		return BoneType::LEFT_FORE_ARM;
	}
	else if (boneName.find("R UpperArm") != std::string::npos) {
		// Right Up
		return BoneType::RIGHT_UPPER_ARM;
	}
	else if (boneName.find("R Forearm") != std::string::npos) {
		// Right lower
		return BoneType::RIGHT_FORE_ARM;
	}

	// No bone type found
	return BoneType::NONE;
}
