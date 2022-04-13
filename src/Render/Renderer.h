#ifndef RENDERER_H
#define RENDERER_H

#include "../World/ChunkMesh.h"
#include "Shader.h"
#include "../Player/Camera.h"
#include "../Window.h"

namespace Renderer
{
	void drawMesh(const Camera& camera, const ChunkMesh& mesh, const Window& window);

	static void prepare(const Camera& camera, const ChunkMesh& mesh, const Window& window);
}

#endif
