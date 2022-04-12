#ifndef OAK_TREE_H
#define OAK_TREE_H

#include "Tree.h"

class OakTree : public Tree
{
public:
	const Vector3i* getLeaves() override;
	int getTrunkHeight() override;
	int getNumLeaves() override;
	BlockType getLeavesType() override;
	BlockType getTrunkType() override;

	OakTree();
	~OakTree();
};

#endif