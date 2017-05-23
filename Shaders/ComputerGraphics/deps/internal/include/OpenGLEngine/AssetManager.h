#pragma once
#include <map>
#include "GLMesh.h"
#include "GLMaterial.h"
#include "PathLoader.h"
#include <iostream>

namespace OpenGLEngine {

	class AssetManager {
	public:
		std::map<std::string, GlobalMesh> meshList;
		std::map<std::string, GlobalMaterial> materialList;
		explicit AssetManager() = default;
		~AssetManager() {
			for (auto& entity : meshList)
				glDeleteVertexArrays(1, &entity.second.meshData->vao),
				glDeleteBuffers(1, &entity.second.meshData->vbo),
				glDeleteBuffers(1, &entity.second.meshData->ebo),
				delete[] entity.second.meshData;
			for (auto& entity : materialList) delete[] entity.second.materialData;
		}
		bool Empty() const { return meshList.empty() && materialList.empty(); }
		void Load(const std::string &keyMesh, const std::string &keyMaterial, const std::string &filePath) {
			const aiScene *pScene = aiImportFile(GetPathToAsset(filePath).c_str(), aiProcessPreset_TargetRealtime_MaxQuality); // Load scene
			meshList[keyMesh] = {};
			meshList[keyMesh].numMeshes = pScene->mNumMeshes;
			meshList[keyMesh].meshData = new GLMesh[pScene->mNumMeshes];
			materialList[keyMaterial] = {};
			materialList[keyMaterial].numMaterials = pScene->mNumMeshes; /// TODO: check pScene->mNumMaterials
			materialList[keyMaterial].materialData = new GLMaterial[pScene->mNumMeshes];
			for (unsigned i = 0; i < pScene->mNumMeshes; ++i) {
				meshList[keyMesh].meshData[i].Load(pScene->mMeshes[i]); // Add each mesh to model
				const aiMaterial* material = pScene->mMaterials[pScene->mMeshes[i]->mMaterialIndex]; // Load material from mesh
				int texIndex{ 0 }; aiString path; // To fill variables
				///TODO: check other texture types http://assimp.sourceforge.net/lib_html/material_8h.html#a7dd415ff703a2cc53d1c22ddbbd7dde0
				if (material->GetTexture(aiTextureType_DIFFUSE, texIndex, &path) == AI_SUCCESS) { // Load diffuse texture from material
					std::string aiDiffPath = path.data; // Get texture path
					auto pos = aiDiffPath.rfind("models");
					auto cstrDiffPath = GetPathToAsset(aiDiffPath.substr(pos, aiDiffPath.size() - pos));
					materialList[keyMaterial].materialData[i].diffuse.Load(cstrDiffPath.c_str()); // Create material for mesh
				}
				if (material->GetTexture(aiTextureType_NORMALS, texIndex, &path) == AI_SUCCESS) { // Load diffuse texture from material
					std::string aiNormalPath = path.data; // Get texture path
					auto pos = aiNormalPath.rfind("models");
					auto cstrNormalPath = GetPathToAsset(aiNormalPath.substr(pos, aiNormalPath.size() - pos));
					materialList[keyMaterial].materialData[i].normal.Load(cstrNormalPath.c_str()); // Create material for mesh
				}
			}
			aiReleaseImport(pScene); // Delete scene imported
		}
		void LoadMesh(const std::string &key, const std::string &filePath) {
			const aiScene *pScene = aiImportFile(GetPathToAsset(filePath).c_str(), aiProcessPreset_TargetRealtime_MaxQuality); // Load scene
			meshList[key] = {};
			meshList[key].numMeshes = pScene->mNumMeshes;
			meshList[key].meshData = new GLMesh[pScene->mNumMeshes];
			for (unsigned i = 0; i < pScene->mNumMeshes; ++i)
				meshList[key].meshData[i].Load(pScene->mMeshes[i]); // Add each mesh to model
			aiReleaseImport(pScene); // Delete scene imported
		}
		void LoadMaterial(const std::string &key, const std::string &diffusePath, const std::string &normalPath = "") {
			materialList[key] = {};
			materialList[key].numMaterials = 1;
			materialList[key].materialData = new GLMaterial[1];
			materialList[key].materialData[0].diffuse.Load(GetPathToAsset(diffusePath).c_str());
			if (normalPath != "") materialList[key].materialData[0].normal.Load(GetPathToAsset(normalPath).c_str());
		}
		GlobalMesh &FindMesh(const std::string &key) {
			auto it = meshList.find(key);
			if (it == meshList.end()) SP_THROW_ERROR("Mesh " + key + " not found on global mesh list.");
			return it->second;
		}
		GlobalMaterial &FindMaterial(const std::string &key) {
			auto it = materialList.find(key);
			if (it == materialList.end()) SP_THROW_ERROR("Mesh " + key + " not found on global mesh list.");
			return it->second;
		}
	};

}


/*JsonBox::Value fileData;
fileData.loadFromFile(filePath);
JsonBox::Object wrapper{ fileData.getObject() };
for (auto entity : wrapper) {
std::string key{ entity.first };
JsonBox::Object properties{ entity.second.getObject() };
new (&gameObjectList[key]) GlobalGameObject(entity.first,
GetPathToAsset(properties["model"].getString()).c_str(),
!properties["diffuse"].isNull() ? GetPathToAsset(properties["diffuse"].getString()).c_str() : nullptr,
!properties["normal"].isNull() ? GetPathToAsset(properties["normal"].getString()).c_str() : nullptr,
properties["specular"].getArray(),
!properties["emissive"].isNull() ? properties["emissive"].getArray() : JsonBox::Array{0, 0, 0},
properties["shininess"].getFloat());
/// Load transform attributes
JsonBox::Array tempArray = properties["position"].getArray();
gameObjectList[key].transform.position = { tempArray[0].getFloat(), tempArray[1].getFloat(), tempArray[2].getFloat() };
tempArray = properties["rotation"].getArray();
gameObjectList[key].transform.rotation = { tempArray[0].getFloat(), tempArray[1].getFloat(), tempArray[2].getFloat() };
tempArray = properties["scale"].getArray();
gameObjectList[key].transform.scale = { tempArray[0].getFloat(), tempArray[1].getFloat(), tempArray[2].getFloat() };
}*/
