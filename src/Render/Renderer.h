#ifndef RENDERER_H
#define RENDERER_H

#include "../World/ChunkMesh.h"
#include "Shader.h"

namespace Renderer
{
	void drawMesh(Shader& shader, ChunkMesh& mesh);
}

#endif
