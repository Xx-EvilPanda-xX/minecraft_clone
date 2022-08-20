#ifndef QUEUE_BLOCK_H
#define QUEUE_BLOCK_H

#include "../World/Block.h"
#include "../Math/Vector3i.h"
#include "SectionLocation.h"

struct QueueBlock
{
	Block block;
	Vector3i sectionRelativePos;
	SectionLocation loc;
};

bool operator== (const QueueBlock& qb1, const QueueBlock& qb2);
#endif