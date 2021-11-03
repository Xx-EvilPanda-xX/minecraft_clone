#ifndef RENDERER_H
#define RENDERER_H

#include "../World/ChunkMesh.h"
#include "Shader.h"
#include "../Player/Camera.h"

namespace Renderer
{
	void drawMesh(Shader& shader, Camera& camera, ChunkMesh& mesh);

	static void prepare(Shader& shader, Camera& camera, ChunkMesh& mesh);
}

#endif
