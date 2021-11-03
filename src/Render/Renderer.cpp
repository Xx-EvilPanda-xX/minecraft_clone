#include <glad/glad.h>
#include "Renderer.h"
#include "Shader.h"
#include "../World/ChunkMesh.h"

namespace Renderer
{
	void drawMesh(Shader& shader, Camera& camera, ChunkMesh& mesh)
	{
		shader.bind();

		glBindVertexArray(mesh.getRenderData().vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.getRenderData().ebo);
		mesh.enableAttribs();

		prepare(shader, camera, mesh);
		glDrawElements(GL_TRIANGLES, mesh.getRenderData().indexCount, GL_UNSIGNED_INT, 0);

		mesh.disableAttribs();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		shader.unbind();
	}

	static void prepare(Shader& shader, Camera& camera, ChunkMesh& mesh)
	{
		glm::mat4 model{};
		glm::vec3 chunkPostion{ static_cast<float>(mesh.getPostion().x), 0.0f, static_cast<float>(mesh.getPostion().y) };

		glm::translate(model, chunkPostion);

		shader.setMat4("model", glm::mat4{} );
		shader.setMat4("view", camera.getViewMat());
		shader.setMat4("proj", camera.getProjectionMat());
	}
}

