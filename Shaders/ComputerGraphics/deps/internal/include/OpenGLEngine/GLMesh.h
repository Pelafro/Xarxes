#pragma once
#include <GL/glew.h>
#include <vector>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#pragma comment(lib, "assimp.lib") 

namespace OpenGLEngine {

	struct GLMesh {
		std::vector<float> vertices;		// Array of mesh vertices
		std::vector<unsigned int> indices;	// Array of mesh elements
		GLuint vao, vbo, ebo;				/// TODO: optimize 1 VAO & 1 VBO for multiple meshes
		unsigned int numElements;			// How many faces does the mesh have
		unsigned int materialID;			// Which material should be binded to the mesh
		explicit GLMesh() = default;
		GLMesh(aiMesh* mesh) { Load(mesh); }
		void Load(aiMesh* mesh) {
			const int nv = mesh->mNumVertices;
			const bool foo = mesh->HasTextureCoords(0);
			vertices.reserve(nv);
			for (int i = 0; i < nv; ++i)
				vertices.push_back(mesh->mVertices[i].x), vertices.push_back(mesh->mVertices[i].y), vertices.push_back(mesh->mVertices[i].z),
				vertices.push_back(mesh->mNormals[i].x), vertices.push_back(mesh->mNormals[i].y), vertices.push_back(mesh->mNormals[i].z),
				vertices.push_back((foo) ? mesh->mTextureCoords[0][i].x : 0.f), vertices.push_back((foo) ? mesh->mTextureCoords[0][i].y : 0.f);
			const int nf = mesh->mNumFaces;
			indices.reserve(nf);
			for (int i = 0; i < nf; ++i)
				indices.push_back(mesh->mFaces[i].mIndices[0]), indices.push_back(mesh->mFaces[i].mIndices[1]), indices.push_back(mesh->mFaces[i].mIndices[2]);
			numElements = indices.size();
			///Create VAO
			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);
			///Create VBO
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), &vertices[0], GL_STATIC_DRAW);
			///Create EBO
			glGenBuffers(1, &ebo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, numElements*sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
			///Configure vertex input
			glVertexAttribPointer(GLuint(0), 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
			glEnableVertexAttribArray(0); // position
			glVertexAttribPointer(GLuint(1), 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
			glEnableVertexAttribArray(1); // normal
			glVertexAttribPointer(GLuint(2), 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(6 * sizeof(float)));
			glEnableVertexAttribArray(2); // uv
			glBindVertexArray(0);
		}
	};

	struct GlobalMesh {
		GLMesh *meshData{ nullptr };
		int numMeshes{ 0 };
	};

}
