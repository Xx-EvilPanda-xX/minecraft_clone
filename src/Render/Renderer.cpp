#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Renderer.h"
#include "Shader.h"
#include "../World/ChunkMesh.h"

namespace Renderer
{
	void drawMesh(const Camera& camera, const ChunkMesh& mesh, const Window& window)
	{
		mesh.getRenderData().shader->bind();
		glBindTexture(GL_TEXTURE_2D, mesh.getRenderData().texture);

		glBindVertexArray(mesh.getRenderData().vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.getRenderData().ebo);
		mesh.enableAttribs();

		prepare(camera, mesh, window);
		glDrawElements(GL_TRIANGLES, mesh.getRenderData().indexCount, GL_UNSIGNED_INT, 0);

		mesh.disableAttribs();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		glBindTexture(GL_TEXTURE_2D, 0);
		mesh.getRenderData().shader->unbind();
	}

	static void prepare(const Camera& camera, const ChunkMesh& mesh, const Window& window)
	{
		glm::dmat4 model(1.0);
		glm::dvec3 chunkPostion{ static_cast<double>(mesh.getPostion().x), 0.0, static_cast<double>(mesh.getPostion().y) };

		model = glm::translate(model, chunkPostion);

		int windowWidth, windowHeight;
		glfwGetWindowSize(window.getGlfwWindow(), &windowWidth, &windowHeight);

		mesh.getRenderData().shader->setMat4("model", model);
		mesh.getRenderData().shader->setMat4("view", camera.getViewMat());
		mesh.getRenderData().shader->setMat4("proj", camera.getProjectionMat(windowWidth, windowHeight));
	}
}

