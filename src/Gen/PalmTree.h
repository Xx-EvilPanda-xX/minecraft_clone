#ifndef PALM_TREE_H
#define PALM_TREE_H

#include "Tree.h"

class PalmTree : public Tree
{
public:
	const Vector3i* getLeaves() override;
	int getTrunkHeight() override;
	int getNumLeaves() override;
	BlockType getLeavesType() override;
	BlockType getTrunkType() override;

	PalmTree();
	~PalmTree();
};

#endif