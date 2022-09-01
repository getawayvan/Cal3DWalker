// Loader
#include <dirent\dirent.h>
#include <cal3d\cal3d.h>
#include <SOIL\SOIL.h>

#include "Window.h"
#include "Model.h"
#include "Gui.h"
#include "Loader.h"

walker::Loader::Loader(Window* window) : m_pWindow(window) {}

GLboolean walker::Loader::load(const std::string& directory) {
	// Locals
	GLboolean isLd = false;
	std::vector<std::string> xmlFiles;

	// Read directory content
	if (loadDirectory(directory, xmlFiles)) {
		// loading start
		CEGUI::ProgressBar* pProBar = static_cast<CEGUI::ProgressBar*>(walker::Gui::instance()->getRoot()->getChild("loadProBar"));
		if (pProBar && xmlFiles.size() > 2) {
			// progress bar step size (0.15 - asset types coeff)
			pProBar->setStepSize(0.15f / (xmlFiles.size() - 2));
			pProBar->step();
		}

		// File content
		std::shared_ptr<cal3d::TiXmlDocument> xmlDocument = std::make_shared<cal3d::TiXmlDocument>();

		// Read all files
		for (size_t i = 0; i < xmlFiles.size(); ++i) {
			// Skip if not file
			if (xmlFiles.at(i) == "." || xmlFiles.at(i) == "..") continue;

			// Load xml file
			const std::string filepath(directory + "/" + xmlFiles.at(i));
			if (xmlDocument->LoadFile(filepath.c_str())) {
				// Get root node of Cal3D configuration
				cal3d::TiXmlNode* rootNodeCal3D = xmlDocument->FirstChild("Cal3DConfig");
				if (rootNodeCal3D) {
					// Get skeleton, mesh, material, texture and animation
					isLd = loadStructure(rootNodeCal3D)
						&& loadGraphics(rootNodeCal3D)
						&& loadDynamics(rootNodeCal3D);

					// Confirm config status
					if (!isLd) return false;

					// Scale coremodels
					std::vector<std::string> coreModels;
					walker::Model::instance()->getCoreModelList(coreModels);
					for (auto& coreModel : coreModels) {
						// Iterate through coremodels
						CalCoreModel* pCoreModel = walker::Model::instance()->getCoreModel(coreModel);
						const walker::GeometryInfo* pGeometryInfo = walker::Model::instance()->getGeometryInfo(pCoreModel);
						if (pGeometryInfo && pGeometryInfo->m_scale != 1.0f)
							pCoreModel->scale((GLfloat)pGeometryInfo->m_scale);
					}
				}

				// Clear content
				xmlDocument->Clear();
			}
		}
	}

	// Config end
	return isLd;
}

GLboolean walker::Loader::loadDirectory(const std::string& directory, std::vector<std::string>& xmlFiles) {
	// Open folder
	struct dirent* ent = nullptr;
	DIR* dir = opendir(directory.c_str());
	if (dir) {
		// Read folder
		while (true) {
			// Read file
			ent = readdir(dir);
			if (ent)
				// Remember filename
				xmlFiles.push_back(ent->d_name);
			else
				// end
				break;
		}

		// Close
		closedir(dir);
		return true;
	}

	// No folder
	return false;
}

GLboolean walker::Loader::loadStructure(cal3d::TiXmlNode* pNode) {
	// Reading skeleton files
	cal3d::TiXmlElement* pElementSkeleton = pNode->FirstChildElement("Skeleton");
	while (pElementSkeleton) {
		const GLchar* pSkeletonName = pElementSkeleton->Attribute("name");
		const GLchar* pSkeletonFile = pElementSkeleton->Attribute("filename");
		const GLchar* pSkeletonPath = pElementSkeleton->Attribute("filepath");
		if (pSkeletonName && pSkeletonFile && pSkeletonPath) {
			// Create core model
			CalCoreModel* calCoreModel = walker::Model::instance()->createCoreModel(pSkeletonName);
			if (calCoreModel) {
				// Create character type
				if (calCoreModel->loadCoreSkeleton(std::string(pSkeletonPath) + "/" + std::string(pSkeletonFile))) {
					// remember path to files
					m_filepath.assign(pSkeletonPath);

					// locals
					GLdouble scale = 1.0;
					GLint x = 0, y = -1;

					// Scale core model
					pElementSkeleton->QueryDoubleAttribute("scale", &scale);

					// Orientation
					pElementSkeleton->QueryIntAttribute("x", &x);
					pElementSkeleton->QueryIntAttribute("y", &y);

					// default graphics data
					walker::GeometryInfo geometryInfo((GLfloat)scale, glm::ivec2(x, y));

					// Remember graphics data
					walker::Model::instance()->setGeometryInfo(pSkeletonName, geometryInfo);
				}
				else {
					// Skeleton file load fail
					CalError::printLastError();
					walker::Model::instance()->removeCoreModel(pSkeletonName);
					return false;
				}
			}
		}

		pElementSkeleton = pElementSkeleton->NextSiblingElement("Skeleton");
	} // end of all skeletons

	  // progress
	updateProBar();

	  // structure
	return true;
}

GLboolean walker::Loader::loadGraphics(cal3d::TiXmlNode* pNode) {
	// Loading model template files
	cal3d::TiXmlElement* pElementTemplate = pNode->FirstChildElement("Model");
	while (pElementTemplate) {
		// Identify skeleton
		const GLchar* pSkeletonName = pElementTemplate->Attribute("skeleton");
		// Identify coremodel
		CalCoreModel* calCoreModel = walker::Model::instance()->getCoreModel(pSkeletonName);

		// Verify skeleton and coremodel
		if (pSkeletonName && calCoreModel) {
			// Loading mesh files
			cal3d::TiXmlElement* pElementMeshes = pElementTemplate->FirstChildElement("Meshes");
			if (pElementMeshes) {
				// Identify model
				const GLchar* pMeshType = pElementMeshes->Attribute("path");

				cal3d::TiXmlElement* pElementMesh = pElementMeshes->FirstChildElement("Mesh");
				while (pElementMesh) {
					// Identify appearance
					const GLchar* pMeshName = pElementMesh->Attribute("name");
					const GLchar* pMeshTex = pElementMesh->Attribute("texture");
					const GLchar* pMeshFile = pElementMesh->Attribute("filename");
					if (pMeshName && pMeshFile) {
						// Compare attribute to existing data
						const std::string pMesh = pMeshType ? (std::string)pMeshType + "/" + pMeshFile : pMeshFile;
						// load mesh into coremodel
						GLint meshID = calCoreModel->loadCoreMesh(m_filepath + "/" + pMesh);
						if (meshID == -1) {
							CalError::printLastError();
							return false;
						}

						// append mesh
						GLint append = 0;
						pElementMesh->QueryIntAttribute("append", &append);

						// Remember mesh data
						walker::MeshData meshData((GLboolean)append, 0, pMeshTex);
						if (walker::Model::instance()->setMeshData(calCoreModel->getName(), pMeshName, meshData)) {
							// Remember mesh name
							if (!walker::Model::instance()->setAssetsInfo(pSkeletonName, AssetType::MESH, pMeshName))
								return false;

							// Add mesh name to the coremodel
							if (!calCoreModel->addMeshName(pMeshName, meshID)) {
								CalError::printLastError();
								return false;
							}
						}
					}

					pElementMesh = pElementMesh->NextSiblingElement("Mesh");
				} // end of all model meshes

				  // progress
				updateProBar();
			}

			// Loading item files
			cal3d::TiXmlElement* pElementItems = pElementTemplate->FirstChildElement("Items");
			if (pElementItems) {
				// Identify model
				const GLchar* pMeshType = pElementItems->Attribute("path");

				cal3d::TiXmlElement* pElementItem = pElementItems->FirstChildElement("Item");
				while (pElementItem) {
					// Identify appearance
					const GLchar* pMeshName = pElementItem->Attribute("name");
					const GLchar* pMeshTex = pElementItem->Attribute("texture");
					const GLchar* pMeshFile = pElementItem->Attribute("filename");
					if (pMeshName && pMeshFile) {
						// Compare attribute to existing data
						const std::string pMesh = pMeshType ? std::string(pMeshType) + "/" + pMeshFile : pMeshFile;
						// Load mesh into coremodel
						GLint meshID = calCoreModel->loadCoreMesh(m_filepath + "/" + pMesh);
						if (meshID == -1) {
							CalError::printLastError();
							return false;
						}

						// Append item
						GLint append = 0;
						pElementItem->QueryIntAttribute("append", &append);

						// Remember mesh data
						walker::MeshData meshData(append, 0, pMeshTex);
						if (walker::Model::instance()->setMeshData(calCoreModel->getName(), pMeshName, meshData)) {
							// Remember item name
							if (!walker::Model::instance()->setAssetsInfo(pSkeletonName, AssetType::ITEM, pMeshName))
								return false;

							// Add mesh name to the coremodel
							if (!calCoreModel->addMeshName(pMeshName, meshID)) {
								CalError::printLastError();
								return false;
							}
						}
					}

					pElementItem = pElementItem->NextSiblingElement("Item");
				} // end of all model items

				  // progress
				updateProBar();
			}

			// Loading material files
			cal3d::TiXmlElement* pElementMaterials = pElementTemplate->FirstChildElement("Materials");
			if (pElementMaterials) {
				// Identify model
				const GLchar* pMaterialType = pElementMaterials->Attribute("path");

				cal3d::TiXmlElement* pElementMaterial = pElementMaterials->FirstChildElement("Material");
				while (pElementMaterial) {
					// Identify appearance
					const GLchar* pMaterialName = pElementMaterial->Attribute("name");
					const GLchar* pMaterialTex = pElementMaterial->Attribute("texture");
					const GLchar* pMaterialFile = pElementMaterial->Attribute("filename");
					if (pMaterialName && pMaterialFile) {
						// Compare attribute to existing data
						const std::string pMaterial = pMaterialType ? (std::string)pMaterialType + "/" + pMaterialFile : pMaterialFile;
						// load material into coremodel
						GLint materialID = calCoreModel->loadCoreMaterial(m_filepath + "/" + pMaterial);
						if (materialID == -1) {
							CalError::printLastError();
							return false;
						}

						// append mesh
						GLint append = 0;
						pElementMaterial->QueryIntAttribute("append", &append);

						// Remember material data
						walker::MaterialData materialData((GLboolean)append, 0, pMaterialTex);
						if (walker::Model::instance()->setMaterialData(calCoreModel->getName(), pMaterialName, materialData)) {
							// Remember material name
							if (!walker::Model::instance()->setAssetsInfo(pSkeletonName, AssetType::MATERIAL, pMaterialName))
								return false;

							// Add material name to coremodel
							if (!calCoreModel->addMaterialName(pMaterialName, materialID)) {
								CalError::printLastError();
								return false;
							}
						}
					}

					pElementMaterial = pElementMaterial->NextSiblingElement("Material");
				} // end of all model material

				  // progress
				updateProBar();
			}

			// Loading texture files
			cal3d::TiXmlElement* pElementTextures = pElementTemplate->FirstChildElement("Textures");
			if (pElementTextures) {
				// Identify model
				const GLchar* pTexturePath = pElementTextures->Attribute("path");

				cal3d::TiXmlElement* pElementTexture = pElementTextures->FirstChildElement("Texture");
				while (pElementTexture) {
					// Identify appearance
					const GLchar* pTextureName = pElementTexture->Attribute("name");
					const GLchar* pTextureFile = pElementTexture->Attribute("filename");
					if (pTextureName && pTextureFile) {
						// Compare attribute to existing data
						const std::string pTexture = pTexturePath ? (std::string)pTexturePath + "/" + pTextureFile : pTextureFile;

						// Load texture file
						GLuint texID = loadTexture(m_filepath + "/" + pTexture);

						// Remember texture name
						if (!walker::Model::instance()->setAssetsInfo(pSkeletonName, AssetType::TEXTURE, pTextureName))
							return false;

						// Remember texture data
						walker::Model::instance()->setTextureID(calCoreModel, pTextureName, texID);
					}

					pElementTexture = pElementTexture->NextSiblingElement("Texture");
				} // end of all model texure

				  // progress
				updateProBar();
			}
		}

		pElementTemplate = pElementTemplate->NextSiblingElement("Model");
	} // end of all model templates

	  // graphics
	return true;
}

GLboolean walker::Loader::loadDynamics(cal3d::TiXmlNode* pNode) {
	// Loading model dynamics files
	cal3d::TiXmlElement* pElementTemplate = pNode->FirstChildElement("Model");
	while (pElementTemplate) {
		// Identify skeleton
		const GLchar* pSkeletonName = pElementTemplate->Attribute("skeleton");

		// Reading cycle animation files
		cal3d::TiXmlElement* pElementCycles = pElementTemplate->FirstChildElement("Cycles");
		if (pSkeletonName && pElementCycles) {
			// Identify animation type
			const GLchar* pCycleType = pElementCycles->Attribute("path");

			cal3d::TiXmlElement* pElementCycle = pElementCycles->FirstChildElement("Cycle");
			while (pElementCycle) {
				const GLchar* pCycleName = pElementCycle->Attribute("name");
				const GLchar* pCycleFile = pElementCycle->Attribute("filename");
				if (pCycleName && pCycleFile) {
					// Get the core model
					CalCoreModel* calCoreModel = walker::Model::instance()->getCoreModel(pSkeletonName);
					if (calCoreModel) {
						// Compare attribute to existing data
						const std::string pAnimation = pCycleType ? (std::string)pCycleType + "/" + pCycleFile : pCycleFile;
						// load animation into coremodel
						GLint animationID = calCoreModel->loadCoreAnimation(m_filepath + "/" + pAnimation);
						if (animationID == -1) {
							CalError::printLastError();
							return false;
						}

						// Store animation name in the core model animtion map
						calCoreModel->addAnimationName(pCycleName, animationID);

						// Store animation additional data
						GLdouble weight = 1.0, speed = 0.0, blend = 0.0;
						pElementCycle->QueryDoubleAttribute("weight", &weight);
						pElementCycle->QueryDoubleAttribute("speed", &speed);
						pElementCycle->QueryDoubleAttribute("blend", &blend);

						walker::AnimationData animationData((GLfloat)weight, (GLfloat)speed, (GLfloat)blend);

						// mesh assigned to animation
						const GLchar* pCycleMesh = pElementCycle->Attribute("mesh");
						if (pCycleMesh) animationData.m_mesh = pCycleMesh;

						// compound assigned to animation
						const GLchar* pCycleComp = pElementCycle->Attribute("compound");
						if (pCycleComp) animationData.m_compound = pCycleComp;

						// Remember cycle name
						if (!walker::Model::instance()->setAssetsInfo(pSkeletonName, AssetType::CYCLE, pCycleName))
							return false;

						// Remember animation data
						walker::Model::instance()->setAnimationData(pSkeletonName, pCycleName, animationData);
					}
				}

				pElementCycle = pElementCycle->NextSiblingElement("Cycle");
			} // end of all cycle animation files

			  // progress
			updateProBar();
		}

		// Loading action animation files
		cal3d::TiXmlElement* pElementActions = pElementTemplate->FirstChildElement("Actions");
		if (pSkeletonName && pElementActions) {
			// Identify animation type
			const GLchar* pActionType = pElementActions->Attribute("path");

			cal3d::TiXmlElement* pElementAction = pElementActions->FirstChildElement("Action");
			while (pElementAction) {
				const GLchar* pActionName = pElementAction->Attribute("name");
				const GLchar* pActionFile = pElementAction->Attribute("filename");
				if (pActionName && pActionFile) {
					// Get the core model
					CalCoreModel* calCoreModel = walker::Model::instance()->getCoreModel(pSkeletonName);
					if (calCoreModel) {
						// Compare attribute to existing data
						const std::string pAnimation = pActionType ? (std::string)pActionType + "/" + pActionFile : pActionFile;
						// load animation into coremodel
						GLint animationID = calCoreModel->loadCoreAnimation(m_filepath + "/" + pAnimation);
						if (animationID == -1) {
							// Gesture file load fail
							CalError::printLastError();
							return false;
						}

						// Store animation name in the core model animtion map
						calCoreModel->addAnimationName(pActionName, animationID);

						// Store animation additional data
						GLdouble weight = 1.0, speed = 0.0, blend = 0.0;
						pElementAction->QueryDoubleAttribute("weight", &weight);
						pElementAction->QueryDoubleAttribute("speed", &speed);
						pElementAction->QueryDoubleAttribute("blend", &blend);

						walker::AnimationData animationData((GLfloat)weight, (GLfloat)speed, (GLfloat)blend);

						// mesh assigned to animation
						const GLchar* pActionMesh = pElementAction->Attribute("mesh");
						if (pActionMesh) animationData.m_mesh = pActionMesh;

						// compound assigned to animation
						const GLchar* pActionComp = pElementAction->Attribute("compound");
						if (pActionComp) animationData.m_compound = pActionComp;

						// Remember action name
						if (!walker::Model::instance()->setAssetsInfo(pSkeletonName, AssetType::ACTION, pActionName))
							return false;

						// Remember action data
						walker::Model::instance()->setAnimationData(pSkeletonName, pActionName, animationData);
					}
				}

				pElementAction = pElementAction->NextSiblingElement("Action");
			} // end of all action animation files

			  // progress
			updateProBar();
		}

		// Loading model compounds
		cal3d::TiXmlElement* pElementCompounds = pElementTemplate->FirstChildElement("Compounds");
		if (pSkeletonName && pElementCompounds) {
			// Identify animation type
			cal3d::TiXmlElement* pElementCompound = pElementCompounds->FirstChildElement("Compound");
			while (pElementCompound) {
				const GLchar* pCompoundName = pElementCompound->Attribute("name");
				const GLchar* pCompoundSkeleton = pElementCompound->Attribute("skeleton");
				if (pCompoundName && pCompoundSkeleton) {
					// Create a compound
					walker::CompoundData compoundData(pCompoundSkeleton);

					// Cycle
					const GLchar* pCompoundCycle = pElementCompound->Attribute("cycle");
					if (pCompoundCycle) compoundData.m_cycle = pCompoundCycle;

					// Action
					const GLchar* pCompoundAction = pElementCompound->Attribute("action");
					if (pCompoundAction) compoundData.m_action = pCompoundAction;

					// Remember compound data
					walker::Model::instance()->setCompoundData(pSkeletonName, pCompoundName, compoundData);
				}

				pElementCompound = pElementCompound->NextSiblingElement("Compound");
			}
		} // end of all compounds

		pElementTemplate = pElementTemplate->NextSiblingElement("Model");
	} // end of all model dynamics

	  // Dynamics
	return true;
}

// Create textures
GLuint walker::Loader::loadTexture(const std::string& filePath) {
	GLuint texID = 0;
	glGenTextures(1, &texID);
	GLint texWidth = 0, texHeight = 0;
	GLubyte* texImage = (GLubyte*)SOIL_load_image(filePath.c_str(), &texWidth, &texHeight, 0, SOIL_LOAD_RGB);

	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, texImage);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(texImage);

	// texture ID
	return texID;
}


GLvoid walker::Loader::updateProBar() {
	// loading start
	CEGUI::ProgressBar* pProBar = static_cast<CEGUI::ProgressBar*>(walker::Gui::instance()->getRoot()->getChild("loadProBar"));
	if (pProBar) {
		// progress
		pProBar->step();

		// draw bar
		m_pWindow->clear();
		walker::Gui::instance()->draw();
		m_pWindow->swap();
	}
}
