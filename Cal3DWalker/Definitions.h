#pragma once

#include <GL\glew.h>
#include <glm\glm.hpp>
#include <string>
#include <vector>

namespace walker {

// Type of model asset
enum class AssetType {
	MESH,
	ITEM,
	MATERIAL,
	TEXTURE,
	CYCLE,
	ACTION
};

// Bone Group
enum class BoneGroup {
	NONE,
	WHEEL,
	STEER,
	SPINE,
	ARM
};

// Bone Type
enum class BoneType {
	NONE,
	SPINE1,
	SPINE2,
	NECK,
	HEAD,
	LEFT_UPPER_ARM,
	LEFT_FORE_ARM,
	RIGHT_UPPER_ARM,
	RIGHT_FORE_ARM
};

// Stores geometry info
struct GeometryInfo {
	GeometryInfo(GLfloat scale, const glm::ivec2 direction);

	// Scale
	GLfloat m_scale;
	// Model heading
	glm::ivec2 m_direction;
};

// Stores model info
struct AssetsInfo {
	// Body meshes
	std::vector<std::string> m_meshes;
	// Object meshes
	std::vector<std::string> m_items;
	// Model materials
	std::vector<std::string> m_materials;
	// Model textures
	std::vector<std::string> m_textures;
	// Cycle animations
	std::vector<std::string> m_cycles;
	// Action animations
	std::vector<std::string> m_actions;
};

// Stores event info
struct ScenarioInfo {
	ScenarioInfo(const std::string& animation);

	// Animation name
	std::string m_animation;
	// Trigger delay
	GLfloat m_delay;
	// Strength
	GLfloat m_weight;
	// Speed factor
	GLfloat m_scale;
	// Blend in
	GLfloat m_blendIn;
	// Blend out
	GLfloat m_blendOut;
	// Animation length
	GLfloat m_duration;
};

// Stores a mesh additional data
struct MeshData {
	MeshData(GLboolean append, GLuint texID, const std::string& texName = "");

	// Append mesh to model
	GLboolean m_append;
	// Texture ID
	GLuint m_texID;
	// Texture name
	std::string m_texName;
};

// Stores a material additional data
struct MaterialData {
	MaterialData(GLboolean append, GLuint texID, const std::string& texName = "");

	// Append material to model
	GLboolean m_append;
	// Texture ID
	GLuint m_texID;
	// Texture name
	std::string m_texName;
};

// Stores a single animation additional data
struct AnimationData {
	AnimationData(GLfloat weight, GLfloat speed, GLfloat blend, const std::string& mesh = "", const std::string& compound = "");

	// Default weight
	GLfloat m_weight;
	// Default speed
	GLfloat m_speed;
	// Blend time factor
	GLfloat m_blend;
	// Additional mesh
	std::string m_mesh;
	// Additional model
	std::string m_compound;
};

// Stores a compound data
struct CompoundData {
	CompoundData(const std::string& skeleton, const std::string& cycle = "", const std::string& action = "");

	// skeleton
	std::string m_skeleton;
	// cycle
	std::string m_cycle;
	// action animation
	std::string m_action;
};

}
