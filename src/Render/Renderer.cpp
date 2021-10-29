#include <glad/glad.h>
#include "Renderer.h"
#include "Shader.h"
#include "../World/ChunkMesh.h"

namespace Renderer
{
	void drawMesh(Shader& shader, ChunkMesh& mesh)
	{
		shader.use();
		glBindVertexArray(mesh.getObjects().vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.getObjects().ebo);

		glDrawElements(GL_TRIANGLES, )

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
}

