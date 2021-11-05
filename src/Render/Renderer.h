#ifndef RENDERER_H
#define RENDERER_H

#include "../World/ChunkMesh.h"
#include "Shader.h"
#include "../Player/Camera.h"

namespace Renderer
{
	void drawMesh(const Camera& camera, const ChunkMesh& mesh);

	static void prepare(const Camera& camera, const ChunkMesh& mesh);
}

#endif
