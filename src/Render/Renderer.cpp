#include <glad/glad.h>
#include "Renderer.h"
#include "Shader.h"
#include "../World/ChunkMesh.h"

namespace Renderer
{
	void drawMesh(Shader& shader, ChunkMesh& mesh)
	{
		shader.bind();

		glBindVertexArray(mesh.getRenderData().vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.getRenderData().ebo);
		mesh.enableAttribs();

		glDrawElements(GL_TRIANGLES, mesh.getRenderData().indexCount, GL_UNSIGNED_INT, 0);

		mesh.disableAttribs();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		shader.unbind();
	}
}

