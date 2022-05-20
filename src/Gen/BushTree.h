#ifndef BUSH_TREE_H
#define BUSH_TREE_H

#include "Tree.h"

class BushTree : public Tree
{
public:
	const Vector3i* getLeaves() const override;
	int getTrunkHeight() const override;
	int getNumLeaves() const override;
	BlockType getLeavesType() const override;
	BlockType getTrunkType() const override;

	BushTree();
	~BushTree();
};

#endif