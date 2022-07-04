#ifndef SECTION_LOCATION_H
#define SECTION_LOCATION_H

#include "../Math/Vector2i.h"

class Chunk;

struct SectionLocation
{
	int sectionIndex;
	Vector2i worldPos;
	Chunk* containingChunk;
};

#endif