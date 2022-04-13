#ifndef OAK_TREE_H
#define OAK_TREE_H

#include "Tree.h"

class OakTree : public Tree
{
public:
	const Vector3i* getLeaves() const override;
	int getTrunkHeight() const override;
	int getNumLeaves() const override;
	BlockType getLeavesType() const override;
	BlockType getTrunkType() const override;

	OakTree();
	~OakTree();
};

#endif