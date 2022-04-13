#ifndef PALM_TREE_H
#define PALM_TREE_H

#include "Tree.h"

class PalmTree : public Tree
{
public:
	const Vector3i* getLeaves() const override;
	int getTrunkHeight() const override;
	int getNumLeaves() const override;
	BlockType getLeavesType() const override;
	BlockType getTrunkType() const override;

	PalmTree();
	~PalmTree();
};

#endif