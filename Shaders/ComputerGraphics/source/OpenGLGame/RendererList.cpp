#include "RendererList.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

bool RendererList::DEBUG_DRAW = false;
GLuint RendererList::DEBUG_MODE = GL_TRIANGLES;

void RendererList::Add(GameObject *newObject) {
	m_objectList.push_back(newObject);
}

void RendererList::Add(DebugObject *newObject) {
	m_debugList.push_back(newObject);
}

void RendererList::Add(DirLight *newLight) {
	m_dirLight = newLight;
}

void RendererList::Add(PointLight *newLight) {
	m_pointLightList.push_back(newLight);
}

void RendererList::Add(SpotLight *newLight) {
	m_spotLightList.push_back(newLight);
}

void RendererList::SendLightAttributes(ShaderProgram &program, GLCamera &camera) {
	// Directional light properties
	glUniform3fv(program.getUniformLocation("dirLight.direction"), 1, glm::value_ptr(m_dirLight->direction));
	glUniform3fv(program.getUniformLocation("dirLight.ambient"), 1, glm::value_ptr(m_dirLight->ambient)); //0.05f, 0.05f, 0.05f
	glUniform3fv(program.getUniformLocation("dirLight.diffuse"), 1, glm::value_ptr(m_dirLight->diffuse)); //0.4f, 0.4f, 0.4f
	glUniform3fv(program.getUniformLocation("dirLight.specular"), 1, glm::value_ptr(m_dirLight->specular)); //0.5f, 0.5f, 0.5f

	// Point light properties
	for (size_t i = 0; i < m_pointLightList.size(); ++i)
		glUniform3fv(program.getUniformLocation("pointLights[" + std::to_string(i) + "].position"), 1, glm::value_ptr(m_pointLightList[i]->position)),
		glUniform3fv(program.getUniformLocation("pointLights[" + std::to_string(i) + "].ambient"), 1, glm::value_ptr(m_pointLightList[i]->ambient)),
		glUniform3fv(program.getUniformLocation("pointLights[" + std::to_string(i) + "].diffuse"), 1, glm::value_ptr(m_pointLightList[i]->diffuse)),
		glUniform3fv(program.getUniformLocation("pointLights[" + std::to_string(i) + "].specular"), 1, glm::value_ptr(m_pointLightList[i]->specular)),
		glUniform1f(program.getUniformLocation("pointLights[" + std::to_string(i) + "].constant"), m_pointLightList[i]->constant),
		glUniform1f(program.getUniformLocation("pointLights[" + std::to_string(i) + "].linear"), m_pointLightList[i]->linear),
		glUniform1f(program.getUniformLocation("pointLights[" + std::to_string(i) + "].quadratic"), m_pointLightList[i]->quadratic);

	// Spot light properties
	for (size_t i = 0; i < m_spotLightList.size(); ++i)
		glUniform3fv(program.getUniformLocation("spotLights[" + std::to_string(i) + "].position"), 1, glm::value_ptr(m_spotLightList[i]->position)),
		glUniform3fv(program.getUniformLocation("spotLights[" + std::to_string(i) + "].direction"), 1, glm::value_ptr(m_spotLightList[i]->direction)),
		glUniform3fv(program.getUniformLocation("spotLights[" + std::to_string(i) + "].ambient"), 1, glm::value_ptr(m_spotLightList[i]->ambient)),
		glUniform3fv(program.getUniformLocation("spotLights[" + std::to_string(i) + "].diffuse"), 1, glm::value_ptr(m_spotLightList[i]->diffuse)),
		glUniform3fv(program.getUniformLocation("spotLights[" + std::to_string(i) + "].specular"), 1, glm::value_ptr(m_spotLightList[i]->specular)),
		glUniform1f(program.getUniformLocation("spotLights[" + std::to_string(i) + "].constant"), m_spotLightList[i]->constant),
		glUniform1f(program.getUniformLocation("spotLights[" + std::to_string(i) + "].linear"), m_spotLightList[i]->linear),
		glUniform1f(program.getUniformLocation("spotLights[" + std::to_string(i) + "].quadratic"), m_spotLightList[i]->quadratic),
		glUniform1f(program.getUniformLocation("spotLights[" + std::to_string(i) + "].cutOff"), m_spotLightList[i]->cutOff),
		glUniform1f(program.getUniformLocation("spotLights[" + std::to_string(i) + "].outerCutOff"), m_spotLightList[i]->outerCutOff);
}

void RendererList::SendMaterialAttributes(ShaderProgram & program, GLCamera & camera) {
	// Material map properties
	glUniform1i(program.getUniformLocation("material.diffuse"), 0);
	glUniform1i(program.getUniformLocation("material.normal"), 1);
	//glUniform1i(program.getUniformLocation("material.specular"), 2);
}

void RendererList::DrawObjects(ShaderProgram &program, GLCamera &camera) {
	// Send camera matrix to shader (projection + view)
	glUniformMatrix4fv(program.getUniformLocation("camera"), 1, GL_FALSE, glm::value_ptr(camera.PVMatrix()));
	// Set camera position as the viewer
	glUniform3fv(program.getUniformLocation("viewerPosition"), 1, glm::value_ptr(camera.position));
	SendLightAttributes(program, camera);
	glActiveTexture(GL_TEXTURE0);
	for (auto gameObject : m_objectList) {
		// Transform properties
		Transform &transformTemp = gameObject->transform;
		glm::mat4 model = glm::translate(glm::mat4(), transformTemp.position);
		model = glm::rotate(model, glm::radians(transformTemp.rotation.x), { 1,0,0 });
		model = glm::rotate(model, glm::radians(transformTemp.rotation.y), { 0,1,0 });
		model = glm::rotate(model, glm::radians(transformTemp.rotation.z), { 0,0,1 });
		model = glm::scale(model, transformTemp.scale);
		glUniformMatrix4fv(program.getUniformLocation("model"), 1, GL_FALSE, glm::value_ptr(model));
		for (int i = 0; i < gameObject->meshRef->numMeshes; ++i) {
			glBindTexture(GL_TEXTURE_2D, gameObject->materialRef->materialData[i].diffuse.id);
			glUniform3fv(program.getUniformLocation("material.specular"), 1, glm::value_ptr(gameObject->materialRef->materialData[i].specular));
			glUniform3fv(program.getUniformLocation("material.emissive"), 1, glm::value_ptr(gameObject->materialRef->materialData[i].emissive));
			glUniform1f(program.getUniformLocation("material.shininess"), gameObject->materialRef->materialData[i].shininess);
			glBindVertexArray(gameObject->meshRef->meshData[i].vao);
			glDrawElements(GL_TRIANGLES, gameObject->meshRef->meshData[i].numElements, GL_UNSIGNED_INT, nullptr);
		}
		glBindVertexArray(0);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(0);

	///glDrawElementsInstanced(GL_TRIANGLES, meshData[i].numElements, GL_UNSIGNED_INT, nullptr, 1); //TODO
	//glDrawArrays(GL_LINES, /*primer*/, /*num*/);
}

#define RENDER_LIGHT_TEMPLATE() \
		glUniformMatrix4fv(program.getUniformLocation("camera"), 1, GL_FALSE, glm::value_ptr(camera.PVMatrix())); \
		glm::mat4 model = glm::translate(glm::mat4(), gameLight->position); \
		glUniformMatrix4fv(program.getUniformLocation("model"), 1, GL_FALSE, glm::value_ptr(model)); \
		glUniform3fv(program.getUniformLocation("lightColor"), 1, glm::value_ptr(gameLight->diffuse)); \
		glBindVertexArray(debugLight.vao); \
		glDrawElements(GL_TRIANGLES, debugLight.elements, GL_UNSIGNED_INT, nullptr); \
		glBindVertexArray(0);

void RendererList::DrawDebug(ShaderProgram & program, GLCamera &camera) {
	static DebugLight debugLight;
	for (auto gameLight : m_pointLightList) { RENDER_LIGHT_TEMPLATE(); }
	for (auto gameLight : m_spotLightList) { RENDER_LIGHT_TEMPLATE(); }
	for (auto debugObject : m_debugList) {
		// Transform properties
		glm::mat4 model = glm::translate(glm::mat4(), debugObject->transform.position);
		model = glm::rotate(model, glm::radians(debugObject->transform.rotation.x), { 1,0,0 });
		model = glm::rotate(model, glm::radians(debugObject->transform.rotation.y), { 0,1,0 });
		model = glm::rotate(model, glm::radians(debugObject->transform.rotation.z), { 0,0,1 });
		model = glm::scale(model, debugObject->transform.scale);
		glUniformMatrix4fv(program.getUniformLocation("model"), 1, GL_FALSE, glm::value_ptr(model));
		// Material properties
		glUniform3fv(program.getUniformLocation("lightColor"), 1, glm::value_ptr(debugObject->color));
		// Mesh properties
		for (int i = 0; i < debugObject->meshRef->numMeshes; ++i) {
			glBindVertexArray(debugObject->meshRef->meshData[i].vao);
			glDrawElements(DEBUG_MODE, debugObject->meshRef->meshData[i].numElements, GL_UNSIGNED_INT, nullptr);
			glBindVertexArray(0);
		}
	}
}

void RendererList::Clear() {
	m_objectList.clear();
	m_debugList.clear();
	m_dirLight = nullptr;
	m_pointLightList.clear();
	m_spotLightList.clear();
}
