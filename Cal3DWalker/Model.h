#pragma once

#include <GL\glew.h>
#include <memory>
#include <map>

#include <cal3d\global.h>

#include "Definitions.h"

#define MOD_NUM_BUFFERS 4
#define MOD_NUM_FLOAT_VRT 3
#define MOD_NUM_FLOAT_CRD 2
#define MOD_NUM_VERTICES 50000
#define MOD_NUM_FACES 90000

struct CalMixerBoneAdjustment;
class CalCoreModel;
class CalSkeleton;
class CalModel;

namespace walker {

// Cal3D model
class Model {
public:
	// Destructor
	virtual ~Model();

	// Returns a pointer to the Cal3DMgr singleton
	static Model* instance();

	// Creates the core model instance
	CalCoreModel* createCoreModel(const std::string& coreModelName);

	// Removes the core model instance
	GLvoid removeCoreModel(const std::string& coreModelName);

	// Set graphics info
	GLboolean setGeometryInfo(const std::string& coreModelName, const GeometryInfo& graphicsData);

	// Set assets info
	GLboolean setAssetsInfo(const std::string& coreModelName, const AssetType type, const std::string& asset);

	// Set scenario info
	GLboolean setScenarioInfo(const std::string& coreModelName, const ScenarioInfo& scenarioInfo);

	// Set mesh data struct
	GLboolean setMeshData(const std::string& coreModelName, const std::string& meshName, const MeshData& meshData);

	// Set mesh appendance
	GLboolean setMeshAppend(const std::string& coreModelName, const std::string& meshName, GLboolean append);

	// Set material data struct
	GLboolean setMaterialData(const std::string& coreModelName, const std::string& materialName, const MaterialData& materialData);

	// Set texture data
	GLboolean setTextureID(const CalCoreModel* calCoreModel, const std::string& texName, GLuint texID);

	// Set animation data struct
	GLboolean setAnimationData(const std::string& coreModelName, const std::string& animationName, const AnimationData& animationData);

	// Set compound data struct
	GLboolean setCompoundData(const std::string& coreModelName, const std::string& compoundName, const CompoundData& compoundData);

	// Set action for repeat
	GLboolean setRepeatAction(const std::string& coreModelName, const std::string& animationName, const GLfloat duration);

	// Returns core model list
	GLboolean getCoreModelList(std::vector<std::string>& coreModels);

	// Returns a const pointer to the core model instance
	CalCoreModel* getCoreModel(const std::string& coreModelName);

	// Returns a const pointer to the graphics info struct
	const GeometryInfo* getGeometryInfo(const CalCoreModel* calCoreModel);

	// Returns a const pointer to the assets info struct
	const AssetsInfo* getAssetsInfo(const CalCoreModel* calCoreModel);

	// Returns a const pointer to the (next) scenario info struct
	const ScenarioInfo* getScenarioInfo(const CalCoreModel* calCoreModel, const GLfloat deltaTime);

	// Returns a const pointer to the mesh data struct
	const MeshData* getMeshData(const CalCoreModel* calCoreModel, const std::string& meshName);

	// Returns a const pointer to the material data struct
	const MaterialData* getMaterialData(const CalCoreModel* calCoreModel, const std::string& materialName);

	// Returns a const pointer to the animation data struct
	const AnimationData* getAnimationData(const CalCoreModel* calCoreModel, const std::string& animationName);

	// Returns a const pointer to the compound data struct
	const CompoundData* getCompoundData(const CalCoreModel* calCoreModel, const std::string& compoundName);

	// Returns action removal delay
	GLfloat* getRepeatAction(const CalCoreModel* calCoreModel, const std::string& animationName);

	// CalModel local orientation
	GLfloat getDirectionAngle(const CalModel* pModel, const glm::vec3 direction) const;

	// Remove scenario event from the list
	GLboolean removeScenarioInfo(const CalCoreModel* calCoreModel, const ScenarioInfo* info);

	// Update on model deactivation status
	GLboolean hasModelExpired(CalModel* pModel, CalModel* pAddModel);

	// Update on model activation status
	GLboolean hasModelStarted(CalModel* pModel, CalModel* pAddModel, const ScenarioInfo* info);

	// Remove additional meshes
	GLboolean detachMesh(CalModel* pModel, GLboolean isAppend);

	// Add appended meshes
	GLboolean attachMesh(CalModel* pModel);

	// Remove cycles
	GLboolean clearCycle(CalModel* pModel, const std::string& animationName = "");

	// Add cycle
	GLboolean assignCycle(CalModel* pModel, const ScenarioInfo* info);

	// Remove actions
	GLboolean clearAction(CalModel* pModel, const std::string& animationName = "");

	// Add action
	GLboolean assignAction(CalModel* pModel, const ScenarioInfo* info);

	// Add action as cycle
	GLvoid loopAction(CalModel* pModel, const GLfloat deltaTime);

	// Remove action repeat
	GLvoid removeAction(const CalCoreModel* calCoreModel, const std::string& animationName);

	// Add absolute adjustments to skeleton
	GLboolean absBoneAdjustment(CalModel* pModel, const glm::vec3 dir, const glm::vec3 aim);

	// Add relative adjustments to skeleton
	GLboolean relBoneAdjustment(CalModel* pModel, GLfloat speed, GLfloat steer, const GLfloat deltaTime);

	// Render model meshes
	GLvoid drawMesh(const GLuint programID, CalModel* pModel);

	// Render model skeleton
	GLvoid drawSkeleton(const CalSkeleton* pSkeleton);

	// Render model bounding box
	GLvoid drawBoundingBox(CalSkeleton* pSkeleton);

	// Returns vao buffer
	const GLuint getVertexArrayID() const;

private:
	// Stores core model and database maps
	struct CalCoreModelData {
		// Core model instance
		std::shared_ptr<CalCoreModel> m_calCoreModel;
		// Graphics info
		std::shared_ptr<GeometryInfo> m_geometryInfo;
		// Core model assets info
		std::shared_ptr<AssetsInfo> m_assetsInfo;
		// Core model scenario event list
		std::list<std::shared_ptr<ScenarioInfo>> m_scenarioInfoLst;
		// map of meshes <name, meshData>
		std::map<std::string, std::shared_ptr<MeshData>> m_meshMap;
		// map of materials <name, materialData>
		std::map<std::string, std::shared_ptr<MaterialData>> m_materialMap;
		// map of animations <name, animationData>
		std::map<std::string, std::shared_ptr<AnimationData>> m_animationMap;
		// map of compounds <name, compoundData>
		std::map<std::string, std::shared_ptr<CompoundData>> m_compoundMap;
		// map of active actions < name, duration >
		std::map<std::string, GLfloat> m_repActMap;
	};

	// Buffers
	GLuint m_vao;
	GLuint m_vbo[MOD_NUM_BUFFERS];

	GLfloat m_vert[MOD_NUM_VERTICES][MOD_NUM_FLOAT_VRT];
	GLfloat m_norm[MOD_NUM_VERTICES][MOD_NUM_FLOAT_VRT];
	GLfloat m_txco[MOD_NUM_VERTICES][MOD_NUM_FLOAT_CRD];
	CalIndex m_face[MOD_NUM_FACES][MOD_NUM_FLOAT_VRT];

	Model();
	Model(const Model&);
	Model& operator=(const Model&);

	// Returns vao buffer
	GLuint getVbo(const GLuint index) const;

	// Returns verticies buffer
	GLfloat* getVert() const;

	// Returns normal buffer
	GLfloat* getNorm() const;

	// Returns coords buffer
	GLfloat* getCoord() const;

	// Returns faces buffer
	CalIndex* getFace() const;

	// OpenGL buffers
	GLvoid clearBuffers();

	// Specifies what group of current bone is given
	BoneGroup getAdjustmentGroup(const std::string& boneName);

	// Specifies what type of current bone is given
	BoneType getAdjustmentType(const std::string& boneName);

private:
	// Cal3DMgr singleton class instance pointer
	static std::unique_ptr<Model> m_pModel;

	// Core model data map < name, struct >
	std::map<std::string, CalCoreModelData> m_calCoreModelMap;

	// Use material
	GLboolean m_useMaterial;

	// Time tracker
	GLfloat m_scTime;

	// Bone adjustment adder
	std::shared_ptr<CalMixerBoneAdjustment> m_boneAdjustMix;

	// Max bone rotation factor (0 - .99)
	const GLfloat BONE_ROT_MAX;

	// Bone adjusting <type, rotation factor>
	std::map<BoneType, GLfloat> m_boneAdjustMap;
};

}