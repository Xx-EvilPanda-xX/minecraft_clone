#ifndef RENDER_DATA_H
#define RENDER_DATA_H

#include "Shader.h"

struct RenderData
{
	unsigned int vao;
	unsigned int vbo;
	unsigned int tbo;
	unsigned int ebo;
	unsigned int lbo;

	int indexCount;

	Shader& shader;

	unsigned int texture;
};

#endif
